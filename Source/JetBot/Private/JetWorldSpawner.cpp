// Fill out your copyright notice in the Description page of Project Settings.


#include "JetWorldSpawner.h"
#include "JetGameState.h"
#include "Async/Async.h"

// Sets default values
AJetWorldSpawner::AJetWorldSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AJetWorldSpawner::OnLandscapeDataCreated(const FOnLandscapeDataCreatedResult& InLandscapeData)
{
	//do stuff

	int32 sixnine = 69;
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
	
	GameState->CreateWorldLandscapes(GetActorLocation(), WorldRadius, LandscapeProperties);

	if (bSpawnWorldAtBeginPlay)
	{
		AJetLandscapeMesh::SpawnNeighborLandscapesInRadius(this, GetActorLocation(), LandscapeProperties, this, GameState->LandscapeDataMap);

		AsyncCreateLandscapeData(LandscapeCreatedDelegate, GetActorLocation(), LandscapeProperties, this, GameState->LandscapeDataMap);
		//FAutoDeleteAsyncTask<FCreateLandscapeDataTask>* CreateLandscapeDataTask = new FAutoDeleteAsyncTask<FCreateLandscapeDataTask>(GetActorLocation(), LandscapeProperties, GameState->LandscapeDataMap);

		//if (CreateLandscapeDataTask)
		//{
		//	//CreateLandscapeDataTask->Task.OnLandscapeDataCreated.BindStatic(&AJetWorldSpawner::OnLandscapeDataCreated);
		//	CreateLandscapeDataTask->StartBackgroundTask();
		//}
	}
}

// Called every frame
void AJetWorldSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJetWorldSpawner::AsyncCreateLandscapeData(FOnLandscapeDataCreatedDelegate Out, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TWeakObjectPtr<AJetWorldSpawner> InWorldSpawner, const TMap<FVector, FProcMeshData>& InLandscapeDataMap)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [Out, InLocation, InLandscapeProperties, InWorldSpawner]()
	{
		FOnLandscapeDataCreatedResult OutLandscapeResult;
		AsyncTask(ENamedThreads::GameThread, [Out, OutLandscapeResult]()
		{
			// We execute the delegate along with the param
			Out.Broadcast(OutLandscapeResult);
		});
	});
}

