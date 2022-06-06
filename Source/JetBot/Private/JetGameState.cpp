// Fill out your copyright notice in the Description page of Project Settings.


#include "JetGameState.h"
#include "../JetBot.h"
#include "JetLandscapeMesh.h"

PRAGMA_DISABLE_OPTIMIZATION
void AJetGameState::OnLandscapeSpawned_Implementation(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh)
{
	LandscapeDataArray.Add(InProcMesh);
	LandscapeDataMap.Add(InProcMesh.SpawnTransform.GetLocation()*FVector(1,1,0), InProcMesh);
}

void AJetGameState::OnLandscapeDestroyed_Implementation(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh)
{

}

bool AJetGameState::GameState_GetNeighborLandscapeExists(AJetLandscapeMesh* InLandscape, ECardinalDirection InNeighborDirection)
{
	if (!InLandscape)
	{
		return false;
	}

	if (InNeighborDirection == ECardinalDirection::None)
	{
		return false;
	}

	AJetLandscapeMesh* FoundLandscape = InLandscape->GetNeighborLandscape(InNeighborDirection);

	if (FoundLandscape)
	{
		return true;
	}

	const FTransform MapTransform = InLandscape->GetNeighborLandscapeSpawnTransform(InNeighborDirection);

	FProcMeshData* DataPtr = LandscapeDataMap.Find((InLandscape->GetActorLocation() + MapTransform.GetLocation()) * FVector(1, 1, 0));

	if (DataPtr)
	{
		return true;
	}

	return false;
}
PRAGMA_ENABLE_OPTIMIZATION