// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PongModeBase.generated.h"

class UAudioComponent;
UCLASS()
class APongModeBase : public AGameMode
{
	GENERATED_BODY()

	// A timer to help the fading effect of transitioning levels
	FTimerHandle newLevelHandle;

	// Duration of the fade effect of level transition
	float fadeDuration = 1.5f;

	// Total time of the level transition
	float timeToLoad = 2.5f;

	// Function to handle the fading effect of level transition
	void FadeOutWorld();
	
protected:
	virtual void BeginPlay() override;

	// Function to handle logic of transitioning/loading levels
	UFUNCTION(BlueprintCallable, meta = (tooltip="Use level reference for level streaming, otherwise use level name"))
	virtual void EndLevel(TSoftObjectPtr<UWorld> LevelReference, FName LevelName);

	// Function to handle a fade in effect when a level is loaded
	UFUNCTION(BlueprintCallable)
	void StartLevel();

	// Function to quit the application smoothly
	UFUNCTION(BlueprintCallable)
	virtual void QuitGame();

public:
	// Functions to override by child modes, starting and ending the game logic
	virtual void StartGame();
	virtual void EndGame();
};
