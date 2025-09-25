// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRPawn.generated.h"

class UMotionControllerComponent;
class UStaticMeshComponent;
class UCameraComponent;
class UCapsuleComponent;
class USceneComponent;

UCLASS()
class VRPONG_API AVRPawn : public ACharacter
{
	GENERATED_BODY()

	FVector rightOldPos;	// A stored position of the right hand controller component per tick
	FVector leftOldPos;		// A stored position of the left hand controller component per tick

	FVector rightVelocity;	// Velocity of right hand motion component
	FVector leftVelocity;	// Velocity of left hand motion component

	// Function to determine the velocity of each paddle, used to pass force along to the ball during collision
	void CalculatePaddleVelocity(UStaticMeshComponent* paddle, FVector& inPosition, FVector& inVelocity, float DeltaTime);

	// Variable to store ball reference
	class AProjectile* GameBall;

public:
	// Sets default values for this pawn's properties
	AVRPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Root Component of the player pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* VRRoot;

	// Camera component of the player pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* PlayerCamera;

	// Component to track left wand/controller of VR set
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMotionControllerComponent* LeftHand;

	// Component to track right wand/controller of VR set
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMotionControllerComponent* RightHand;

	// Renderable of right hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RightHandMesh;

	// Renderable of left hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LeftHandMesh;

	// Function to handle the ball recalling to the right hand mesh
	UFUNCTION(BlueprintCallable)
	void RecallBallToRightHand();

	// Function to handle the ball recalling to the left hand mesh
	UFUNCTION(BlueprintCallable)
	void RecallBallToLeftHand();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Function to handle the left hand haptic when the paddle hits the ball
	UFUNCTION()
	void LeftHandHaptic(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Function to handle the right hand haptic when the paddle hits the ball
	UFUNCTION()
	void RightHandHaptic(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Returns the tracked velocity of the paddle requested
	FVector GetPaddleVelocity(UStaticMeshComponent* paddle);

	// Gather a reference to the ball from the scene
	void SetBallReference(AProjectile* inBall);

	// A reference to the desired haptic effect for the paddles when they hit the ball
	UPROPERTY(EditAnywhere)
	class UHapticFeedbackEffect_Base* HapticEffect;
};
