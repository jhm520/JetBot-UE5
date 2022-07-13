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
class AJetWorldSpawner;
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
	TArray<AJetLandscapeMesh*> LandscapeObjectArray;

	UPROPERTY()
	TArray<FProcMeshData> LandscapeSpawnQueue;

	UPROPERTY()
	TArray<AJetLandscapeMesh*> LandscapeDestroyQueue;

	UPROPERTY()
	TArray<FTransform> LandscapeSpawnTransformQueue;

	UPROPERTY()
	FLandscapeProperties LandscapeSpawnProperties;

	UFUNCTION()
	void AppendLandscapeSpawnQueue(const TArray<FProcMeshData> InLandscapeQueue);

	UFUNCTION()
	void AppendLandscapeDestroyQueue(const TArray<AJetLandscapeMesh*> InLandscapeQueue);


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

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void CreateWorldLandscapes(FVector InWorldOrigin, int32 InWorldDimensions, const FLandscapeProperties& InLandscapeProperties);


	virtual void Tick(const float DeltaSeconds) override;

	UFUNCTION()
	void TickSpawnLandscapeTransform();

	UFUNCTION()
	void TickSpawnLandscape();

	UFUNCTION()
	void TickDestroyLandscape();

	UPROPERTY(BlueprintReadWrite, Category = "World Spawner")
	AJetWorldSpawner* WorldSpawner = nullptr;
};
