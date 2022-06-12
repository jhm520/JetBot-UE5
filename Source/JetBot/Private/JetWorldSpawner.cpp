// Fill out your copyright notice in the Description page of Project Settings.


#include "JetWorldSpawner.h"
#include "JetGameState.h"
#include "Async/Async.h"

PRAGMA_DISABLE_OPTIMIZATION
// Sets default values
AJetWorldSpawner::AJetWorldSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AJetWorldSpawner::OnLandscapeDataCreated(const FOnLandscapeDataCreatedResult& InLandscapeData)
{
	//do stuff

	int32 sixnine = InLandscapeData.LandscapeArray.Num();

	TArray<FVector> VectorArray;
	InLandscapeData.LandscapeDataMap.GenerateKeyArray(VectorArray);

	int32 sixninetwo = VectorArray.Num();

	int32 sixninethree = 6969696;

	// Spawn all of the new neighbors
	for (const FProcMeshData& Landscape : InLandscapeData.LandscapeArray)
	{
		/*Landscape.bIsActive = true;
		OnLandscapeDataCreated(WorldContextObject, Landscape);*/
		AJetLandscapeMesh::SpawnLandscapeWithData(this, Landscape, LandscapeProperties, this);
	}

	OnWorldSpawned();

}

// Called when the game starts or when spawned
void AJetWorldSpawner::BeginPlay()
{
	Super::BeginPlay();

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	LandscapeCreatedDelegate.AddDynamic(this, &AJetWorldSpawner::OnLandscapeDataCreated);
	
	//GameState->CreateWorldLandscapes(GetActorLocation(), WorldRadius, LandscapeProperties);

	if (bSpawnWorldAtBeginPlay)
	{
		/*FProcMeshData FirstLandscape = AJetLandscapeMesh::CreateLandscapeData(FTransform(GetActorLocation()), LandscapeProperties);

		AJetLandscapeMesh::SpawnLandscapeWithData(this, FirstLandscape, LandscapeProperties, this);*/

		//AJetLandscapeMesh::SpawnNeighborLandscapesInRadius(this, GetActorLocation(), LandscapeProperties, this, GameState->LandscapeDataMap);

		AsyncCreateLandscapeData(LandscapeCreatedDelegate, GetActorLocation(), LandscapeProperties, this, GameState->LandscapeDataMap);
	}
}

// Called every frame
void AJetWorldSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJetWorldSpawner::AsyncCreateLandscapeData(FOnLandscapeDataCreatedDelegate Out, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TWeakObjectPtr<AJetWorldSpawner> InWorldSpawner, const TMap<FVector, FProcMeshData>& InLandscapeDataMap)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [Out, InLocation, InLandscapeProperties, InWorldSpawner, InLandscapeDataMap]()
	{
		FOnLandscapeDataCreatedResult OutLandscapeResult;

		OutLandscapeResult.LandscapeDataMap = InLandscapeDataMap;

		AJetLandscapeMesh::CreateLandscapesInRadius(InLocation, InLandscapeProperties, OutLandscapeResult.LandscapeArray, OutLandscapeResult.LandscapeDataMap);

		AsyncTask(ENamedThreads::GameThread, [Out, OutLandscapeResult]()
		{

			// We execute the delegate along with the param
			Out.Broadcast(OutLandscapeResult);
		});
	});
}

void AJetWorldSpawner::WorldSpawner_OnPlayerEnteredLandscape(AJetLandscapeMesh* InLandscape, ACharacter* InPlayer)
{
	if (!InLandscape)
	{
		return;
	}

	if (!InPlayer)
	{
		return;
	}

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	TWeakObjectPtr<AJetWorldSpawner> WeakPtr = this;
	AsyncCreateLandscapeData(LandscapeCreatedDelegate, InLandscape->GetActorLocation(), LandscapeProperties, WeakPtr, GameState->LandscapeDataMap);
}
PRAGMA_ENABLE_OPTIMIZATION
