// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "../JetBot.h"

#include "JetGameState.generated.h"

/**
 * 
 */
struct FProcMeshData;

UCLASS()
class JETBOT_API AJetGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FProcMeshData> LandscapeDataArray;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TMap<FVector, FProcMeshData> LandscapeDataMap;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Procedural Mesh")
	void OnLandscapeSpawned(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Procedural Mesh")
	void OnLandscapeDestroyed(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh);
};
