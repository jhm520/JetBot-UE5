// Fill out your copyright notice in the Description page of Project Settings.


#include "JetGameState.h"
#include "../JetBot.h"
#include "JetLandscapeMesh.h"

PRAGMA_DISABLE_OPTIMIZATION
void AJetGameState::OnLandscapeSpawned_Implementation(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh)
{
	LandscapeDataArray.Add(InProcMesh);
	FProcMeshData NewProcMesh = InProcMesh;

	NewProcMesh.bIsActive = true;
	LandscapeDataMap.Add(NewProcMesh.SpawnTransform.GetLocation()*FVector(1,1,0), NewProcMesh);
}

void AJetGameState::OnLandscapeDestroyed_Implementation(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh)
{
	FProcMeshData NewProcMesh = InProcMesh;

	NewProcMesh.bIsActive = false;
	LandscapeDataMap.Add(NewProcMesh.SpawnTransform.GetLocation() * FVector(1, 1, 0), NewProcMesh);

}

bool AJetGameState::GameState_GetNeighborLandscapeData(AJetLandscapeMesh* InLandscape, ECardinalDirection InNeighborDirection, FProcMeshData& OutProcMeshData)
{
	if (!InLandscape)
	{
		return false;
	}

	if (InNeighborDirection == ECardinalDirection::None)
	{
		return false;
	}

	/*AJetLandscapeMesh* FoundLandscape = InLandscape->GetNeighborLandscape(InNeighborDirection);

	if (FoundLandscape)
	{
		return true;
	}*/

	const FTransform MapTransform = InLandscape->GetNeighborLandscapeSpawnTransform(InNeighborDirection);

	FProcMeshData* DataPtr = LandscapeDataMap.Find((InLandscape->GetActorLocation() + MapTransform.GetLocation()) * FVector(1, 1, 0));

	if (DataPtr)
	{
		OutProcMeshData = *DataPtr;
		return true;
	}

	return false;
}
PRAGMA_ENABLE_OPTIMIZATION