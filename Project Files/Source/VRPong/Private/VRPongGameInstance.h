// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FStringIntPair.h"
#include "VRPongGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UVRPongGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	TArray<FStringIntPair> ReturnMapLeaderboard(FString mapName);

	UFUNCTION(BlueprintImplementableEvent)
	void SaveLeaderboardValues(const TArray<FStringIntPair>& scores, const FString& saveSlot);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStringIntPair> sessionHighScores;

	// Session ID returned from the API call to attach remaining information to once game ends
	FString SessionID;

	// Map names to match the retool database. These will be used to submit scores for leaderboard purposes
	FString NeonMapName = TEXT("Neon Park");
	FString IndustrialMapName = TEXT("Industrial Hall");
	
	FDateTime startTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FStringIntPair> neonGlobalHighScores;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FStringIntPair> industrialGlobalHighScores;
};
