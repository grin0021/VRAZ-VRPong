// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FStringIntPair.generated.h"

// Struct for a key-value pair
USTRUCT(BlueprintType)
struct FStringIntPair
{
    GENERATED_BODY()

    // Key is an FString
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KeyValuePair")
    FString Key;

    // Value is an int32
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KeyValuePair")
    int32 Value;

    // Default constructor
    FStringIntPair()
        : Key(TEXT("")), Value(0)
    {}

    // Parameterized constructor
    FStringIntPair(FString InKey, int32 InValue)
        : Key(InKey), Value(InValue)
    {}
};