// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FStringIntPair.h"
#include "HighScoreObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class VRPONG_API UHighScoreObject : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FStringIntPair scoreInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 rank;

public:
	UFUNCTION(BlueprintCallable)
	FString GetPlayerName();

	UFUNCTION(BlueprintCallable)
	int32 GetScoreValue();

	UFUNCTION(BlueprintCallable)
	int32 GetLeaderboardRanking();
};
