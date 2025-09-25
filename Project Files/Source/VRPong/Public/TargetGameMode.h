// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PongModeBase.h"
#include "TargetGameMode.generated.h"

// Enum for difficulty selection
UENUM(BlueprintType)
enum EDifficulty
{
	Easy = 0,
	Medium,
	Hard
};

UCLASS()
class VRPONG_API ATargetGameMode : public APongModeBase
{
	GENERATED_BODY()

	ATargetGameMode();

	TEnumAsByte<EDifficulty> Difficulty = EDifficulty::Medium;		// Variable to store the game difficulty

	// References to important game objects
	class ATargetManager* targetManager;
	class AProjectile* projectile;

	// Transform parameters for projectile spawning
	FVector projSpawnLoc;
	FRotator projSpawnRot;

	void SpawnProjectile();			// Spawn the game ball
	
	bool m_bisGameSet = false;		// Boolean to check if game components are set up
	
protected:
	void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Function to handle displaying the splash screen at the end of the round (showing score, and options to restart or quit)
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSplashScreen();

	// Function to handle hiding the splash screen, if player decides to restart after a round
	UFUNCTION(BlueprintImplementableEvent)
	void HideSplashScreen();

	// Number of targets allowed, passed to target manager
	UPROPERTY(EditDefaultsOnly, Category = "Base Game Values")
	int numTargets;

	// Maximum speed that the target manager can pass to spawned targets
	UPROPERTY(EditDefaultsOnly, Category = "Base Game Values")
	float targetMaxSpeed;
    
	// Minimum speed that the target manager can pass to spawned targets
	UPROPERTY(EditDefaultsOnly, Category = "Base Game Values")
	float targetMinSpeed;

	// Minimum time allowed before the target manager can spawn a new target
	UPROPERTY(EditDefaultsOnly, Category = "Base Game Values")
	float targetSpawnTime;

	// Multiplier or modifier for easy difficulty, to affect above values
	UPROPERTY(EditDefaultsOnly, Category = "Difficulty Modifiers")
	float easyMultiplier;

	// Multiplier or modifier for medium difficulty, to affect above values
	UPROPERTY(EditDefaultsOnly, Category = "Difficulty Modifiers")
	float mediumMultiplier;

	// Multiplier or modifier for hard difficulty, to affect above values
	UPROPERTY(EditDefaultsOnly, Category = "Difficulty Modifiers")
	float hardMultiplier;

	// Timer when level loads, before the minigame starts
	UPROPERTY(EditDefaultsOnly, Category = "Game Timers")
	float preGameCountdown = 5.0f;

	// How long the minigame lasts once started
	UPROPERTY(EditDefaultsOnly, Category = "Game Timers")
	float GameDurationCountdown = 30.0f;

	// Desired class of the projectile to spawn during play
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Spawning")
	TSubclassOf<class AProjectile> ProjectileClass;

	// Class reference to the ball spawner in the level, to know where to spawn the ball
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Spawning")
	TSubclassOf<class AActor> ProjectileSpawner;

	// Audio Component handling the growing "ticking" sound when the game is in the last seconds of play
	UPROPERTY(EditDefaultsOnly, Category = "Game Audio")
	class UAudioComponent* EndgameBuildup;

	// Audio component handling sound to indicate the final three seconds (vocal, or otherwise)
	UPROPERTY(EditDefaultsOnly, Category = "Game Audio")
	UAudioComponent* FinalCountdown;
	
public:
	// Set desired difficulty for the level
	UFUNCTION(BlueprintCallable)
	void SetDifficulty(EDifficulty difficulty);

	// Start the minigame
	UFUNCTION(BlueprintCallable)
	virtual void StartGame() override;

	// End the minigame
	UFUNCTION(BlueprintCallable)
	virtual void EndGame() override;

	// Function to setup all game components in their inital positions and values
	UFUNCTION(BlueprintCallable)
	void SetGameComponents();

	// Get the remaining time of one of the indicated handle
	UFUNCTION(BlueprintCallable)
	float GetRemainingTime(FTimerHandle handle);

	// Handle logic for smooth exiting of the game
	virtual void QuitGame() override;

	// Handle the logic for smoothing exiting a level and loading the next
	virtual void EndLevel(TSoftObjectPtr<UWorld> LevelReference, FName LevelName) override;

	// Timer handle for the pre-game duration
	UPROPERTY(BlueprintReadOnly, Category = "Game Timers")
	FTimerHandle preGameTimerHandle;

	// Timer handle for the duration of the minigame
	UPROPERTY(BlueprintReadOnly, Category = "Game Timers")
	FTimerHandle gameDurationHandle;
};
