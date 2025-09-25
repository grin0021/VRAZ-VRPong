// Fill out your copyright notice in the Description page of Project Settings.


#include "BallMovementComponent.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UBallMovementComponent::UBallMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UBallMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the component we will use to move the object
	UpdatedComponent = GetOwner()->GetRootComponent();
}


// Called every frame
void UBallMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveUpdatedComponent(DeltaTime);
}

void UBallMovementComponent::MoveUpdatedComponent(float DeltaTime)
{
	// Get the component's current position
	FVector currentPos = UpdatedComponent->GetComponentLocation();

	// Calculate the component's next position, referencing its current position
	FVector nextPos = currentPos + (Velocity * DeltaTime);

	// Set the component's position
	UpdatedComponent->SetWorldLocation(nextPos);

	// If the ball should return to the player
	if (bReturnToPlayer)
	{
		// Get the ball's current position
		FVector ballPos = GetOwner()->GetActorLocation();

		// Get the direction vector from the ball's posiiton to the return target
		FVector dir = (ReturnLocation - ballPos).GetSafeNormal();

		// Set the magnitude of the direction vector to the current velocity magnitude
		dir *= Velocity.Length();

		// Set the current velocity to the calculated direction
		SetVelocity(FMath::Lerp(Velocity, dir, 0.05f));
	}
	// If the ball should return to the "central" location (not the player)
	else if (bReturnToLocation)
	{
		FVector ballPos = GetOwner()->GetActorLocation();

		FVector dir = (SpawnLocation - ballPos).GetSafeNormal();
		dir *= Velocity.Length();

		SetVelocity(FMath::Lerp(Velocity, dir, HomingStrength));
	}

	// If the speed of the ball is more than the minimum allowed speed
	if (Velocity.Length() > MinSpeed)
	{
		// Reduce velocity every frame
		FVector velocityUnit = Velocity.GetSafeNormal();

		Velocity -= (velocityUnit * Drag * DeltaTime);
	}
}

void UBallMovementComponent::SetVelocity(FVector inVelocity)
{
	// Set the velocity
	Velocity = inVelocity;

	// Get the unit vector of the velocity
	FVector velocityUnit = Velocity.GetSafeNormal();
	float excess;

	// If the magnitude of the new velocity is greater than the maximum speed
	if (Velocity.Length() > MaxSpeed)
	{
		// Find out how much excess speed we have (over the maximum velocity)
		excess = Velocity.Length() - MaxSpeed;

		// Create a vector using the excess we found and the direction vector, subtract it from the current velocity
		Velocity -= velocityUnit * excess;
	}
	// If the magnitude of the new velocity is less than the minimum speed
	else if (Velocity.Length() < MinSpeed)
	{
		// Find the excess speed (how much below the minimum speed are we)
		excess = MinSpeed - Velocity.Length();

		// Create a velocity vector using the direction vector and our excess, add it to the current velocity
		Velocity += velocityUnit * excess;
	}
}

void UBallMovementComponent::AddForce(FVector inVelocity)
{
	if ((Velocity.X > 0 && inVelocity.X > 0) || (Velocity.X < 0 && inVelocity.X < 0))
	{
		Velocity.X += inVelocity.X;
	}

	if ((Velocity.Y > 0 && inVelocity.Y > 0) || (Velocity.Y < 0 && inVelocity.Y < 0))
	{
		Velocity.Y += inVelocity.Y;
	}
	
	if ((Velocity.Z > 0 && inVelocity.Z > 0) || (Velocity.Z < 0 && inVelocity.Z < 0))
	{
		Velocity.Z += inVelocity.Z;
	}

	Velocity += inVelocity;

	// After applying force, ensure the speed is not greater than our maximum allowed speed
	if (Velocity.Length() > MaxSpeed)
	{
		float excess = Velocity.Length() - MaxSpeed;

		FVector velocityUnit = Velocity.GetSafeNormal();
		
		Velocity -= velocityUnit * excess;
	}
}

// When ball bounces, which direction will it go?
void UBallMovementComponent::CalculateReflectionVector(FVector inNormal, FVector inVelocity)
{
	FVector temp = ScalarMultiply(2 * FVector::DotProduct(inNormal, inVelocity), inNormal);

	Velocity = inVelocity - temp;
}

// I think this function is redundant I'll do some research
FVector UBallMovementComponent::ScalarMultiply(float inScalarVal, FVector inVector)
{
	FVector temp;

	temp.X = inScalarVal * inVector.X;
	temp.Y = inScalarVal * inVector.Y;
	temp.Z = inScalarVal * inVector.Z;

	return temp;
}

void UBallMovementComponent::SetReturnToLocation(bool inShouldReturn)
{
	bReturnToLocation = inShouldReturn;
}

// Return to the player as fast as possible
void UBallMovementComponent::AggressiveRecall(bool inShouldReturn, FVector inReturnLocation)
{
	bReturnToPlayer = inShouldReturn;
	ReturnLocation = inReturnLocation;

	if (ReturnLocation != FVector::ZeroVector)
	{
		FVector currVelocityUnit = Velocity.GetSafeNormal();
		SetVelocity(currVelocityUnit * MaxSpeed);
	}
}

void UBallMovementComponent::SetReturnTarget(FVector inLoc)
{
	SpawnLocation = inLoc;
}