// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetManager.generated.h"

class ATarget;
UCLASS()
class VRPONG_API ATargetManager : public AActor
{
	GENERATED_BODY()

	FTimerHandle SpawnTargetHandle;		// TimerHandle for spawning targets

	float TargetSpawnTime;		// Time it takes to spawn new targets when array is not "full"
	float TargetMaxSpeed;		// Maximum speed the manager can pass to a target when spawning
	float TargetMinSpeed;		// Minimum speed the manager can pass to a target when spawning
	int MaxTargets;				// Maximum number of targets the manager can have at one time
	float difficultyMult;		// A multiplier passed from the GameMode to help determine score value when hit

	TArray<ATarget*> ActiveTargets;		// Container of references to each target that is active in-game

	bool m_bisGameRunning = false;		// Is the game currently running? Game-specific logic does not execute when false

	FVector unitToPlayer;				// Unit Vector representing the direction towards the player character. This is used to modify difficulty based on spawned target distance from player
	APawn* playerPawn;					// Reference to the scene's player
	float maxDistanceFromBound = 0.0f;	// A calculated value of how far a target can be from the front of the spawn boundary, to assist in score calculation

	FVector maxBounds = FVector(0.0f, 0.0f, 0.0f);	// The maximum vertex coordinates of the spawning box
	FVector minBounds = FVector(0.0f, 0.0f, 0.0f);	// THe minimum vertex coordinates of the spawning box

	void SetIsGameRunning(bool inBool);		// Set the minigame in motion
	
public:	
	// Sets default values for this actor's properties
	ATargetManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FVector SpawnOffsetRange;		// Basically a bounding box around the manager that the targets can spawn in

	UPROPERTY(EditAnywhere)
	class UBoxComponent* spawnBounds;	// A box collider with no collision but defines the bounding box that targets can spawn in

	void SpawnTarget();					// Spawn one target
	void DestroyAllTargets();			// Destroy all spawned targets

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTargetSpawnTime(float inTime);		// Set the maximum time it takes to spawn a new target
	void SetTargetMaxSpeed(float inSpeed);		// Set the maximum speed a target can travel
	void SetTargetMinSpeed(float inSpeed);		// Set the minimum speed a target can travel
	void SetMaxTargets(int inNum);				// Set the maximum number of active targets allowed at one time
	void SetDifficultyMultiplier(float inMultiplier);		// Set the difficulty multiplier to pass to targets for score (TODO: bring all base values to this class instead of gamemode)
	float GetDifficultyMultiplier() { return difficultyMult; }		// Getter for the difficulty multiplier to pass to each target

	void DestroyTarget(ATarget* inTarget);	// Destroy a target

	void SpawnFirstSet();	// Spawn the first full set of targets
	
	void StartGame();		// Start the minigame: Allow target manager to execute minigame logic
	void EndGame();			// End the minigame: Handle logic required to smoothly end the game

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATarget> TargetClass;	// Class reference to desired target object to spawn
};
