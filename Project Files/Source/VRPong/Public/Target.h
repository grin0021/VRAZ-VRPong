// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"

UENUM()
enum EDirection
{
	Right = 0,
	Front,
	Vertical,
};

class ATargetManager;
UCLASS()
class VRPONG_API ATarget : public AActor
{
	GENERATED_BODY()
	
	// Sphere cast in desired direction to check if it is clear of obstacles
	bool CheckPathInDirection(FVector direction);

	// Reference to "owning" target manager
	ATargetManager* targetManager;
	
public:	
	// Sets default values for this actor's properties
	ATarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Boolean flag to mark the target to be destroyed
	UPROPERTY(BlueprintReadOnly)
	bool bCuedForDeath = false;

	// Variable to store the real score value of a target, after calculations are done
	UPROPERTY(BlueprintReadOnly)
	int actualScoreValue = 0;

	// Basic scene component for root of this object
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Root;

	// Renderable of the object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* StaticMesh;

	// Collider representing the movement path of the target
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* pathCollider;

	// Sound to play when score is awarded to the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class UAudioComponent* addScoreSound;

	// Sound to play when a target is hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	UAudioComponent* targetDestroyedSound;

	// Variable storing the direction of the target's movement
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDirection> moveDirection;

	// Length of the movement path
	UPROPERTY(EditAnywhere)
	float maxMovement = 250.0f;

	// Movement speed of the target
	UPROPERTY(EditAnywhere)
	float moveSpeed = 50.0f;

	// Amount the target has moved along its path
	float currMovement = 0.0f;

	// Vector for movement direction
	FVector dirVector;

	// When currMovement = maxMovement, this boolean flips the targets movement
	bool bChangeDirection = false;

	// Takes care of the movement logic of the target
	UFUNCTION()
	void Move(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Adds score to the specified player, specify if it was a critical hit
	void AddScore(AActor* player, bool critHit = false);

	// Get Vector representing the front face of the target
	// TODO: When the static mesh changes, this will likely have to change as well
	FVector GetFrontVector();

	// Set the direction enum and vector for target movement
	// TODO: When the static mesh changes, the direction vector will likely have to change too
	bool SetDirection(EDirection direction);

	// Give the target a reference to a target manager
	void SetManager(ATargetManager* inManager);

	// Set the target speed
	void SetSpeed(float inSpeed);

	// Destroy this target and its references
	void CueForRemoval();

	// Return the flag of whether this target should be destroyed
	bool GetShouldDestroy() {return bCuedForDeath;}

	// Function to set the emissive material colour based on the actual score value of the target
	UFUNCTION(BlueprintImplementableEvent)
	void SetMaterialColor(float factor);

	// Calculate the real score value of the target based on its speed and distance from the player
	void CalculateScoreValue(float baseSpeed, float distance);

	// Score value for a regular hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Score")
	int baseScoreValue = 1;

	// Score value for a critical hit
	// TODO: We need to define what a critical hit should be
	UPROPERTY(EditDefaultsOnly, Category = "Score")
	int criticalScoreValue = 3;

	// Particle effect for when the target is destroyed
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystemComponent* ExplosionParticle;
};
