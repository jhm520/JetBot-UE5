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
struct FLandscapeProperties;
UCLASS()
class JETBOT_API AJetGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FProcMeshData> LandscapeDataArray;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TMap<FVector, FProcMeshData> LandscapeDataMap;

	UPROPERTY()
	TArray<FProcMeshData> LandscapeSpawnQueue;

	UPROPERTY()
	FLandscapeProperties LandscapeSpawnProperties;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Procedural Mesh")
	void OnLandscapeSpawned(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Procedural Mesh")
	void OnLandscapeDestroyed(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	bool GameState_FindLandscapeData(const FVector& InVectorKey, FProcMeshData& OutProcMeshData, const FLandscapeProperties& InLandscapeProperties);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	bool GameState_GetNeighborLandscapeData(const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutNeighborData, int32 InVectorScale);
	
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	bool Static_GameState_GetNeighborLandscapeData(const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutNeighborData, int32 InVectorScale);

	virtual void Tick(const float DeltaSeconds) override;

	UFUNCTION()
	void TickSpawnLandscape();
};
