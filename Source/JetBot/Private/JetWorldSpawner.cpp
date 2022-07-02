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

AJetLandscapeMesh* AJetWorldSpawner::GetCurrentLandscapeMesh(UObject* WorldContextObject)
{
	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState || !GameState->WorldSpawner)
	{
		return nullptr;
	}

	return GameState->WorldSpawner->CurrentLandscape;
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

	//const FProcMeshData& ProcMesh = InLandscapeData.LandscapeArray[0];

	/*AJetLandscapeMesh* NewLandscape = AJetLandscapeMesh::SpawnLandscapeWithData(this, ProcMesh, this->LandscapeProperties, this);

	if (CurrentLandscape)
	{
		CurrentLandscape->SmoothDestroy();
	}

	CurrentLandscape = NewLandscape;*/

	//OnLandscapesFinishedSpawning();

	GameState->AppendLandscapeSpawnQueue(InLandscapeData.LandscapeArray);

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
		int32 Mod = LandscapeProperties.GetVectorScale() / 2;
		FVector VectorMod = FVector(Mod, Mod, 0);
		AsyncCreateLandscapeData(LandscapeCreatedDelegate, GetActorLocation()-VectorMod, LandscapeProperties, this, GameState->LandscapeDataMap, WorldLandscapeVerticesMap);
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

		AJetLandscapeMesh::CreateLandscapesInRadius(InLocation, InLandscapeProperties, OutLandscapeResult.LandscapeArray, OutLandscapeResult.LandscapeDataMap, OutLandscapeResult.LandscapeVerticesMap);

		/*FProcMeshData InOutSuperLandscapeData;

		AJetLandscapeMesh::CreateLandscapeDataInRadius(InLocation, InLandscapeProperties, OutLandscapeResult.LandscapeArray, InOutSuperLandscapeData, OutLandscapeResult.LandscapeDataMap, OutLandscapeResult.LandscapeVerticesMap);*/


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

	/*if (bCreatingLandscapeData)
	{
		PlayerEnteredLandscapeQueue.Add(InLandscape);
		return;
	}*/

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	TWeakObjectPtr<AJetWorldSpawner> WeakPtr = this;
	bCreatingLandscapeData = true;
	AsyncCreateLandscapeData(LandscapeCreatedDelegate, InLandscape->GetActorLocation(), LandscapeProperties, WeakPtr, GameState->LandscapeDataMap, WorldLandscapeVerticesMap);
}
PRAGMA_ENABLE_OPTIMIZATION

void AJetWorldSpawner::OnLandscapesFinishedSpawning_Implementation()
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
