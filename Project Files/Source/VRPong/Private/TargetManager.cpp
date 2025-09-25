// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Target.h"

// Sets default values
ATargetManager::ATargetManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set default values
	SpawnOffsetRange = FVector(0.0f, 0.0f, 0.0f);
	TargetSpawnTime = 5.0f;
	TargetMaxSpeed = 150.0f;
	TargetMinSpeed = 50.0f;
	MaxTargets = 5;

	spawnBounds = CreateDefaultSubobject<UBoxComponent>("Target Spawn Bounds");
	spawnBounds->SetupAttachment(RootComponent);
	spawnBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	spawnBounds->SetCollisionProfileName("NoCollision");
}

// Called when the game starts or when spawned
void ATargetManager::BeginPlay()
{
	Super::BeginPlay();

	// Gather details of the spawn boundary
	maxBounds = spawnBounds->Bounds.GetBox().Max;
	minBounds = spawnBounds->Bounds.GetBox().Min;

	// Determine which axis points towards the player
	playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	unitToPlayer = (GetActorLocation() - playerPawn->GetActorLocation()).GetSafeNormal();
	unitToPlayer.Z = 0.0f;
	
	if (FMath::Abs(unitToPlayer.X) > FMath::Abs(unitToPlayer.Y))
	{
		unitToPlayer.Y = 0.0f;
		unitToPlayer.X = 1.0f;
	}
	else
	{
		unitToPlayer.X = 0.0f;
		unitToPlayer.Y = 1.0f;
	}

	// Determine the length of the bounding box on the axis pointing to the player
	// This math is axis-agnostic, the unit vector we create will specify the axis for us
	maxDistanceFromBound = ((minBounds - maxBounds) * unitToPlayer).Length();
}

// Spawn the initial set of targets to start the game
void ATargetManager::SpawnFirstSet()
{
	// While the number of targets is less than maximum allowed
	while (ActiveTargets.Num() < MaxTargets)
	{
		SpawnTarget();
	}
}

// Called every frame
void ATargetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the game is running, and the number of targets is less than number allowed
	if (ActiveTargets.Num() < MaxTargets && m_bisGameRunning)
	{
		// If the spawn timer hasn't already started
		if (!GetWorld()->GetTimerManager().IsTimerActive(SpawnTargetHandle))
		{
			// Set a timer to spawn the next target
			GetWorld()->GetTimerManager().SetTimer(SpawnTargetHandle, this, &ATargetManager::SpawnTarget, TargetSpawnTime, false);
		}
	}
}

