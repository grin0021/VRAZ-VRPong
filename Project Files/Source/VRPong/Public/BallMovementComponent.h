// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BallMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRPONG_API UBallMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	// Pointer reference to the desired component to move each frame
	TObjectPtr<USceneComponent> UpdatedComponent;

	// Ignore MoveSpeed and MoveDirection if this is greater than zero
	FVector Velocity;

	// The location that the ball spawns in at, the ball will home-in on this location to help ensure it is in reach of the player
	FVector SpawnLocation;

	// This location is determined dynamically on player input, the ball will aggressively home-in to this location to keep play time consistent
	FVector ReturnLocation;

	// Function to handle the movement of the desired component
	void MoveUpdatedComponent(float DeltaTime);

	// Function to handle the scalar multiplication of vectors
	FVector ScalarMultiply(float inScalarVal, FVector inVector);

	// Booleans to determine if/where the ball should recall
	bool bReturnToLocation;
	bool bReturnToPlayer;

public:	
	// Sets default values for this component's properties
	UBallMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Value to determine deceleration of the component when above the minimum allowed speed
	UPROPERTY(EditAnywhere)
	float Drag = 2.0f;

	// Maximum speed the component is allowed to move
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 400.0f;

	// Minumum speed the component is allowed to move
	UPROPERTY(EditAnywhere)
	float MinSpeed = 200.0f;

	/* Strength of homing factor when returning to player. Value 0-1, anything below 0.01 tends to be sufficient*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HomingStrength = 0.01f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Math to determine the bounce direction of the ball when it hits a surface
	void CalculateReflectionVector(FVector inNormal, FVector inVelocity);

	// Add force (in form of velocity) when the object is hit by the player paddle
	void AddForce(FVector inVelocity);

	// Set the velocity of the component
	void SetVelocity(FVector inVelocity);

	// Set the desired return location for ball recalling
	void SetReturnTarget(FVector inLoc);

	// Return the velocity of the component
	FVector GetVelocity() {return Velocity;}

	// Get whether the ball should recall to the player
	bool GetShouldReturnToPlayer() {return bReturnToPlayer;}

	// Set whether the ball should recall to location
	void SetReturnToLocation(bool inShouldReturn);

	// Set whether the ball shoudl recall to the player (at max speed)
	void AggressiveRecall(bool inShouldReturn, FVector inReturnLocation = FVector(0.0f, 0.0f, 0.0f));
};