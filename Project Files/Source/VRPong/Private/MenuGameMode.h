// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PongModeBase.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AMenuGameMode : public APongModeBase
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;

	// Set the mesh of the player's paddle during customization
	UFUNCTION(BlueprintCallable)
	void SetPaddleMesh(UStaticMesh* inMesh);

	// Get the mesh components from the player
	//UFUNCTION(BlueprintCallable)
	//TArray<UStaticMeshComponent*> GetPlayerMeshComponents();

	// Gather an array of meshes to be able to apply to the player's paddles
	UPROPERTY(EditAnywhere, Category = "Player Customization")
	TArray<UStaticMesh*> PaddleMeshes;

private:
	// Reference to the player in the scene
	class AVRPawn* m_player;
};