// Spawn a target
void ATargetManager::SpawnTarget()
{
	// Escape values for upcoming while loops
	bool success = false;
	int attempts = 0;

	do
	{
		// Create a random offset using the offset range variable
		FVector offset;
		offset.X = FMath::RandRange(minBounds.X, maxBounds.X);
		offset.Y = FMath::RandRange(minBounds.Y, maxBounds.Y);
		offset.Z = FMath::RandRange(minBounds.Z, maxBounds.Z);

		// Set a location from this actor's location plus offset
		FVector loc = offset;

		FRotator rot(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnParams;

		// Spawn target from TargetClass
		ATarget* target = Cast<ATarget>(GetWorld()->SpawnActor<ATarget>(TargetClass, offset, GetActorRotation(), SpawnParams));

		float distFactor = FMath::Abs(((maxBounds - target->GetActorLocation()) * unitToPlayer).Length()) / maxDistanceFromBound;
			
		if (distFactor < 0.1f)
			distFactor = 0.1f;

		// Give target random speed within specified range, with a multiplier based on distance (slower if closer to player)
		float speedMultiplier = ((1.0f - distFactor) < 0.1f) ? 0.1f : (1.0f - distFactor);
		target->SetSpeed((FMath::RandRange(TargetMinSpeed, TargetMaxSpeed)) * speedMultiplier);

		// Adjust the scale to be larger if closer to the player, to a maximum of 1.75f
		float scale = ((1.0f + distFactor) > 1.75f) ? 1.75f : (1.0f + distFactor);
		target->SetActorScale3D(FVector(scale));

		// Variable initializion for upcoming while loop
		int dir;
		EDirection direction = EDirection::Right;
		bool dirSuccess = false;

		// Array of possible directions for target to query for movement
		TArray<TEnumAsByte<EDirection>> queryDir;
		queryDir.Add(EDirection::Right);
		queryDir.Add(EDirection::Front);
		queryDir.Add(EDirection::Vertical);

		do
		{
			// Pick a random direction from query array
			dir = queryDir[FMath::RandRange(0, queryDir.Num() - 1)];

			// Set EDirection variable based on random selection
			switch(dir)
			{
				case 0:
					direction = EDirection::Right;
					break;
				case 1:
					direction = EDirection::Front;
					break;
				case 2:
					direction = EDirection::Vertical;
			}

			// Set target's direction (if possible), store success in boolean
			dirSuccess = target->SetDirection(direction);

			// If it was not possible to set the queried direction
			if (!dirSuccess)
			{
				// Remove this direction from future queries
				queryDir.Remove(direction);
			}
		} while (!dirSuccess && !queryDir.IsEmpty());	// Will perform this loop until success or until there is no more directions to query

		// If we ran out of directions to query (all directions failed)
		if (queryDir.IsEmpty())
		{
			// Destroy the created target
			target->Destroy();

			// Increment the attempts taken so far
			attempts++;
		}
		else
		{
			// Give spawned target a reference to this manager
			target->SetManager(this);

			// Give this manager a reference to the spawned target
			ActiveTargets.Add(target);

			target->CalculateScoreValue(TargetMinSpeed, distFactor);

			// We did it!
			success = true;
		}
	} while (!success && attempts < 5);		// Perform this loop until success or until the attempts are too many
}

// Destroy all active targets
void ATargetManager::DestroyAllTargets()
{
	// For every item in the array
	for (int i = 0; i < ActiveTargets.Num(); i++)
	{
		// Destroy the object
		ActiveTargets[i]->Destroy();
	}

	// Remove null values
	ActiveTargets.Empty();
}

// Destroy a single target
void ATargetManager::DestroyTarget(ATarget* inTarget)
{
	// If the active targets array contains this target
	if (ActiveTargets.Contains(inTarget))
	{
		// Remove this target
		ActiveTargets.Remove(inTarget);

		// Destroy the target object
		inTarget->Destroy();
	}
}

// Start executing minigame logic
void ATargetManager::StartGame()
{
	// Set game boolean to true - Allow minigame logic to execute
	SetIsGameRunning(true);
}

void ATargetManager::EndGame()
{
	// Set game boolean to false - Stop executing minigame logic
	SetIsGameRunning(false);

	// Destroy remaining targets in level
	DestroyAllTargets();

	// If a timer was started for spawning a new target
	if (GetWorld()->GetTimerManager().IsTimerActive(SpawnTargetHandle))
	{
		// Clear this timer
		GetWorld()->GetTimerManager().ClearTimer(SpawnTargetHandle);
	}
}

// Set how long it takes for new targets to spawn
void ATargetManager::SetTargetSpawnTime(float inTime)
{
	TargetSpawnTime = inTime;
}

// Set maximum speed this manager can pass to a spawned target
void ATargetManager::SetTargetMaxSpeed(float inSpeed)
{
	TargetMaxSpeed = inSpeed;
}

// Set the minimum speed this manager can pass to a spawned target
void ATargetManager::SetTargetMinSpeed(float inSpeed)
{
	TargetMinSpeed = inSpeed;
}

// Set the maximum number of targets this manager can have at one time
void ATargetManager::SetMaxTargets(int inNum)
{
	MaxTargets = inNum;
}

// Should minigame logic be allowed to execute
void ATargetManager::SetIsGameRunning(bool inBool)
{
	m_bisGameRunning = inBool;
}

void ATargetManager::SetDifficultyMultiplier(float inMultiplier)
{
	difficultyMult = inMultiplier;
}