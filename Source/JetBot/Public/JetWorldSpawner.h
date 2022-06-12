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

//class FCreateLandscapeDataTask : public FNonAbandonableTask
//{
//	friend class FAutoDeleteAsyncTask<FCreateLandscapeDataTask>;
//	public:
//		FCreateLandscapeDataTask(const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, AJetWorldSpawner* InWorldSpawner, const TMap<FVector, FProcMeshData>& InLandscapeDataMap) : Location(InLocation), LandscapeProperties(InLandscapeProperties), WorldSpawner(InWorldSpawner), LandscapeDataMap(InLandscapeDataMap){}
//
//	protected:
//		const FVector Location;
//		const FLandscapeProperties LandscapeProperties;
//		const AJetWorldSpawner* WorldSpawner;
//		FOnLandscapeDataCreatedDelegate OnLandscapeDataCreated;
//		TMap<FVector, FProcMeshData> LandscapeDataMap;
//
//	void DoWork()
//	{ // Place the Async Code here. This function runs automatically.
//
//		//TArray<FProcMeshData> OutLandscapeArray;
//		//TMap<FVector, FProcMeshData> OutLandscapeDataMap;
//
//		//FOnLandscapeDataCreatedResult LandscapeResult;
//
//
//		//LandscapeResult.LandscapeArray = OutLandscapeArray;
//		//LandscapeResult.LandscapeDataMap = OutLandscapeDataMap;
//
//		////OnLandscapeDataCreated.BindStatic(&AJetWorldSpawner::OnLandscapeDataCreated);
//		//OnLandscapeDataCreated.Execute(LandscapeResult);
//	}
//
//		// This next section of code needs to be here. Not important as to why.
//
//	FORCEINLINE TStatId GetStatId() const
//	{
//		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
//	}
//
//};

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

};
