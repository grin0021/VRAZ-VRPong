// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "VRPawn.h"
#include "Components/StaticMeshComponent.h"

void AMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Get a reference to the player from the scene
    m_player = Cast<AVRPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AVRPawn::StaticClass()));

    // Ensure we got the player, or tell us there is something wrong
    if (!m_player)
    {
        GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, "Player not found");
    }
}

void AMenuGameMode::SetPaddleMesh(UStaticMesh* inMesh)
{

}

//TArray<UStaticMeshComponent*> AMenuGameMode::GetPlayerMeshComponents()
//{
    // populate a temporary array with the player's mesh components for their paddles
    //TArray<UActorComponent*> tempArray;
    //tempArray = m_player->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Player Custom"));

    //TArray<UStaticMeshComponent*> outMesh;

    // Filter the retrieved components and add the appropriate ones to a final array
    //for (auto meshComp : tempArray)
    //{
        //outMesh.Add(Cast<UStaticMeshComponent>(meshComp));
    //}

    // Return the array
    //return outMesh;
//}