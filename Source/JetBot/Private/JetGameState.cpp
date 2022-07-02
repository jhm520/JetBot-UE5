// Fill out your copyright notice in the Description page of Project Settings.


#include "JetGameState.h"
#include "../JetBot.h"
#include "JetLandscapeMesh.h"
#include "JetWorldSpawner.h"

PRAGMA_DISABLE_OPTIMIZATION
void AJetGameState::OnLandscapeSpawned_Implementation(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh)
{
	LandscapeDataArray.Add(InProcMesh);
	FProcMeshData NewProcMesh = InProcMesh;

	NewProcMesh.bIsActive = true;
	LandscapeDataMap.Add(NewProcMesh.SpawnTransform.GetLocation()*FVector(1,1,0), NewProcMesh);

	LandscapeObjectArray.AddUnique(InLandscape);
}

void AJetGameState::OnLandscapeDestroyed_Implementation(AJetLandscapeMesh* InLandscape, const FProcMeshData& InProcMesh)
{
	FProcMeshData NewProcMesh = InProcMesh;

	NewProcMesh.bIsActive = false;
	LandscapeDataMap.Add(NewProcMesh.SpawnTransform.GetLocation() * FVector(1, 1, 0), NewProcMesh);

}

void AJetGameState::AppendLandscapeSpawnQueue(const TArray<FProcMeshData> InLandscapeQueue)
{
	LandscapeSpawnQueue.Append(InLandscapeQueue);
}

void AJetGameState::AppendLandscapeDestroyQueue(const TArray<AJetLandscapeMesh*> InLandscapeQueue)
{
	LandscapeDestroyQueue.Append(InLandscapeQueue);

	for (AJetLandscapeMesh* Neighbor : InLandscapeQueue)
	{
		LandscapeObjectArray.Remove(Neighbor);
	}
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

void AJetGameState::CreateWorldLandscapes(FVector InWorldOrigin, int32 InWorldDimensions, const FLandscapeProperties& InLandscapeProperties)
{
	int32 x = 0;
	int32 y = 0;

	int32 xDim = (InWorldDimensions * 2) + 1;
	int32 yDim = xDim;

	int32 Modifier = xDim / 2;

	for (y = 0; y < yDim; y++)
	{
		for (x = 0; x < xDim; x++)
		{
			FProcMeshData Landscape;

			FVector MapKey = FVector(InWorldOrigin + (InLandscapeProperties.GetVectorScale() * (FVector(x - Modifier, y - Modifier, 0))));

			MapKey = MapKey * FVector(1, 1, 0);

			FTransform NewTileSpawnTransform = FTransform(MapKey);
			TMap<FVector, int32> VectorMap;


			Landscape = AJetLandscapeMesh::CreateLandscapeData(NewTileSpawnTransform, InLandscapeProperties, LandscapeDataMap, VectorMap);

			if (x > 0)
			{
				AJetLandscapeMesh::ZipLandscapeDataWithNeighbor(ECardinalDirection::South, Landscape, InLandscapeProperties, LandscapeDataMap);
			}
			else
			{
				AJetLandscapeMesh::ZipLandscapeDataWithNeighbor(ECardinalDirection::North, Landscape, InLandscapeProperties, LandscapeDataMap);
			}

			if (y > 0)
			{
				AJetLandscapeMesh::ZipLandscapeDataWithNeighbor(ECardinalDirection::West, Landscape, InLandscapeProperties, LandscapeDataMap);
			}
			else
			{
				AJetLandscapeMesh::ZipLandscapeDataWithNeighbor(ECardinalDirection::East, Landscape, InLandscapeProperties, LandscapeDataMap);
			}
			
			AJetLandscapeMesh::OnLandscapeDataCreated(this, Landscape);
		}
	}
}

PRAGMA_ENABLE_OPTIMIZATION

void AJetGameState::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*TickSpawnLandscape();
	TickDestroyLandscape();*/
}

void AJetGameState::TickSpawnLandscapeTransform()
{
	if (LandscapeSpawnTransformQueue.Num() == 0)
	{
		return;
	}

	const FTransform& FirstLandscapeTransform = LandscapeSpawnTransformQueue[0];

	FProcMeshData Landscape;

	FVector MapKey = FVector(FirstLandscapeTransform.GetLocation());

	bool bFoundLandscape = AJetLandscapeMesh::FindLandscapeData(LandscapeDataMap, MapKey, Landscape, LandscapeSpawnProperties);

	if (bFoundLandscape)
	{
		if (Landscape.bIsActive)
		{
			LandscapeSpawnTransformQueue.RemoveAt(0);
			return;
		}
	}
	else
	{
		FTransform NewTileSpawnTransform = FTransform(MapKey);

		TMap<FVector, int32> VectorMap;

		Landscape = AJetLandscapeMesh::CreateLandscapeData(NewTileSpawnTransform, LandscapeSpawnProperties, LandscapeDataMap, VectorMap);
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

	AJetLandscapeMesh::SpawnLandscapeWithData(this, Landscape, LandscapeSpawnProperties, nullptr);

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

void AJetGameState::TickSpawnLandscape()
{
	if (!WorldSpawner)
	{
		return;
	}

	if (LandscapeSpawnQueue.Num() == 0)
	{
		return;
	}

	FProcMeshData& FirstLandscape = LandscapeSpawnQueue[0];

	AJetLandscapeMesh::SpawnLandscapeWithData(this, FirstLandscape, WorldSpawner->LandscapeProperties, WorldSpawner);

	LandscapeSpawnQueue.RemoveAt(0);

	if (LandscapeSpawnQueue.Num() == 0)
	{
		WorldSpawner->OnLandscapesFinishedSpawning();
	}

}

void AJetGameState::TickDestroyLandscape()
{
	if (!WorldSpawner)
	{
		return;
	}

	if (LandscapeDestroyQueue.Num() == 0)
	{
		return;
	}

	AJetLandscapeMesh* FirstLandscape = LandscapeDestroyQueue[0];

	LandscapeDestroyQueue.RemoveAt(0);

	if (FirstLandscape)
	{
		FirstLandscape->Destroy();
	}


	/*if (LandscapeDestroyQueue.Num() == 0)
	{
	}*/
}
