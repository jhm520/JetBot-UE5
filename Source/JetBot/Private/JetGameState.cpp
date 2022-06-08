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

bool AJetGameState::GameState_FindLandscapeData(const FVector& InVectorKey, FProcMeshData& OutProcMeshData, const FLandscapeProperties& InLandscapeProperties)
{
	FProcMeshData* DataPtr = LandscapeDataMap.Find(InVectorKey);

	if (!DataPtr)
	{
		return false;
	}

	OutProcMeshData = *DataPtr;

	return true;
}

bool AJetGameState::GameState_GetNeighborLandscapeData(const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutNeighborData, int32 InVectorScale)
{
	/*if (!InLandscape)
	{
		return false;
	}*/

	if (InNeighborDirection == ECardinalDirection::None)
	{
		return false;
	}

	const FTransform MapTransform = AJetLandscapeMesh::GetNeighborLandscapeSpawnTransform(InNeighborDirection, InVectorScale);

	FProcMeshData* DataPtr = LandscapeDataMap.Find((InLandscapeData.SpawnTransform.GetLocation() + MapTransform.GetLocation()) * FVector(1, 1, 0));

	if (DataPtr)
	{
		OutNeighborData = *DataPtr;
		return true;
	}

	return false;
}

bool AJetGameState::Static_GameState_GetNeighborLandscapeData(const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutNeighborData, int32 InVectorScale)
{
	if (InNeighborDirection == ECardinalDirection::None)
	{
		return false;
	}

	const FTransform MapTransform = AJetLandscapeMesh::GetNeighborLandscapeSpawnTransform(InNeighborDirection, InVectorScale);

	FProcMeshData* DataPtr = LandscapeDataMap.Find((InLandscapeData.SpawnTransform.GetLocation() + MapTransform.GetLocation()) * FVector(1, 1, 0));

	if (DataPtr)
	{
		OutNeighborData = *DataPtr;
		return true;
	}

	return false;
}

PRAGMA_ENABLE_OPTIMIZATION

void AJetGameState::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickSpawnLandscape();
}

void AJetGameState::TickSpawnLandscape()
{
	if (LandscapeSpawnTransformQueue.Num() == 0)
	{
		return;
	}

	const FTransform& FirstLandscapeTransform = LandscapeSpawnTransformQueue[0];

	FProcMeshData Landscape;

	FVector MapKey = FVector(FirstLandscapeTransform.GetLocation());

	bool bFoundLandscape = AJetLandscapeMesh::FindLandscapeData(this, MapKey, Landscape, LandscapeSpawnProperties);

	if (bFoundLandscape)
	{
		if (Landscape.bIsActive)
		{
			LandscapeSpawnTransformQueue.RemoveAt(0);
			return;
		}
		else
		{
			int32 sixnine = 69;
		}
	}
	else
	{
		FTransform NewTileSpawnTransform = FTransform(MapKey);

		Landscape = AJetLandscapeMesh::CreateLandscapeData(NewTileSpawnTransform, LandscapeSpawnProperties);
	}

	


	/*if (x > 0)
	{
		ZipLandscapeDataWithNeighbor(this, ECardinalDirection::South, Landscape, LandscapeSpawnProperties);
	}
	else
	{
		ZipLandscapeDataWithNeighbor(this, ECardinalDirection::North, Landscape, LandscapeSpawnProperties);
	}

	if (y > 0)
	{
		ZipLandscapeDataWithNeighbor(this, ECardinalDirection::West, Landscape, LandscapeSpawnProperties);
	}
	else
	{
		ZipLandscapeDataWithNeighbor(this, ECardinalDirection::East, Landscape, LandscapeSpawnProperties);
	}*/

	Landscape.bIsActive = true;

	AJetLandscapeMesh::OnLandscapeDataCreated(this, Landscape);

	AJetLandscapeMesh::SpawnLandscapeWithData(this, Landscape, LandscapeSpawnProperties);

	LandscapeSpawnTransformQueue.RemoveAt(0);


	/*LocalLandscapeSpawnQueue.Add(Landscape);*/

	/*if (LandscapeSpawnQueue.Num() == 0)
	{
		return;
	}

	const FProcMeshData& FirstLandscape = LandscapeSpawnQueue[0];

	AJetLandscapeMesh::SpawnLandscapeWithData(this, FirstLandscape, LandscapeSpawnProperties);

	LandscapeSpawnQueue.RemoveAt(0);*/

}
