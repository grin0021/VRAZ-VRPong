// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Projectile.generated.h"

UCLASS()
class VRPONG_API AProjectile : public AActor
{
	GENERATED_BODY()

	// Reference to player in the scene
	class AVRPawn* playerRef;
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	// Should gameplay execute?
	bool m_bgameIsRunning = false;

protected:
	// Custom movement component
	UPROPERTY(EditAnywhere)
	class UBallMovementComponent* BallMovementComponent;

	// Collision component of the projectile
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;

	// Renderable component of the projectile
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* StaticMesh;

	// Audio component to play ball hits environment sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio Setup")
	class UAudioComponent* hitEnvironment;

	// Audio component to play ball hits player paddle sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio Setup")
	UAudioComponent* hitPaddle;

	// Handles spawning particles whenever the ball collides with anything
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	TSubclassOf<AActor> ParticleSpawner;

	// Flag to determine if the ball has been hit yet. Game starts when ball is hit for the first time
	UPROPERTY(BlueprintReadOnly)
	bool bFirstHit = false;

	//FVector CurrentDirection;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function to execute when the projectile hits another object
	UFUNCTION()
    void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Function to execute when the projectile overlaps with a trigger
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Functions to handle recalling the ball in response to input or collision with certain objects
	void AggressiveRecall(FVector inLoc);
	void SetReturnLocation(FVector inLoc);

	// Start gameplay
	void StartGame();
};
