// Fill out your copyright notice in the Description page of Project Settings.


#include "JetWorldSpawner.h"
#include "JetGameState.h"

// Sets default values
AJetWorldSpawner::AJetWorldSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	
	GameState->CreateWorldLandscapes(GetActorLocation(), WorldRadius, LandscapeProperties);

	if (bSpawnWorldAtBeginPlay)
	{
		AJetLandscapeMesh::SpawnNeighborLandscapesInRadius(this, GetActorLocation(), LandscapeProperties, this);
	}
}

// Called every frame
void AJetWorldSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

