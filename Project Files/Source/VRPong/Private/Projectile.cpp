// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "MotionControllerComponent.h"
#include "Target.h"
#include "VRPawn.h"
#include "PongModeBase.h"
#include "BallMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    RootComponent = SphereComponent;
    SphereComponent->SetCollisionProfileName("BlockAll");
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
    StaticMesh->SetCollisionProfileName("NoCollision");
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StaticMesh->SetupAttachment(RootComponent);

    hitEnvironment = CreateDefaultSubobject<UAudioComponent>("Hit Environment Sound");
    hitEnvironment->SetupAttachment(RootComponent);

    hitPaddle = CreateDefaultSubobject<UAudioComponent>("Hit Paddle Sound");
    hitPaddle->SetupAttachment(RootComponent);

    //Calls OnHit
    SphereComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnComponentHit);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBoxBeginOverlap);

    BallMovementComponent = CreateDefaultSubobject<UBallMovementComponent>("Ball Movement");
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

    // Ensure that the audio components don't play sound on startup
    hitEnvironment->Stop();
    hitPaddle->Stop();

    // Get the player reference from the world, and pass this ball as a reference to the player
    playerRef = Cast<AVRPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    playerRef->SetBallReference(this);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Allow game-loop logic to execute
void AProjectile::StartGame()
{
    m_bgameIsRunning = true;
}

// Recall to the player paddle that is passed
void AProjectile::AggressiveRecall(FVector inLoc)
{
    BallMovementComponent->AggressiveRecall(true, inLoc);
}

// Recall to pre-determined location
void AProjectile::SetReturnLocation(FVector inLoc)
{
    BallMovementComponent->SetReturnTarget(inLoc);
}

void AProjectile::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    FVector normal = Hit.Normal;
    FVector inVelocity = BallMovementComponent->GetVelocity();
    BallMovementComponent->CalculateReflectionVector(normal, inVelocity);

    // If the component hit has the tag "Return to player" -> Typically set pieces, static meshes or targets
    if (OtherComp->ComponentHasTag("ReturnToPlayer"))
    {
        // Enable the ball to home in on the set return-position
        BallMovementComponent->SetReturnToLocation(true);
    }

    // If the actor hit is a target
    if (ATarget* target = Cast<ATarget>(OtherActor))
    {
        // Get the normal of the impact
        FVector vec = Hit.Normal;

        // Critical if hit from the front
        bool crit = UKismetMathLibrary::EqualEqual_VectorVector(vec, target->GetFrontVector(), 0.0001f);

        // Have the target pass score to the player
        target->AddScore(playerRef, crit);

        // Send the target to the shadow realm
        target->CueForRemoval();
    }
    // If a player hits the ball
    else if (AVRPawn* player = Cast<AVRPawn>(OtherComp->GetOwner()))
    {   
        // If the player's paddle was hit
        if (UStaticMeshComponent* paddleMesh = Cast<UStaticMeshComponent>(OtherComp))
        {
            // Apply a force to the movement component equal to the velocity that the paddle was moving
            BallMovementComponent->AddForce(player->GetPaddleVelocity(paddleMesh));

            // Reset homing functionality to let the ball fly in the desired direction
            BallMovementComponent->SetReturnToLocation(false);
            BallMovementComponent->AggressiveRecall(false);

            // Play the audio of ball hitting paddle
            hitPaddle->Play();
        }

        // If this is the first time the ball is hit (start of the game)
        if (!bFirstHit)
        {
            // Set to true so we don't pass this again
            bFirstHit = true;

            // Get the game mode to start the game
            APongModeBase* gameMode = Cast<APongModeBase>(GetWorld()->GetAuthGameMode());
            gameMode->StartGame();
        }
    }
    else
    {
        // Set a location from this actor's location plus offset
		FVector loc = Hit.ImpactPoint;

		FRotator rot(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnParams;

		// Spawn target from TargetClass
		AActor* Particle = Cast<AActor>(GetWorld()->SpawnActor<AActor>(ParticleSpawner, loc, rot, SpawnParams));
    }
    
    // Play the typical ball bounce sound
    hitEnvironment->Play();
}

void AProjectile::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    // Once the ball hits the soft boundary, we want to remove the strong homing effect of the aggressive recall
    // Otherwise, the ball tends to orbit that position if it isn't hit in time, which can cause some disruption in gameplay
    if (OtherComp->ComponentTags.Contains("BallBoundary"))
    {
        BallMovementComponent->AggressiveRecall(false);
    }
}
