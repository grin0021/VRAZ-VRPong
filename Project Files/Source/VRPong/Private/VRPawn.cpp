// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Projectile.h"
#include "Haptics/HapticFeedbackEffect_Base.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");

	VRRoot = CreateDefaultSubobject<USceneComponent>("VRRoot");
	VRRoot->SetupAttachment(RootComponent);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("Player Camera");
	PlayerCamera->SetupAttachment(VRRoot);
	PlayerCamera->bLockToHmd = true;

	LeftHand = CreateDefaultSubobject<UMotionControllerComponent>("Left Hand");
	LeftHand->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHand->SetCollisionProfileName("NoCollision");
	LeftHand->SetupAttachment(VRRoot);

	//LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>("Left Hand Mesh");
	//LeftHandMesh->SetupAttachment(LeftHand);
	//LeftHandMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//LeftHandMesh->SetCollisionProfileName("BlockAllDynamic");

	//LeftHandMesh->OnComponentHit.AddDynamic(this, &AVRPawn::LeftHandHaptic);

	//ConstructorHelpers::FObjectFinder<UStaticMesh> leftHandAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	//if (leftHandAsset.Succeeded())
	//{
		//LeftHandMesh->SetStaticMesh(leftHandAsset.Object);
	//}

	RightHand = CreateDefaultSubobject<UMotionControllerComponent>("Right Hand");
	RightHand->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHand->SetCollisionProfileName("NoCollision");
	RightHand->SetupAttachment(VRRoot);

	//RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>("Right Hand Mesh");
	//RightHandMesh->SetupAttachment(RightHand);
	//RightHandMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//RightHandMesh->SetCollisionProfileName("BlockAllDynamic");

	//RightHandMesh->OnComponentHit.AddDynamic(this, &AVRPawn::RightHandHaptic);

	//ConstructorHelpers::FObjectFinder<UStaticMesh> rightHandAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	//if (rightHandAsset.Succeeded())
	//{
		//RightHandMesh->SetStaticMesh(rightHandAsset.Object);
	//}
}

// Play haptic effect on the right hand controller
void AVRPawn::RightHandHaptic(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayHapticEffect(HapticEffect, EControllerHand::Right);
}

// Play haptic effect on the left hand controller
void AVRPawn::LeftHandHaptic(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayHapticEffect(HapticEffect, EControllerHand::Left);
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RightHandMesh)
	{
		CalculatePaddleVelocity(RightHandMesh, rightOldPos, rightVelocity, DeltaTime);
	}
	
	if (LeftHandMesh)
	{
		CalculatePaddleVelocity(LeftHandMesh, leftOldPos, leftVelocity, DeltaTime);
	}
}

// Calculate the velocity of the passed paddle
void AVRPawn::CalculatePaddleVelocity(UStaticMeshComponent* paddle, FVector& inPosition, FVector& inVelocity, float DeltaTime)
{
	FVector newPosition = paddle->GetComponentLocation();
	inVelocity = (newPosition - inPosition) / DeltaTime;
	inPosition = newPosition;
}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Return the velocity of the requested paddle
FVector AVRPawn::GetPaddleVelocity(UStaticMeshComponent* paddle)
{
	if (paddle == RightHandMesh)
		return rightVelocity;
	else
		return leftVelocity;
}

void AVRPawn::SetBallReference(AProjectile* inBall)
{
	GameBall = inBall;
}

void AVRPawn::RecallBallToLeftHand()
{
	if (GameBall)
	{
		GameBall->AggressiveRecall(LeftHand->GetComponentLocation());
	}
}

void AVRPawn::RecallBallToRightHand()
{
	if (GameBall)
	{
		GameBall->AggressiveRecall(RightHand->GetComponentLocation());
	}
}