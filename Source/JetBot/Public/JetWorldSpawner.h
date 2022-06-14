// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../JetBot.h"
#include "JetWorldSpawner.generated.h"

USTRUCT(BlueprintType)
struct FOnLandscapeDataCreatedResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	TArray<FProcMeshData> LandscapeArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	TMap<FVector, FProcMeshData> LandscapeDataMap;


	FOnLandscapeDataCreatedResult() {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLandscapeDataCreatedDelegate, const struct FOnLandscapeDataCreatedResult&, OnLandscapeDataCreatedResult);

UCLASS()
class JETBOT_API AJetWorldSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJetWorldSpawner();

	UFUNCTION()
	void OnLandscapeDataCreated(const FOnLandscapeDataCreatedResult& InLandscapeData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	int32 WorldRadius = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	FLandscapeProperties LandscapeProperties;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	bool bSpawnWorldAtBeginPlay = false;

	FOnLandscapeDataCreatedDelegate LandscapeCreatedDelegate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    static void AsyncCreateLandscapeData(FOnLandscapeDataCreatedDelegate Out, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TWeakObjectPtr<AJetWorldSpawner> InWorldSpawner, const TMap<FVector, FProcMeshData>& InLandscapeDataMap);

	UPROPERTY()
	TArray<AJetLandscapeMesh*> PlayerEnteredLandscapeQueue;

	UPROPERTY(Transient)
	bool bCreatingLandscapeData = false;

	void WorldSpawner_OnPlayerEnteredLandscape(AJetLandscapeMesh* InLandscape, ACharacter* InPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "World Spawner")
	void OnWorldSpawned();

	bool bHasWorldSpawned = false;

	UFUNCTION()
	void OnLandscapesFinishedSpawning();
};
