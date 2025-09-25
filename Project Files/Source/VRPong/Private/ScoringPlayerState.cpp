// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoringPlayerState.h"

void AScoringPlayerState::BeginPlay()
{
    Super::BeginPlay();

    // Could retrieve saved high score here
}

void AScoringPlayerState::AddScore(int inScore)
{
    PlayerScore += inScore;
}

void AScoringPlayerState::ResetScore()
{
    // Some high score saving logic can go here before resetting

    PlayerScore = 0;
}