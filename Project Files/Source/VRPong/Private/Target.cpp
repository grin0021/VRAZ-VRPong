// Fill out your copyright notice in the Description page of Project Settings.


#include "Target.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "VRPawn.h"
#include "ScoringPlayerState.h"
#include "TargetManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root Component");
	RootComponent = Root;

	pathCollider = CreateDefaultSubobject<UCapsuleComponent>("Path Collider");
	pathCollider->SetupAttachment(RootComponent);

	// Path Collider doesn't provide any physics collision. It's just there for raycasts
	pathCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	pathCollider->SetCollisionProfileName("OverlapAll");
	// Set Radius to the approximate size of the target mesh
	pathCollider->SetCapsuleRadius(24.0f);

	// Static mesh will handle the physics side of collisions with this object
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetCollisionProfileName("BlockAll");
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	addScoreSound = CreateDefaultSubobject<UAudioComponent>("Add Score Sound");
	addScoreSound->SetupAttachment(RootComponent);

	targetDestroyedSound = CreateDefaultSubobject<UAudioComponent>("Target Destroyed Sound");
	targetDestroyedSound->SetupAttachment(RootComponent);

	ExplosionParticle = CreateDefaultSubobject<UParticleSystemComponent>("Explosion Particles");
	ExplosionParticle->SetAutoActivate(false);
	ExplosionParticle->SetupAttachment(StaticMesh);
	ExplosionParticle->SetWorldScale3D(FVector(0.25f, 0.25f, 0.25f));
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();

	// Set the height of the capsule to the length of the movement path it is intended to take
	pathCollider->SetCapsuleHalfHeight(maxMovement - (pathCollider->GetScaledCapsuleRadius() * 2.0f));

	addScoreSound->Stop();
	targetDestroyedSound->Stop();
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetShouldDestroy())
	{
		if (ExplosionParticle->HasCompleted())
		{
			targetManager->DestroyTarget(this);
		}
	}
	else
	{
		Move(DeltaTime);
	}
}

void ATarget::Move(float DeltaTime)
{
	// Add the movement to the current movement offset
	currMovement += moveSpeed * DeltaTime;

	// If our current movement equals or surpasses the maximum amount allowed
	if (currMovement > maxMovement)
	{
		// Change direction for movement
		bChangeDirection = !bChangeDirection;

		// Reset the current movement offset
		currMovement = 0.0f;
	}

	FVector newLoc;

	// Use the change direction boolean to determine which way the target moves
	if (!bChangeDirection)
	{
		// Determine the location of the target for this frame
		newLoc = StaticMesh->GetComponentLocation() + (moveSpeed * DeltaTime * dirVector);
	}
	else
	{
		newLoc = StaticMesh->GetComponentLocation() + (-moveSpeed * DeltaTime * dirVector);
	}

	// Set relative location of mesh using the location vector created
	StaticMesh->SetWorldLocation(newLoc);
}

void ATarget::AddScore(AActor* player, bool critHit)
{
	// Ensure the actor is valid and of the expected type
	if (AVRPawn* VRPlayer = Cast<AVRPawn>(player))
	{
		// Get the player state of the passed actor
		if (AScoringPlayerState* playerState = Cast<AScoringPlayerState>(VRPlayer->GetPlayerState()))
		{
			// If the hit was critical
			if (critHit)
			{
				// Add critical score
				playerState->AddScore(criticalScoreValue);
			}
			else
			{
				// Add basic score
				playerState->AddScore(actualScoreValue * targetManager->GetDifficultyMultiplier());
			}
		}

		addScoreSound->Play();
	}
}

FVector ATarget::GetFrontVector()
{
	return StaticMesh->GetUpVector();
}

