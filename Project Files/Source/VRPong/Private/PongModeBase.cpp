// Fill out your copyright notice in the Description page of Project Settings.


#include "PongModeBase.h"
#include "Components/AudioComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void APongModeBase::BeginPlay()
{
    Super::BeginPlay();

    StartLevel();
}

void APongModeBase::FadeOutWorld()
{
    // Apply a fading effect to the camera when a level is being exited
    if (GetWorld())
    {
        APlayerCameraManager* cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

        if (cameraManager)
        {
            cameraManager->StartCameraFade(0.0f, 1.0f, fadeDuration, FLinearColor::Black, true, true);
        }
    }
}

void APongModeBase::StartGame()
{

}

void APongModeBase::EndGame()
{
    
}

void APongModeBase::EndLevel(TSoftObjectPtr<UWorld> LevelReference, FName LevelName)
{
    // Apply fade out effect
    FadeOutWorld();

    // Declare timer delegate
    FTimerDelegate TimerDel;

    // Start loading the new level
    FLatentActionInfo info;
    UGameplayStatics::LoadStreamLevel(GetWorld(), LevelName, false, false, info);

    // If the level soft object reference exists
    if (LevelReference)
    {
        // Define a lambda for the timer delegate based on soft reference
        TimerDel.BindLambda([this, LevelReference]()
        {
            UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LevelReference, true);
        });
    }
    else
    {
        // define a lambda for the timer delegate using the name that is passed 
        TimerDel.BindLambda([this, LevelName]()
        {
            UGameplayStatics::OpenLevel(GetWorld(), LevelName, true);
        });
    }

    // Set a timer to execute the lambda function on completion
    GetWorld()->GetTimerManager().SetTimer(newLevelHandle, TimerDel, timeToLoad, false);
}

void APongModeBase::StartLevel()
{
    // Fade in the camera when a level is entered
    if (GetWorld())
    {
        APlayerCameraManager* cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

        if (cameraManager)
        {
            cameraManager->StartCameraFade(1.0f, 0.0f, fadeDuration * 1.5f, FLinearColor::Black, true, true);
        }
    }
}

void APongModeBase::QuitGame()
{
    // Apply the fade out effect to the camera
    FadeOutWorld();

    // Define a timer delegate and bind a lambda to quit the game
    FTimerDelegate TimerDel;
    TimerDel.BindLambda([this]()
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
    });
    
    // Set a timer to call the lambda function on completion
    GetWorld()->GetTimerManager().SetTimer(newLevelHandle, TimerDel, timeToLoad, false);
}