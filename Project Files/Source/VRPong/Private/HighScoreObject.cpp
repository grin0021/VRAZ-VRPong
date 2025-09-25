// Fill out your copyright notice in the Description page of Project Settings.


#include "HighScoreObject.h"

FString UHighScoreObject::GetPlayerName()
{
    return scoreInfo.Key;
}

int32 UHighScoreObject::GetScoreValue()
{
    return scoreInfo.Value;
}

int32 UHighScoreObject::GetLeaderboardRanking()
{
    return rank;
}