bool ATarget::SetDirection(EDirection direction)
{
	// Branch statements to determine which direction
	if (direction == EDirection::Right)
	{
		// Set desired vector for this direction
		dirVector = Root->GetRightVector();

		// Check the direction for collision
		if (!CheckPathInDirection(dirVector))
		{
			// If no collision in path, set variables

			// Set rotation of the path collider to match the path
			FRotator rot = FRotator(0.0f, 0.0f, 90.0f);
			pathCollider->AddRelativeRotation(rot);

			// Adjust the position of the path collider to match the movement path
			FVector newPos = FVector(0.0f, 0.0f, 0.0f);
			newPos.Y = (pathCollider->GetScaledCapsuleHalfHeight() - pathCollider->GetScaledCapsuleRadius()) / GetActorScale3D().Y;

			pathCollider->SetRelativeLocation(newPos);

			// Set direction success
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Left/Right");
			return true;
		}
		else
		{	
			// If collision detected, this direction fails
			return false;
		}
	}
	else if (direction == EDirection::Front)
	{
		// Set desired vector for direction
		dirVector = Root->GetForwardVector();

		// Check for collision along movement path
		if (!CheckPathInDirection(dirVector))
		{
			// Set rotation of path collider to match movement path
			FRotator rot = FRotator(90.0f, 0.0f, 0.0f);
			pathCollider->AddRelativeRotation(rot);

			// Adjust relative position of path collider to better represent movement path
			FVector newPos = FVector(0.0f, 0.0f, 0.0f);
			newPos.X = (pathCollider->GetScaledCapsuleHalfHeight() - pathCollider->GetScaledCapsuleRadius()) / GetActorScale3D().X;

			pathCollider->SetRelativeLocation(newPos);

			// Set direction success
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Front/Back");
			return true;
		}
		else
		{
			// If collision detected, set direction failed
			return false;
		}
	}
	else
	{	
		// Set direction vector for desired direction
		dirVector = Root->GetUpVector();

		// Check for collision in movement path for this direction
		if (!CheckPathInDirection(dirVector))
		{
			// Path collider rotation matches this direction by default

			// Adjust relative position of path collider to better represent movement path
			FVector newPos = FVector(0.0f, 0.0f, 0.0f);
			newPos.Z = (pathCollider->GetScaledCapsuleHalfHeight() - pathCollider->GetScaledCapsuleRadius()) / GetActorScale3D().Z;

			pathCollider->SetRelativeLocation(newPos);

			// Set direction success
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Up/Down");
			return true;
		}
		else
		{
			// If collision detected, set direction fails
			return false;
		}
	}
}

bool ATarget::CheckPathInDirection(FVector direction)
{
	// Initialize start location to location of this actor
	FVector startLoc = GetActorLocation();

	// Initialize end location to the end of the movement path
	FVector endLoc = startLoc + ((maxMovement * direction) * GetActorScale3D());
	GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Yellow, direction.ToString());

	// Object types to query, WorldStatic and Target objects
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);	// World Statics
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery7);	// Other Targets
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery8);	// Projectile

	// Actors to ignore, just this actor
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	// out variable, we don't need this other than for syntax of the function
	FHitResult OutHit;

	// Perform a SphereCast in the movement path
	bool didHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), startLoc, endLoc, 25.0f * GetActorScale3D().X, ObjectTypes, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true);

	// Return hit or miss of spherecast
	return didHit;
}

void ATarget::CueForRemoval()
{
	bCuedForDeath = true;
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetActive(false);
	StaticMesh->SetVisibility(false);
	pathCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	targetDestroyedSound->Play();
	ExplosionParticle->Activate();
}

void ATarget::CalculateScoreValue(float baseSpeed, float distance)
{
	float speedFactor = moveSpeed / baseSpeed;

	// Create a multiplier for the score based on the speed it is moving and its distance from the player
	int scoreMultiplier = (speedFactor / distance) / GetActorScale3D().X;
	if (scoreMultiplier > 6)
		scoreMultiplier = 6;

	actualScoreValue = baseScoreValue * int(scoreMultiplier);
	if (actualScoreValue < baseScoreValue)
		actualScoreValue = baseScoreValue;

	// Set the emissive material colour (green, orange/yellow, red) according the the multiplier value
	SetMaterialColor(scoreMultiplier);
}

void ATarget::SetManager(ATargetManager* inManager)
{
	targetManager = inManager;
}

void ATarget::SetSpeed(float inSpeed)
{
	moveSpeed = inSpeed;
}