// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPongGameInstance.h"
#include "CoreMinimal.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
//#include "FStringIntPair.h"

void UVRPongGameInstance::Init()
{
    
}

// This will send the necessary data to our database when the game is closed
void UVRPongGameInstance::Shutdown()
{
    
}

TArray<FStringIntPair> UVRPongGameInstance::ReturnMapLeaderboard(FString mapName)
{
    if (mapName == NeonMapName)
    {
        return neonGlobalHighScores;
    }
    else if (mapName == IndustrialMapName)
    {
        return industrialGlobalHighScores;
    }

    TArray<FStringIntPair> nullArray;

    return nullArray;
}