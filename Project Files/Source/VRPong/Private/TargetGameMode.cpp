// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TargetManager.h"
#include "Projectile.h"
#include "Components/AudioComponent.h"

ATargetGameMode::ATargetGameMode()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Initialize base values. All are exposed and modifiable in blueprint
    numTargets = 5;
    targetMaxSpeed = 150.0f;
    targetMinSpeed = 50.0f;
    targetSpawnTime = 2.5f;

    easyMultiplier = 1.0f;
    mediumMultiplier = 1.5f;
    hardMultiplier = 2.0f;

    EndgameBuildup = CreateDefaultSubobject<UAudioComponent>("Endgame Buildup");
    EndgameBuildup->SetupAttachment(RootComponent);

    FinalCountdown = CreateDefaultSubobject<UAudioComponent>("Final Countdown");
    FinalCountdown->SetupAttachment(RootComponent);
}

void ATargetGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Find the target manager placed in the level
    targetManager = Cast<ATargetManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATargetManager::StaticClass()));

    // If the target manager was found
    if (targetManager)
    {
        // Adjust and set target rules based on difficulty
        switch (Difficulty)
        {
            case EDifficulty::Easy:
                targetManager->SetMaxTargets(int(numTargets / easyMultiplier));
                targetManager->SetTargetMaxSpeed(targetMaxSpeed * easyMultiplier);
                targetManager->SetTargetMinSpeed(targetMinSpeed * easyMultiplier);
                targetManager->SetTargetSpawnTime(targetSpawnTime / easyMultiplier);
                targetManager->SetDifficultyMultiplier(easyMultiplier);
                break;
            case EDifficulty::Medium:
                targetManager->SetMaxTargets(int(numTargets / mediumMultiplier));
                targetManager->SetTargetMaxSpeed(targetMaxSpeed * mediumMultiplier);
                targetManager->SetTargetMinSpeed(targetMinSpeed * mediumMultiplier);
                targetManager->SetTargetSpawnTime(targetSpawnTime / mediumMultiplier);
                targetManager->SetDifficultyMultiplier(mediumMultiplier);
                break;
            case EDifficulty::Hard:
                targetManager->SetMaxTargets(int(numTargets / hardMultiplier));
                targetManager->SetTargetMaxSpeed(targetMaxSpeed * hardMultiplier);
                targetManager->SetTargetMinSpeed(targetMinSpeed * hardMultiplier);
                targetManager->SetTargetSpawnTime(targetSpawnTime / hardMultiplier);
                targetManager->SetDifficultyMultiplier(hardMultiplier);
        }
    }
    else
    {
        // Let us know if we forgot to place a target manager in the level
        GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Yellow, "No target manager found in level");
    }

    // Find the ball spawner and get transform information from it
    AActor* spawner = UGameplayStatics::GetActorOfClass(GetWorld(), ProjectileSpawner);
    projSpawnLoc = spawner->GetActorLocation();
    projSpawnRot = spawner->GetActorRotation();

    // Ensure audio components don't play on startup
    EndgameBuildup->Stop();
    FinalCountdown->Stop();

    // Set the game components in their initial positions and values
    SetGameComponents();
}

void ATargetGameMode::Tick(float DeltaTime)
{
    // Get a reference to the world timer manager
    const FTimerManager& timerManager = GetWorld()->GetTimerManager();

    // If the duration timer is active
    if (timerManager.IsTimerActive(gameDurationHandle))
    {
        // Get the time remaining of the duration timer
        float timeRemaining = timerManager.GetTimerRemaining(gameDurationHandle);

        // If the time remaining is less than 10.7f (arbitrary values to fit the sound effects)
        // this is a ticking effect that should count each second from 10 seconds
        if (timeRemaining <= 10.7f)
        {
            // Play the buildup ticking if not already playing
            if (!EndgameBuildup->IsPlaying())
            {
                EndgameBuildup->Play();
            }

            // fadein effect for the sound (needs testing, I'm sure that the audio I imported fades already)
            float volumeMultiplier = (10.0f / timeRemaining) - 1.0f;
            EndgameBuildup->SetVolumeMultiplier(1.0f);
        }

        // If the time remaining is less than 3.5f (arbitrary values to fit the sound effects)
        // this is a countdown effect that should count each second from 3 seconds
        if (timeRemaining <= 3.5f)
        {
            // Play the audio component if not already playing
            if (!FinalCountdown->IsPlaying())
            {
                FinalCountdown->Play();
            }
        }
    }
}

void ATargetGameMode::SpawnProjectile()
{
    // Create spawn parameters for the projectile
    FActorSpawnParameters SpawnParams;

    // Spawn a projectile from the desired class
    projectile = Cast<AProjectile>(GetWorld()->SpawnActor<AProjectile>(ProjectileClass, projSpawnLoc, projSpawnRot, SpawnParams));
    projectile->SetReturnLocation(projSpawnLoc);
}

void ATargetGameMode::StartGame()
{
    if (targetManager)
        targetManager->StartGame();

    if (projectile)
        projectile->StartGame();

    // Start the minigame timer
    GetWorld()->GetTimerManager().SetTimer(gameDurationHandle, this, &ATargetGameMode::EndGame, GameDurationCountdown, false);
}

void ATargetGameMode::EndGame()
{
    // Let the target manager know we've ended the game
    if (targetManager)
        targetManager->EndGame();

    // Destroy the projectile
    if (projectile)
        projectile->Destroy();
    
    ShowSplashScreen(); 

    EndgameBuildup->Stop();
    FinalCountdown->Stop();
}

float ATargetGameMode::GetRemainingTime(FTimerHandle handle)
{
    // If the indicated handle is active
    if (GetWorld()->GetTimerManager().IsTimerActive(handle))
    {
        // Return the remaining time of the handle
        return GetWorld()->GetTimerManager().GetTimerRemaining(handle);
    }
    else
    {
        // Return 0, this handle is finished
        return 0.0f;
    }
}

void ATargetGameMode::SetDifficulty(EDifficulty difficulty)
{
    Difficulty = difficulty;
}

void ATargetGameMode::QuitGame()
{
    Super::QuitGame();

    if (targetManager)
        targetManager->EndGame();

    if (projectile)
        projectile->Destroy();

    GetWorld()->GetTimerManager().ClearTimer(gameDurationHandle);
    GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
}

void ATargetGameMode::EndLevel(TSoftObjectPtr<UWorld> LevelReference, FName LevelName)
{
    Super::EndLevel(LevelReference, LevelName);

    if (targetManager)
        targetManager->EndGame();

    if (projectile)
        projectile->Destroy();

    GetWorld()->GetTimerManager().ClearTimer(gameDurationHandle);
    GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
}

void ATargetGameMode::SetGameComponents()
{
    // Let the target manager know we've ended the game
    if (targetManager)
        targetManager->EndGame();

    // Destroy the projectile
    if (projectile)
        projectile->Destroy();

    SpawnProjectile();
    targetManager->SpawnFirstSet();
    HideSplashScreen();

    // Clear the timer, so it can be restarted
    GetWorld()->GetTimerManager().ClearTimer(gameDurationHandle);
}