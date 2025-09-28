// Fill out your copyright notice in the Description page of Project Settings.


#include "HighScoreObject.h"

FString UHighScoreObject::GetPlayerName()           //
{                                                   //
    return scoreInfo.Key;                           //
}                                                   //
                                                    //
int32 UHighScoreObject::GetScoreValue()             //
{                                                   //      Small class that stores information to push to the leaderboard database, and/or store what is pulled
    return scoreInfo.Value;                         //      from the database. Database is hosted on Retool and I was excited to use some Python and SQL basics
}                                                   //      in a Retool workflow to create this system.
                                                    //
int32 UHighScoreObject::GetLeaderboardRanking()     //
{                                                   //
    return rank;                                    //
}                                                   //