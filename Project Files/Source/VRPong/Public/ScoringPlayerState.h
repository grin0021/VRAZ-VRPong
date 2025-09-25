// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ScoringPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class VRPONG_API AScoringPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Variable to track player's total score
	UPROPERTY()
	int PlayerScore = 0;

public:
	// Add score to the player
	UFUNCTION(BlueprintCallable)
	void AddScore(int inScore);

	// Reset the player's score
	UFUNCTION(BlueprintCallable)
	void ResetScore();
	
	// Return the player's score
	UFUNCTION(BlueprintCallable)
	int pongGetScore() { return PlayerScore; }
};
