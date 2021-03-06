// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../JetBot.h"
#include "JetLandscapeMesh.h"
#include "JetWorldSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLandscapeDataCreatedDelegate);

class UJetProceduralMeshComponent;

UCLASS()
class JETBOT_API AJetWorldSpawner : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AJetWorldSpawner();

	UPROPERTY()
	bool OnLandscapeDataCreatedMutex = false;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Landscape")
	static AJetLandscapeMesh* GetCurrentLandscapeMesh(UObject* WorldContextObject);

	UPROPERTY(BlueprintReadWrite, Category = "Landscape")
	AJetLandscapeMesh* CurrentLandscape = nullptr;

	UFUNCTION()
	void OnLandscapeDataCreated();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	int32 WorldRadius = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	FLandscapeProperties LandscapeProperties;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	bool bSpawnWorldAtBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	bool bUseJetProceduralMesh = false;

	FOnLandscapeDataCreatedDelegate LandscapeCreatedDelegate;

	UPROPERTY()
	FProcMeshData WorldLandscapeProcMeshData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Procedural Mesh")
	class UJetProceduralMeshComponent* JetLandscapeProcMesh;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Procedural Mesh")
	void CreateLandscapeMeshSectionWithData(UObject* WorldContextObject, const FProcMeshData& InProcMeshData, const FLandscapeProperties& InLandscapeProperties, AJetWorldSpawner* InWorldSpawner);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	int32 GetActorCurrentLandscapeSectionIndex(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void OnCharacterEnteredNewLandscapeSection(ACharacter* InCharacter, int32 InLandscapeSectionIndex);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void OnCharacterExitedLandscapeSection(ACharacter* InCharacter, int32 InExitedLandscapeSectionIndex, int32 InNewLandscapeSectionIndex);

	UPROPERTY()
	int32 CurrentMeshSectionIndex = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void AsyncCreateLandscapeData(FOnLandscapeDataCreatedDelegate Out, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TWeakObjectPtr<AJetWorldSpawner> InWorldSpawner, const TMap<FVector, FProcMeshData>& InLandscapeDataMap, const TMap<FVector, FLandscapeVertexData>& InLandscapeVerticesMap, const TMap<FVector, FVector>& InLandscapeNormalMap, FOnLandscapeDataCreatedResult* InOutWorldLandscapeData, FOnLandscapeDataCreatedResult* InOutNewWorldLandscapeData);

	UPROPERTY()
	TArray<AJetLandscapeMesh*> PlayerEnteredLandscapeQueue;

	UPROPERTY()
	TArray<int32> PlayerEnteredLandscapeIndexQueue;

	UPROPERTY()
	TMap<FVector, int32> WorldLandscapeHeightMap;

	UPROPERTY()
	TMap<FVector, FLandscapeVertexData> WorldLandscapeVertexMap;

	UPROPERTY()
	TMap<FVector, FVector> WorldLandscapeNormalMap;

	UPROPERTY()
	UMaterialInstanceDynamic* WorldLandscapeMaterial = nullptr;

	UPROPERTY(Transient)
	bool bCreatingLandscapeData = false;

	UPROPERTY(Transient)
	TArray<int32> DeleteOldMeshSections;

	//edited by AsyncCreateLandscapeData, contains global world landscape data
	UPROPERTY()
	FOnLandscapeDataCreatedResult WorldLandscapeData;

	//edited by AsyncCreateLandscapeData, loads with new world landscape data
	UPROPERTY()
	FOnLandscapeDataCreatedResult NewWorldLandscapeData;

	void WorldSpawner_OnPlayerEnteredLandscape(AJetLandscapeMesh* InLandscape, ACharacter* InPlayer, const FVector& InLandscapeSegmentVector);

	UFUNCTION(BlueprintImplementableEvent, Category = "World Spawner")
	void OnWorldSpawned();

	UFUNCTION(BlueprintCallable, Category = "World Spawner")
	void UpdateLandscape();

	UPROPERTY(BlueprintReadWrite, Category = "WorldSpawner")
	bool bHasWorldSpawned = false;

	UFUNCTION(BlueprintNativeEvent, Category = "WorldSpawner")
	void OnLandscapesFinishedSpawning();

	void WorldSpawner_TickSpawnLandscape();

	void WorldSpawner_TickDestroyLandscape();


	void WorldSpawner_FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup);

	/** Once async physics cook is done, create needed state */
	void FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup);

	UPROPERTY()
	bool bIsPhysicsAsyncCook = false;


	UPROPERTY()
	TArray<int32> LandscapeProcMeshSectionIndexArray;

	UPROPERTY()
	TArray<int32> LandscapeProcMeshSectionIndexDestroyQueue;

};
