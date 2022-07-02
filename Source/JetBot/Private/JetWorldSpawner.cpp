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
	//bCreatingLandscapeData = false;

	TArray<FVector> VectorArray;
	InLandscapeData.LandscapeDataMap.GenerateKeyArray(VectorArray);

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	if (InLandscapeData.LandscapeArray.Num() == 0)
	{
		OnLandscapesFinishedSpawning();
	}

	if (CurrentLandscape)
	{
		CurrentLandscape->Destroy();
	}

	CurrentLandscape = AJetLandscapeMesh::SpawnLandscapeWithData(this, InLandscapeData.LandscapeArray[0], this->LandscapeProperties, this);

	OnLandscapesFinishedSpawning();

	//GameState->AppendLandscapeSpawnQueue(InLandscapeData.LandscapeArray);

	WorldLandscapeVerticesMap = InLandscapeData.LandscapeVerticesMap;

	//// Spawn all of the new neighbors
	//for (const FProcMeshData& Landscape : InLandscapeData.LandscapeArray)
	//{
	//	/*Landscape.bIsActive = true;
	//	OnLandscapeDataCreated(WorldContextObject, Landscape);*/
	//	AJetLandscapeMesh::SpawnLandscapeWithData(this, Landscape, LandscapeProperties, this);
	//}

	if (PlayerEnteredLandscapeQueue.Num() == 0)
	{
		return;
	}

	AJetLandscapeMesh* EnteredLandscape = PlayerEnteredLandscapeQueue[0];

	if (!EnteredLandscape)
	{
		return;
	}

	WorldSpawner_OnPlayerEnteredLandscape(EnteredLandscape, nullptr, EnteredLandscape->GetActorLocation());

	PlayerEnteredLandscapeQueue.RemoveAt(0);

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

	GameState->WorldSpawner = this;

	LandscapeCreatedDelegate.AddDynamic(this, &AJetWorldSpawner::OnLandscapeDataCreated);
	
	//GameState->CreateWorldLandscapes(GetActorLocation(), WorldRadius, LandscapeProperties);

	if (bSpawnWorldAtBeginPlay)
	{
		/*FProcMeshData FirstLandscape = AJetLandscapeMesh::CreateLandscapeData(FTransform(GetActorLocation()), LandscapeProperties);

		AJetLandscapeMesh::SpawnLandscapeWithData(this, FirstLandscape, LandscapeProperties, this);*/

		//AJetLandscapeMesh::SpawnNeighborLandscapesInRadius(this, GetActorLocation(), LandscapeProperties, this, GameState->LandscapeDataMap);
		bCreatingLandscapeData = true;
		AsyncCreateLandscapeData(LandscapeCreatedDelegate, GetActorLocation(), LandscapeProperties, this, GameState->LandscapeDataMap, WorldLandscapeVerticesMap);
	}
}

// Called every frame
void AJetWorldSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJetWorldSpawner::AsyncCreateLandscapeData(FOnLandscapeDataCreatedDelegate Out, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TWeakObjectPtr<AJetWorldSpawner> InWorldSpawner, const TMap<FVector, FProcMeshData>& InLandscapeDataMap, const TMap<FVector, int32>& InLandscapeVerticesMap)
{

	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [Out, InLocation, InLandscapeProperties, InWorldSpawner, InLandscapeDataMap, InLandscapeVerticesMap]()
	{
		FOnLandscapeDataCreatedResult OutLandscapeResult;

		OutLandscapeResult.LandscapeDataMap = InLandscapeDataMap;
		OutLandscapeResult.LandscapeVerticesMap = InLandscapeVerticesMap;

		//AJetLandscapeMesh::CreateLandscapesInRadius(InLocation, InLandscapeProperties, OutLandscapeResult.LandscapeArray, OutLandscapeResult.LandscapeDataMap);

		FProcMeshData InOutSuperLandscapeData;

		AJetLandscapeMesh::CreateLandscapeDataInRadius(InLocation, InLandscapeProperties, OutLandscapeResult.LandscapeArray, InOutSuperLandscapeData, OutLandscapeResult.LandscapeDataMap, OutLandscapeResult.LandscapeVerticesMap);


		AsyncTask(ENamedThreads::GameThread, [Out, OutLandscapeResult]()
		{

			// We execute the delegate along with the param
			Out.Broadcast(OutLandscapeResult);
		});
	});
}

void AJetWorldSpawner::WorldSpawner_OnPlayerEnteredLandscape(AJetLandscapeMesh* InLandscape, ACharacter* InPlayer, const FVector& InLandscapeSegmentVector)
{
	if (!InLandscape)
	{
		return;
	}

	if (bCreatingLandscapeData)
	{
		PlayerEnteredLandscapeQueue.Add(InLandscape);
		return;
	}

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	TWeakObjectPtr<AJetWorldSpawner> WeakPtr = this;
	bCreatingLandscapeData = true;
	AsyncCreateLandscapeData(LandscapeCreatedDelegate, InLandscape->GetLandscapeCenter() + InLandscapeSegmentVector, LandscapeProperties, WeakPtr, GameState->LandscapeDataMap, WorldLandscapeVerticesMap);
}
PRAGMA_ENABLE_OPTIMIZATION

void AJetWorldSpawner::OnLandscapesFinishedSpawning()
{
	bCreatingLandscapeData = false;

	OnWorldSpawned();

	if (PlayerEnteredLandscapeQueue.Num() == 0)
	{
		return;
	}

	AJetLandscapeMesh* EnteredLandscape = PlayerEnteredLandscapeQueue[0];

	if (!EnteredLandscape)
	{
		return;
	}

	WorldSpawner_OnPlayerEnteredLandscape(EnteredLandscape, nullptr, EnteredLandscape->GetActorLocation());

	PlayerEnteredLandscapeQueue.RemoveAt(0);
}
