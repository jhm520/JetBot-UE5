// Fill out your copyright notice in the Description page of Project Settings.


#include "JetLandscapeMesh.h"
#include "Kismet/GameplayStatics.h"
#include "../JetBot.h"
#include "JetGameState.h"
#include "JetWorldSpawner.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"

PRAGMA_DISABLE_OPTIMIZATION
namespace LandscapeStatics
{
	FVector North = FVector(1, 0, 0);
	FVector Northeast = FVector(1, 1, 0);
	FVector Northwest = FVector(1, -1, 0);
	FVector West = FVector(0, -1, 0);
	FVector East = FVector(0, 1, 0);
	FVector South = FVector(-1, 0, 0);
	FVector Southwest = FVector(-1, -1, 0);
	FVector Southeast = FVector(-1, 1, 0);

	TMap<ECardinalDirection, FTransform> NeighborSpawnTransformMap =
	{
		{ECardinalDirection::North, FTransform(LandscapeStatics::North)},
		{ECardinalDirection::Northeast, FTransform(LandscapeStatics::Northeast)},
		{ECardinalDirection::Northwest, FTransform(LandscapeStatics::Northwest)},
		{ECardinalDirection::West, FTransform(LandscapeStatics::West)},
		{ECardinalDirection::East, FTransform(LandscapeStatics::East)},
		{ECardinalDirection::Southeast, FTransform(LandscapeStatics::Southeast)},
		{ECardinalDirection::South, FTransform(LandscapeStatics::South)},
		{ECardinalDirection::Southwest, FTransform(LandscapeStatics::Southwest)}
	};

}

AJetLandscapeMesh::AJetLandscapeMesh()
{
	//
	PrimaryActorTick.bCanEverTick = false;

	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->SetupAttachment(RootComponent);

	SphereCollider->SetSphereRadius(100.0f);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionObjectType(ECollisionChannel::ECC_EngineTraceChannel2);

	SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	SphereCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	SphereCollider->SetVisibility(false);
	SphereCollider->SetHiddenInGame(true);
}

void AJetLandscapeMesh::OnPlayerEnteredLandscape(ACharacter* InPlayer)
{
	if (!InPlayer || !InPlayer->IsLocallyControlled())
	{
		return;
	}

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	if (WorldSpawner)
	{
		WorldSpawner->WorldSpawner_OnPlayerEnteredLandscape(this, InPlayer);

	}


	//SpawnNeighborLandscapesInRadius();
	//SpawnNeighborLandscapesInRadius(this, GetActorLocation(), LandscapeProperties, WorldSpawner, GameState->LandscapeDataMap);
}

void AJetLandscapeMesh::OnPlayerExitedLandscape(ACharacter* InPlayer, AJetLandscapeMesh* NewLandscape)
{

	if (!InPlayer || !InPlayer->IsLocallyControlled())
	{
		return;
	}

	if (!NewLandscape)
	{
		return;
	}

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	TArray<AJetLandscapeMesh*> OldNeighbors;

	int32 MaxDist = LandscapeProperties.NeighborSpawnRadius * LandscapeProperties.GetVectorScale();
	for (AJetLandscapeMesh* LandscapeMesh : GameState->LandscapeObjectArray)
	{
		FVector Dist = NewLandscape->GetActorLocation() - LandscapeMesh->GetActorLocation();

		if (FMath::Abs(Dist.X) > MaxDist)
		{
			OldNeighbors.Add(LandscapeMesh);
		}
		else if (FMath::Abs(Dist.Y) > MaxDist)
		{
			OldNeighbors.Add(LandscapeMesh);
		}
	}

	/*TArray<AJetLandscapeMesh*> OldNeighbors = GetAllNeighborLandscapes();

	TArray<AJetLandscapeMesh*> NewNeighbors = NewLandscape->GetAllNeighborLandscapes();

	bHasSpawnedNeighborLandscapes = false;

	OldNeighbors.Remove(NewLandscape);

	for (AJetLandscapeMesh* Land : NewNeighbors)
	{
		OldNeighbors.Remove(Land);
	}*/

	GameState->AppendLandscapeDestroyQueue(OldNeighbors);

	/*for (AJetLandscapeMesh* OldNeighbor : OldNeighbors)
	{
		if (OldNeighbor)
		{
			OldNeighbor->Destroy();
		}
	}*/
}

void AJetLandscapeMesh::AppendLandscapeSpawnQueue(const TArray<FProcMeshData>& InLandscapeNeighborSpawnQueue)
{
	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	GameState->LandscapeSpawnProperties = LandscapeProperties;
	GameState->LandscapeSpawnQueue.Append(InLandscapeNeighborSpawnQueue);
}

void AJetLandscapeMesh::AppendLandscapeSpawnQueue_Transform(const TArray<FTransform>& InLandscapeSpawnQueue)
{
	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	GameState->LandscapeSpawnProperties = LandscapeProperties;
	GameState->LandscapeSpawnTransformQueue.Append(InLandscapeSpawnQueue);
}

FProcMeshData AJetLandscapeMesh::CreateLandscapeData(const FTransform& InSpawnTransform, const FLandscapeProperties& InLandscapeProperties, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap)
{
	FProcMeshData OutProcMeshData;

	FProcMeshFaceVertexMap LandscapeVertexMap;
	OutProcMeshData.FaceVertexMapArray.Add(LandscapeVertexMap);

	OutProcMeshData.SpawnTransform = InSpawnTransform;
	//OutProcMeshData.Vertices = CreateLandscapeVertexArray(InLandscapeProperties, OutProcMeshData, InOutLandscapeDataMap);

	OutProcMeshData.Vertices = CreateLandscapeVertexArrayNew(InLandscapeProperties, OutProcMeshData, InOutLandscapeDataMap);


	OutProcMeshData.UVs = CreateLandscapeUVArray(InLandscapeProperties.LandscapeSize, InLandscapeProperties.TileSize, InLandscapeProperties.HeightVariation);

	OutProcMeshData.Triangles = CreateLandscapeTriangleArray(OutProcMeshData.FaceVertexMapArray[0].VertexIndexMap, InLandscapeProperties.LandscapeSize, InLandscapeProperties.TileSize, InLandscapeProperties.HeightVariation);

	return OutProcMeshData;
}

void AJetLandscapeMesh::ZipLandscapeDataWithNeighbors(UObject* WorldContextObject, FProcMeshData& InOutLandscapeData, const FLandscapeProperties& InLandscapeProperties)
{
	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	for (dir = 0; dir < MaxDirection + 1; dir++)
	{
		ECardinalDirection CardDir = (ECardinalDirection)dir;

		ZipLandscapeDataWithNeighbor(CardDir, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
	}
}

void AJetLandscapeMesh::ZipLandscapeDataWithNeighbor(ECardinalDirection InNeighborDirection, FProcMeshData& InOutLandscapeData, const FLandscapeProperties& InLandscapeProperties, TMap<FVector, FProcMeshData>& InLandscapeDataMap)
{
	FVector2D ZippeeVector;
	FVector2D ZipperVector;

	bool bCorner = false;

	FProcMeshData ZipperData;

	bool bGotNeighborData = GetNeighborLandscapeData(InOutLandscapeData, InNeighborDirection, ZipperData, InLandscapeProperties.GetVectorScale(), InLandscapeDataMap);

	if (!bGotNeighborData)
	{
		return;
	}


	if (InNeighborDirection == ECardinalDirection::West)
	{
		for (int32 i = 0; i < InLandscapeProperties.LandscapeSize + 1; i++)
		{
			int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(i, 0, 0));

			int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(i, InLandscapeProperties.LandscapeSize, 0));

			FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

			FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

			InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (InNeighborDirection == ECardinalDirection::East)
	{
		for (int32 i = 0; i < InLandscapeProperties.LandscapeSize + 1; i++)
		{
			int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(i, InLandscapeProperties.LandscapeSize, 0));

			int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(i, 0, 0));

			FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

			FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

			FVector ZippeeWorld = InOutLandscapeData.SpawnTransform.TransformPosition(ZippeeRelative);

			InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (InNeighborDirection == ECardinalDirection::North)
	{
		for (int32 i = 0; i < InLandscapeProperties.LandscapeSize + 1; i++)
		{

			int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(InLandscapeProperties.LandscapeSize, i, 0));

			int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(0, i, 0));

			FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

			FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

			InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (InNeighborDirection == ECardinalDirection::South)
	{
		for (int32 i = 0; i < InLandscapeProperties.LandscapeSize + 1; i++)
		{

			int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(0, i, 0));

			int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(InLandscapeProperties.LandscapeSize, i, 0));

			FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

			FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

			InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (InNeighborDirection == ECardinalDirection::Northwest)
	{
		int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(InLandscapeProperties.LandscapeSize, 0, 0));

		int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(0, InLandscapeProperties.LandscapeSize, 0));

		FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

		InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
	else if (InNeighborDirection == ECardinalDirection::Northeast)
	{
		int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(InLandscapeProperties.LandscapeSize, InLandscapeProperties.LandscapeSize, 0));

		int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(0, 0, 0));

		FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

		InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
	else if (InNeighborDirection == ECardinalDirection::Southwest)
	{
		int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(0, 0, 0));

		int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(InLandscapeProperties.LandscapeSize, InLandscapeProperties.LandscapeSize, 0));

		FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

		InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
	else if (InNeighborDirection == ECardinalDirection::Southeast)
	{
		int32 ZippeeIndex = InOutLandscapeData.GetVertexIndex(FVector(0, InLandscapeProperties.LandscapeSize, 0));

		int32 ZipperIndex = ZipperData.GetVertexIndex(FVector(InLandscapeProperties.LandscapeSize, 0, 0));

		FVector WorldZipperLoc = ZipperData.SpawnTransform.TransformPosition(ZipperData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InOutLandscapeData.SpawnTransform.InverseTransformPosition(WorldZipperLoc);

		InOutLandscapeData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
}

void AJetLandscapeMesh::ZipNewLandscape(UObject* WorldContextObject, FProcMeshData& InOutLandscapeData, ECardinalDirection InNeighborDirection, const FLandscapeProperties& InLandscapeProperties)
{
	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}


	switch (InNeighborDirection)
	{
		case ECardinalDirection::North:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::West, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::South, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::East, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			break;
		}
		case ECardinalDirection::South:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::East, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::North, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::West, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			break;
		}
		case ECardinalDirection::East:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::North, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::West, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::South, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);

			break;
		}
		case ECardinalDirection::West:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::South, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::East, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::North, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);

			break;
		}
		case ECardinalDirection::Northeast:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::West, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::Southwest, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::South, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);

		}
		case ECardinalDirection::Southeast:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::North, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::Northwest, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::West, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			//ZipLandscapeDataWithNeighbor(WorldContextObject, ECardinalDirection::South, InOutLandscapeData, InLandscapeProperties);

		}
		case ECardinalDirection::Southwest:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::East, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::Northeast, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::North, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);

		}
		case ECardinalDirection::Northwest:
		{
			ZipLandscapeDataWithNeighbor(ECardinalDirection::South, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::Southeast, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			ZipLandscapeDataWithNeighbor(ECardinalDirection::East, InOutLandscapeData, InLandscapeProperties, GameState->LandscapeDataMap);
			//ZipLandscapeDataWithNeighbor(WorldContextObject, ECardinalDirection::South, InOutLandscapeData, InLandscapeProperties);

		}
	}
}

AJetLandscapeMesh* AJetLandscapeMesh::GetNeighborLandscapeWithCardinalDirection_Implementation(ECardinalDirection InNeighborDirection, const FLandscapeProperties& InLandscapeProperties)
{
	FTransform* TransPtr = LandscapeStatics::NeighborSpawnTransformMap.Find(InNeighborDirection);

	if (!TransPtr)
	{
		return nullptr;
	}

	const FTransform& TransRef = *TransPtr;
	FHitResult HitResult;

	const FVector& NeighborLocation = GetActorLocation() + (TransRef.GetLocation() * InLandscapeProperties.GetVectorScale());

	FVector TraceStart = NeighborLocation - FVector(0,0,200.0f+(HeightVariation*2));

	FVector TraceEnd = NeighborLocation + FVector(0,0,200.0f+(HeightVariation*2));

	FCollisionObjectQueryParams COQP = FCollisionObjectQueryParams();
	COQP.AddObjectTypesToQuery(ECC_GameTraceChannel2);
	FCollisionQueryParams CQP = FCollisionQueryParams();
	CQP.AddIgnoredActor(this);

	bool bGotHit = GetWorld()->LineTraceSingleByObjectType(HitResult, TraceStart, TraceEnd, COQP, CQP);

	if (!bGotHit)
	{
		return nullptr;
	}

	return Cast<AJetLandscapeMesh>(HitResult.GetActor());
}

AJetLandscapeMesh* AJetLandscapeMesh::GetNeighborLandscapeWithRelativeOffset(const FVector& InOffset, const FLandscapeProperties& InLandscapeProperties)
{
	const FVector& NeighborLocation = GetActorLocation() + InOffset;

	FVector TraceStart = NeighborLocation - FVector(0, 0, 200.0f + (InLandscapeProperties.HeightVariation * 2));

	FVector TraceEnd = NeighborLocation + FVector(0, 0, 200.0f + (InLandscapeProperties.HeightVariation * 2));

	FCollisionObjectQueryParams COQP = FCollisionObjectQueryParams();
	COQP.AddObjectTypesToQuery(ECC_GameTraceChannel2);
	FCollisionQueryParams CQP = FCollisionQueryParams();
	CQP.AddIgnoredActor(this);

	FHitResult HitResult;

	bool bGotHit = GetWorld()->LineTraceSingleByObjectType(HitResult, TraceStart, TraceEnd, COQP, CQP);

	if (!bGotHit)
	{
		return nullptr;
	}

	return Cast<AJetLandscapeMesh>(HitResult.GetActor());
}

bool AJetLandscapeMesh::GetNeighborLandscapeData(const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutProcMeshData, int32 InVectorScale, TMap<FVector, FProcMeshData>& InLandscapeDataMap)
{

	if (InNeighborDirection == ECardinalDirection::None)
	{
		return false;
	}

	const FTransform MapTransform = AJetLandscapeMesh::GetNeighborLandscapeSpawnTransform(InNeighborDirection, InVectorScale);

	FProcMeshData* DataPtr = InLandscapeDataMap.Find((InLandscapeData.SpawnTransform.GetLocation() + MapTransform.GetLocation()) * FVector(1, 1, 0));

	if (DataPtr)
	{
		OutProcMeshData = *DataPtr;
		return true;
	}

	return false;
}

bool AJetLandscapeMesh::FindLandscapeData(const TMap<FVector, FProcMeshData>& InLandscapeDataMap, const FVector& InVectorKey, FProcMeshData& OutProcMeshData, const FLandscapeProperties& InLandscapeProperties)
{
	const FProcMeshData* DataPtr = InLandscapeDataMap.Find(InVectorKey);

	if (!DataPtr)
	{
		return false;
	}

	OutProcMeshData = *DataPtr;

	return true;
}

bool AJetLandscapeMesh::Static_GetNeighborLandscapeData(UObject* WorldContextObject, const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutNeighborData, int32 InVectorScale)
{
	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState)
	{
		return false;
	}

	return GameState->GameState_GetNeighborLandscapeData(InLandscapeData, InNeighborDirection, OutNeighborData, InVectorScale);

}

void AJetLandscapeMesh::SpawnNeighborLandscapesInRadius(UObject* WorldContextObject, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, AJetWorldSpawner* InWorldSpawner, TMap<FVector, FProcMeshData>& InLandscapeDataMap)
{
	/*if (bHasSpawnedNeighborLandscapes)
	{
		return;
	}*/

	int32 i = 0;

	TMap<FVector, FProcMeshData> OutProcMeshData;

	TArray<FProcMeshData> LocalLandscapeSpawnQueue;

	int32 x = 0;
	int32 y = 0;

	int32 xDim = (InLandscapeProperties.NeighborSpawnRadius * 2) + 1;
	int32 yDim = xDim;

	int32 Modifier = xDim/2;

	TArray<FTransform> LocalSpawnTransformQueue;

	for (y = 0; y < yDim; y++)
	{
		for (x = 0; x < xDim; x++)
		{

			FProcMeshData Landscape;

			FVector MapKey = FVector(InLocation + (InLandscapeProperties.GetVectorScale() * (FVector(x - Modifier, y - Modifier, 0))));

			MapKey = MapKey * FVector(1, 1, 0);


			bool bFoundLandscape = FindLandscapeData(InLandscapeDataMap, MapKey, Landscape, InLandscapeProperties);

			if (bFoundLandscape)
			{
				if (!Landscape.bIsActive)
				{
					LocalLandscapeSpawnQueue.Add(Landscape);
				}

				continue;
			}

			FTransform NewTileSpawnTransform = FTransform(MapKey);

			Landscape = CreateLandscapeData(NewTileSpawnTransform, InLandscapeProperties, InLandscapeDataMap);

			if (x > 0)
			{
				ZipLandscapeDataWithNeighbor(ECardinalDirection::South, Landscape, InLandscapeProperties, InLandscapeDataMap);
			}
			else
			{
				ZipLandscapeDataWithNeighbor(ECardinalDirection::North, Landscape, InLandscapeProperties, InLandscapeDataMap);
			}

			if (y > 0)
			{
				ZipLandscapeDataWithNeighbor(ECardinalDirection::West, Landscape, InLandscapeProperties, InLandscapeDataMap);
			}
			else
			{
				ZipLandscapeDataWithNeighbor(ECardinalDirection::East, Landscape, InLandscapeProperties, InLandscapeDataMap);
			}

			LocalLandscapeSpawnQueue.Add(Landscape);
		}
	}

	// Spawn all of the new neighbors
	for (FProcMeshData& Landscape : LocalLandscapeSpawnQueue)
	{
		Landscape.bIsActive = true;
		OnLandscapeDataCreated(WorldContextObject, Landscape);
		SpawnLandscapeWithData(WorldContextObject, Landscape, InLandscapeProperties, InWorldSpawner);
	}

	//bHasSpawnedNeighborLandscapes = true;
}

void AJetLandscapeMesh::CreateLandscapesInRadius(const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TArray<FProcMeshData>& InOutLandscapeDataArray, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap)
{
	int32 x = 0;
	int32 y = 0;

	int32 xDim = (InLandscapeProperties.NeighborSpawnRadius * 2) + 1;
	int32 yDim = xDim;

	int32 Modifier = xDim / 2;

	bool bxEven = true;
	bool byEven = true;

	int32 xEven = 0;
	int32 yEven = 0;

	for (yEven = 0; yEven < 2; yEven++)
	{
		for (y = 0; y < yDim; y++)
		{
			if ((y % 2 == 0) != (yEven == 0))
			{
				continue;
			}

			for (xEven = 0; xEven < 2; xEven++)
			{
				for (x = 0; x < xDim; x++)
				{
					if ((x % 2 == 0) != (xEven == 0))
					{
						continue;
					}
					FProcMeshData Landscape;

					FVector MapKey = FVector(InLocation + (InLandscapeProperties.GetVectorScale() * (FVector(x - Modifier, y - Modifier, 0))));

					MapKey = MapKey * FVector(1, 1, 0);


					bool bFoundLandscape = FindLandscapeData(InOutLandscapeDataMap, MapKey, Landscape, InLandscapeProperties);

					if (bFoundLandscape)
					{
						if (!Landscape.bIsActive)
						{
							InOutLandscapeDataArray.Add(Landscape);
						}

						continue;
					}

					FTransform NewTileSpawnTransform = FTransform(MapKey);

					Landscape = CreateLandscapeData(NewTileSpawnTransform, InLandscapeProperties, InOutLandscapeDataMap);

					FProcMeshData EasternNeighbor;
					bool bEasternNeighbor = GetNeighborLandscapeData(Landscape, ECardinalDirection::East, EasternNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

					FProcMeshData NorthernNeighbor;
					bool bNorthernNeighbor = GetNeighborLandscapeData(Landscape, ECardinalDirection::North, NorthernNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

					FProcMeshData SouthernNeighbor;
					bool bSouthernNeighbor = GetNeighborLandscapeData(Landscape, ECardinalDirection::South, SouthernNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

					FProcMeshData WesternNeighbor;
					bool bWesternNeighbor = GetNeighborLandscapeData(Landscape, ECardinalDirection::West, WesternNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);


					if (bSouthernNeighbor)
					{
						ZipLandscapeDataWithNeighbor(ECardinalDirection::South, Landscape, InLandscapeProperties, InOutLandscapeDataMap);
					}
					
					if (bNorthernNeighbor)
					{
						ZipLandscapeDataWithNeighbor(ECardinalDirection::North, Landscape, InLandscapeProperties, InOutLandscapeDataMap);
					}

					if (bWesternNeighbor)
					{
						ZipLandscapeDataWithNeighbor(ECardinalDirection::West, Landscape, InLandscapeProperties, InOutLandscapeDataMap);
					}
					
					if (bEasternNeighbor)
					{
						ZipLandscapeDataWithNeighbor(ECardinalDirection::East, Landscape, InLandscapeProperties, InOutLandscapeDataMap);
					}

					InOutLandscapeDataArray.Add(Landscape);
					InOutLandscapeDataMap.Add(Landscape.SpawnTransform.GetLocation() * FVector(1, 1, 0), Landscape);
				}
			}
		}
	}
}

void AJetLandscapeMesh::QueueSpawnNeighborLandscapesInRadius()
{
	if (bHasSpawnedNeighborLandscapes)
	{
		return;
	}

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;

	int32 xDim = (NeighborSpawnRadius * 2) + 1;
	int32 yDim = xDim;

	int32 Modifier = xDim / 2;

	TArray<FTransform> LocalSpawnTransformQueue;

	for (y = 0; y < yDim; y++)
	{
		for (x = 0; x < xDim; x++)
		{
			FProcMeshData Landscape;

			FVector MapKey = FVector(GetActorLocation() + (LandscapeProperties.GetVectorScale() * (FVector(x - Modifier, y - Modifier, 0))));

			MapKey = MapKey * FVector(1, 1, 0);

			FTransform NewTileSpawnTransform = FTransform(MapKey);

			LocalSpawnTransformQueue.Add(NewTileSpawnTransform);
		}
	}

	AppendLandscapeSpawnQueue_Transform(LocalSpawnTransformQueue);

	//// Spawn all of the new neighbors
	//for (FProcMeshData& Landscape : LocalLandscapeSpawnQueue)
	//{
	//	SpawnLandscapeWithData(this, Landscape, LandscapeProperties);
	//}

	bHasSpawnedNeighborLandscapes = true;
}

TArray<AJetLandscapeMesh*> AJetLandscapeMesh::GetNeighborLandscapesInRadius()
{
	return TArray<AJetLandscapeMesh*>();
}

void AJetLandscapeMesh::SpawnNeighborLandscapes()
{
	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	for (dir = 0; dir < MaxDirection + 1; dir++)
	{
		SpawnNeighborLandscape((ECardinalDirection) dir);
	}
}

TArray<AJetLandscapeMesh*> AJetLandscapeMesh::GetAllNeighborLandscapes()
{
	TArray<AJetLandscapeMesh*> OutLandscapes;

	int32 i = 0;

	TArray<AJetLandscapeMesh*> CurrentNeighborLandscapes;

	TArray<AJetLandscapeMesh*> NewNeighborLandscapes;

	CurrentNeighborLandscapes.Add(this);

	for (i = 0; i < NeighborSpawnRadius; i++)
	{
		for (AJetLandscapeMesh* CurrentNeighbor : CurrentNeighborLandscapes)
		{
			if (!CurrentNeighbor)
			{
				continue;
			}

			int32 MaxDirection = (int32)ECardinalDirection::Northwest;
			int32 dir = 0;

			for (dir = 0; dir < MaxDirection + 1; dir++)
			{
				AJetLandscapeMesh* NeighborLandscape = CurrentNeighbor->GetNeighborLandscapeWithCardinalDirection((ECardinalDirection)dir, LandscapeProperties);

				if (!NeighborLandscape)
				{
					continue;
				}

				NewNeighborLandscapes.Add(NeighborLandscape);

				if (OutLandscapes.Contains(NeighborLandscape))
				{
					continue;
				}

				OutLandscapes.Add(NeighborLandscape);
			}
		}
		CurrentNeighborLandscapes.Empty();
		CurrentNeighborLandscapes = NewNeighborLandscapes;
		NewNeighborLandscapes.Empty();
	}

	return OutLandscapes;
}

AJetLandscapeMesh* AJetLandscapeMesh::SpawnNeighborLandscape(ECardinalDirection InNeighborDirection)
{
	//if the neighbor landscape already exists, we don't need to spawn it, return
	AJetLandscapeMesh* NeighborLandscape = GetNeighborLandscapeWithCardinalDirection(InNeighborDirection, LandscapeProperties);

	if (NeighborLandscape)
	{
		/*if (bReturnExisting)
		{
			return NeighborLandscape;
		}*/

		return nullptr;
	}

	if (InNeighborDirection == ECardinalDirection::None)
	{
		return nullptr;
	}

	FVector CurrentLocation = GetActorLocation();
	int32 VectorScale = TileSize * LandscapeSize;
	FTransform NeighborTransform = GetNeighborLandscapeSpawnTransform(InNeighborDirection, VectorScale);
	NeighborTransform.SetLocation(NeighborTransform.GetLocation() + CurrentLocation);

	int32 HeightMod = FMath::RandRange(-HeightVariation, HeightVariation);

	NeighborTransform.SetLocation(NeighborTransform.GetLocation() + FVector(0, 0, HeightMod));

	//Search for the previously spawned landscape, spawn with that data if found
	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return nullptr;
	}

	FProcMeshData* DataPtr = GameState->LandscapeDataMap.Find(NeighborTransform.GetLocation() * FVector(1, 1, 0));

	if (DataPtr)
	{
		return SpawnLandscapeWithData(this, *DataPtr, LandscapeProperties, WorldSpawner);
	}
	

	//Create a new landscape and spawn it
	FProcMeshData NewLandscapeData = CreateLandscapeData(NeighborTransform, LandscapeProperties, GameState->LandscapeDataMap);

	return SpawnNeighboringLandscapeWithData(NewLandscapeData);
	//ZipLandscapeDataWithNeighbors(NewLandscapeData);
}

bool AJetLandscapeMesh::CreateNewNeighborLandscapeData(UObject* WorldContextObject, const FProcMeshData& InLandscape, ECardinalDirection InNeighborDirection, FProcMeshData& OutProcMeshData, const FLandscapeProperties& InLandscapeProperties)
{
	//Search for the previously spawned landscape, spawn with that data if found
	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState)
	{
		return false;
	}

	//if the neighbor landscape already exists, we don't need to spawn it, return
	bool bFoundNeighbor = GetNeighborLandscapeData(InLandscape, InNeighborDirection, OutProcMeshData, InLandscapeProperties.GetVectorScale(), GameState->LandscapeDataMap);

	if (bFoundNeighbor)
	{
		if (OutProcMeshData.bIsActive)
		{
			return false;
		}
	}

	if (InNeighborDirection == ECardinalDirection::None)
	{
		return false;
	}

	FVector CurrentLocation = InLandscape.SpawnTransform.GetLocation();

	FTransform NeighborTransform = GetNeighborLandscapeSpawnTransform(InNeighborDirection, InLandscapeProperties.GetVectorScale());
	NeighborTransform.SetLocation(NeighborTransform.GetLocation() + CurrentLocation);

	int32 HeightMod = FMath::RandRange(-InLandscapeProperties.HeightVariation, InLandscapeProperties.HeightVariation);

	NeighborTransform.SetLocation(NeighborTransform.GetLocation() + FVector(0, 0, HeightMod));

	////Search for the previously spawned landscape, spawn with that data if found
	//AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	//if (GameState)
	//{
	//	FProcMeshData* DataPtr = GameState->LandscapeDataMap.Find(NeighborTransform.GetLocation() * FVector(1, 1, 0));

	//	if (DataPtr)
	//	{
	//		return SpawnLandscapeWithData(this, *DataPtr, LandscapeSize, TileSize, HeightVariation, NeighborSpawnRadius);
	//	}
	//}

	//Create a new landscape and spawn it
	FProcMeshData NewLandscapeData = CreateLandscapeData(NeighborTransform, InLandscapeProperties, GameState->LandscapeDataMap);
	
	//ZipLandscapeDataWithNeighbors(WorldContextObject, NewLandscapeData, InLandscapeProperties);

	OutProcMeshData = NewLandscapeData;

	return true;
	//ZipLandscapeDataWithNeighbors(NewLandscapeData);
}


void AJetLandscapeMesh::OnLandscapeDataCreated(UObject* WorldContextObject, const FProcMeshData& InLandscape)
{
	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	GameState->LandscapeDataMap.Add(InLandscape.GetMapKey(), InLandscape);
}

AJetLandscapeMesh* AJetLandscapeMesh::SpawnNeighboringLandscapeWithData(const FProcMeshData& InNeighborData)
{
	FActorSpawnParameters ActorSpawnParams = FActorSpawnParameters();
	UClass* LandscapeClass = GetClass();

	//Spawn the landscape deferred
	AJetLandscapeMesh* SpawnedActor = GetWorld()->SpawnActorDeferred<AJetLandscapeMesh>(LandscapeClass, InNeighborData.SpawnTransform);

	if (!SpawnedActor)
	{
		return nullptr;
	}

	SpawnedActor->bAutoCreateLandscape = false;
	SpawnedActor->LandscapeSize = LandscapeSize;
	SpawnedActor->TileSize = TileSize;
	SpawnedActor->HeightVariation = HeightVariation;
	SpawnedActor->NeighborSpawnRadius = NeighborSpawnRadius;

	SpawnedActor->ProcMeshData = InNeighborData;
	
	UGameplayStatics::FinishSpawningActor(SpawnedActor, InNeighborData.SpawnTransform);

	//SpawnedActor->CreateLandscape(SpawnedActor->LandscapeSize);

	//ZipNeighborLandscape(this, SpawnedActor);

	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	for (dir = 0; dir < MaxDirection + 1; dir++)
	{
		ECardinalDirection CardDir = (ECardinalDirection)dir;
		AJetLandscapeMesh* CurrentNeighbor = SpawnedActor->GetNeighborLandscapeWithCardinalDirection(CardDir, LandscapeProperties);

		if (CurrentNeighbor)
		{
			ZipNeighborLandscape(CurrentNeighbor, SpawnedActor);
		}
	}

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (GameState)
	{
		GameState->OnLandscapeSpawned(SpawnedActor, SpawnedActor->ProcMeshData);
	}

	SpawnedActor->CreateMesh();

	

	return SpawnedActor;
}

AJetLandscapeMesh* AJetLandscapeMesh::SpawnLandscapeWithData(UObject* WorldContextObject, const FProcMeshData& InProcMeshData, const FLandscapeProperties& InLandscapeProperties, AJetWorldSpawner* InWorldSpawner)
{
	FActorSpawnParameters ActorSpawnParams = FActorSpawnParameters();
	UClass* LandscapeClass = InLandscapeProperties.LandscapeClass;

	//Spawn the landscape deferred
	AJetLandscapeMesh* SpawnedActor = WorldContextObject->GetWorld()->SpawnActorDeferred<AJetLandscapeMesh>(LandscapeClass, InProcMeshData.SpawnTransform);

	if (!SpawnedActor)
	{
		return nullptr;
	}

	SpawnedActor->bAutoCreateLandscape = false;
	SpawnedActor->LandscapeSize = InLandscapeProperties.LandscapeSize;
	SpawnedActor->TileSize = InLandscapeProperties.TileSize;
	SpawnedActor->HeightVariation = InLandscapeProperties.HeightVariation;
	SpawnedActor->NeighborSpawnRadius = InLandscapeProperties.NeighborSpawnRadius;

	SpawnedActor->LandscapeProperties = InLandscapeProperties;
	SpawnedActor->WorldSpawner = InWorldSpawner;


	SpawnedActor->ProcMeshData = InProcMeshData;

	UGameplayStatics::FinishSpawningActor(SpawnedActor, InProcMeshData.SpawnTransform);

	SpawnedActor->CreateMesh();

	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (GameState)
	{
		GameState->OnLandscapeSpawned(SpawnedActor, SpawnedActor->ProcMeshData);
	}

	return SpawnedActor;
}

void AJetLandscapeMesh::ZipNeighborLandscape(AJetLandscapeMesh* InZipper, AJetLandscapeMesh* InZippee)
{
	//"zip" the landscape up with its neighbors

	if (!InZipper || !InZippee)
	{
		return;
	}

	FVector2D ZippeeVector;
	FVector2D ZipperVector;

	ECardinalDirection NeighborDir = GetNeighborCardinality(InZipper, InZippee);

	bool bCorner = false;

	if (NeighborDir == ECardinalDirection::West)
	{
		for (int32 i = 0; i < InZipper->LandscapeSize + 1; i++)
		{

			int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(i, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

			int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(i, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

			if (ZippeeIndex < 0)
			{
				continue;
			}

			if (ZipperIndex < 0)
			{
				continue;
			}

			FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);
			
			FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

			InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (NeighborDir == ECardinalDirection::East)
	{
		for (int32 i = 0; i < InZipper->LandscapeSize + 1; i++)
		{

			int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(i, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

			int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(i, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

			FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

			FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

			InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (NeighborDir == ECardinalDirection::North)
	{
		for (int32 i = 0; i < InZipper->LandscapeSize + 1; i++)
		{

			int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(0, i, 0), FVector(LandscapeSize, LandscapeSize, 0));

			int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(LandscapeSize, i, 0), FVector(LandscapeSize, LandscapeSize, 0));

			FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

			FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

			InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (NeighborDir == ECardinalDirection::South)
	{
		for (int32 i = 0; i < InZipper->LandscapeSize + 1; i++)
		{

			int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(LandscapeSize, i, 0), FVector(LandscapeSize, LandscapeSize, 0));

			int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(0, i, 0), FVector(LandscapeSize, LandscapeSize, 0));

			FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

			FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

			InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
		}
	}
	else if (NeighborDir == ECardinalDirection::Northwest)
	{
		int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(0, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

		int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(LandscapeSize, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

		FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

		InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
	else if (NeighborDir == ECardinalDirection::Northeast)
	{
		int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(0, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

		int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(LandscapeSize, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

		FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

		InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
	else if (NeighborDir == ECardinalDirection::Southwest)
	{
		int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(LandscapeSize, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

		int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(0, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

		FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

		InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
	else if (NeighborDir == ECardinalDirection::Southeast)
	{
		int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(LandscapeSize, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

		int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(0, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

		FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

		InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
}

FTransform AJetLandscapeMesh::GetNeighborLandscapeSpawnTransform(ECardinalDirection InNeighborDirection, int32 InVectorScale)
{
	FTransform* TransformPtr = LandscapeStatics::NeighborSpawnTransformMap.Find(InNeighborDirection);

	if (TransformPtr)
	{
		FTransform OutTransform = *TransformPtr;

		OutTransform.SetLocation(OutTransform.GetLocation() * InVectorScale);
		return OutTransform;
	}
	
	return FTransform();
}

ECardinalDirection AJetLandscapeMesh::GetNeighborCardinality(AJetLandscapeMesh* InLandscapeOne, AJetLandscapeMesh* InLandscapeTwo)
{
	FVector PosOne = InLandscapeOne->GetActorLocation();
	FVector PosTwo = InLandscapeTwo->GetActorLocation();

	FVector Dir = PosTwo - PosOne;

	Dir.Z = 0;

	FVector Normal = Dir.GetSafeNormal();

	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	for (dir = 0; dir < MaxDirection + 1; dir++)
	{
		ECardinalDirection CardDir = (ECardinalDirection)dir;

		FTransform* TransPtr = LandscapeStatics::NeighborSpawnTransformMap.Find(CardDir);

		if (TransPtr)
		{
			FVector CardDirVector = TransPtr->GetLocation().GetSafeNormal();

			if (CardDirVector.Equals(Normal, 0.1f))
			{
				return CardDir;
			}
		}
	}

	return ECardinalDirection::None;
}

ECardinalDirection AJetLandscapeMesh::GetNeighborDataCardinality(const FProcMeshData& InLandscapeDataOne, const FProcMeshData& InLandscapeDataTwo)
{
	FVector PosOne = InLandscapeDataOne.SpawnTransform.GetLocation();
	FVector PosTwo = InLandscapeDataTwo.SpawnTransform.GetLocation();

	FVector Dir = PosTwo - PosOne;

	Dir.Z = 0;

	FVector Normal = Dir.GetSafeNormal();

	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	TMap<ECardinalDirection, FTransform> LocalNeighborSpawnTransformMap =
	{ {ECardinalDirection::North, FTransform(FVector(1,0,0))} };

	for (dir = 0; dir < MaxDirection + 1; dir++)
	{
		ECardinalDirection CardDir = (ECardinalDirection)dir;

		FTransform* TransPtr = LocalNeighborSpawnTransformMap.Find(CardDir);

		if (TransPtr)
		{
			FVector CardDirVector = TransPtr->GetLocation().GetSafeNormal();

			if (CardDirVector.Equals(Normal, 0.1f))
			{
				return CardDir;
			}
		}
	}

	return ECardinalDirection::None;
}

TArray<FVector> AJetLandscapeMesh::CreateLandscapeVertexArray(const FLandscapeProperties& InLandscapeProperties, FProcMeshData& InOutProcMeshData, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap)
{
	if (!InOutProcMeshData.FaceVertexMapArray.IsValidIndex(0))
	{
		return TArray<FVector>();
	}

	int32 VertexNum = (InLandscapeProperties.LandscapeSize + 1) * (InLandscapeProperties.LandscapeSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector> OutVertexArray;

	int32 XDim = InLandscapeProperties.LandscapeSize + 1;
	int32 YDim = InLandscapeProperties.LandscapeSize + 1;

	int32 i = 0;

	FProcMeshData EasternNeighbor;
	bool bEasternNeighbor = GetNeighborLandscapeData(InOutProcMeshData, ECardinalDirection::East, EasternNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

	FProcMeshData NorthernNeighbor;
	bool bNorthernNeighbor = GetNeighborLandscapeData(InOutProcMeshData, ECardinalDirection::North, NorthernNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);


	FProcMeshData SouthernNeighbor;
	bool bSouthernNeighbor = GetNeighborLandscapeData(InOutProcMeshData, ECardinalDirection::South, SouthernNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

	FProcMeshData WesternNeighbor;
	bool bWesternNeighbor = GetNeighborLandscapeData(InOutProcMeshData, ECardinalDirection::West, WesternNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

	bool bBackward = false;
	if (bNorthernNeighbor || bEasternNeighbor)
	{
		bBackward = true;
	}

	if (!bBackward)
	{
		for (y = 0; y < YDim; y++)
		{
			for (x = 0; x < XDim; x++)
			{
				int32 xPre = InOutProcMeshData.GetVertexIndex(FVector(x - 1, y, 0), 0);

				int32 yPre = InOutProcMeshData.GetVertexIndex(FVector(x, y - 1, 0), 0);

				int32 xPost = InOutProcMeshData.GetVertexIndex(FVector(x + 1, y, 0), 0);

				int32 yPost = InOutProcMeshData.GetVertexIndex(FVector(x, y + 1, 0), 0);

				bool bHavexPre = xPre > -1;
				bool bHaveyPre = yPre > -1;
				bool bHavexPost = xPost > -1;
				bool bHaveyPost = yPost > -1;


				int32 xPreHeight = 0;

				int32 yPreHeight = 0;


				if (!bHavexPre)
				{
					if (bSouthernNeighbor)
					{
						int32* HeightPtr = SouthernNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(XDim - 1, y, 0));

						if (HeightPtr)
						{
							bHavexPre = true;
							xPreHeight = SouthernNeighbor.Vertices[*HeightPtr].Z;
						}
					}
				}
				else
				{
					xPreHeight = OutVertexArray[xPre].Z;
				}


				if (!bHaveyPre)
				{
					if (bWesternNeighbor)
					{
						int32* HeightPtr = WesternNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(x, YDim - 1, 0));

						if (HeightPtr)
						{
							bHaveyPre = true;
							yPreHeight = WesternNeighbor.Vertices[*HeightPtr].Z;
						}
					}
				}
				else
				{
					yPreHeight = OutVertexArray[yPre].Z;
				}

				if (!bHavexPost)
				{
					if (bEasternNeighbor)
					{
						int32* HeightPtr = EasternNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(x, 0, 0));

						if (HeightPtr)
						{
							bHavexPost = true;
							xPreHeight = EasternNeighbor.Vertices[*HeightPtr].Z;
						}
					}
				}
				else
				{
					if (xPreHeight == 0)
					{
						xPreHeight = OutVertexArray[yPost].Z;
					}
				}

				if (!bHaveyPost)
				{
					if (bNorthernNeighbor)
					{
						int32* HeightPtr = NorthernNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(0, y, 0));

						if (HeightPtr)
						{
							bHaveyPost = true;
							yPreHeight = NorthernNeighbor.Vertices[*HeightPtr].Z;
						}
					}
				}
				else
				{
					if (yPreHeight == 0)
					{
						yPreHeight = OutVertexArray[yPost].Z;
					}
				}


				int32 Divisor = ((bHavexPre ? 1 : 0) + (bHaveyPre ? 1 : 0));

				int32 AvgPreHeight = (xPreHeight + yPreHeight) / (Divisor > 0 ? Divisor : 1);

				int32 RandHeightDiff = FMath::RandRange(AvgPreHeight - InLandscapeProperties.MaximumHeightDifference, AvgPreHeight + InLandscapeProperties.MaximumHeightDifference);

				//HeightMod = HeightMod + AvgPreHeight;

				int32 HeightMod = RandHeightDiff;

				/*}*/

				OutVertexArray.Add(FVector(x * InLandscapeProperties.TileSize, y * InLandscapeProperties.TileSize, HeightMod));

				InOutProcMeshData.FaceVertexMapArray[0].VertexIndexMap.Add(FVector(x, y, 0), i);
				//VertexIndexMap.Add(FVector2D(x, y), i);

				i++;
			}
		}
	}
	else
	{
		if (bNorthernNeighbor)
		{
			int32 iMinus = (XDim * YDim);
			OutVertexArray.SetNumZeroed(iMinus);
			iMinus--;
			//backward
			for (y = 0; y < YDim; y++)
			{
				for (x = XDim - 1; x > -1; x--)
				{
					int32 xPre = InOutProcMeshData.GetVertexIndex(FVector(x - 1, y, 0), 0);

					int32 yPre = InOutProcMeshData.GetVertexIndex(FVector(x, y - 1, 0), 0);

					int32 xPost = InOutProcMeshData.GetVertexIndex(FVector(x + 1, y, 0), 0);

					int32 yPost = InOutProcMeshData.GetVertexIndex(FVector(x, y + 1, 0), 0);

					bool bHavexPre = xPre > -1;
					bool bHaveyPre = yPre > -1;
					bool bHavexPost = xPost > -1;
					bool bHaveyPost = yPost > -1;

					int32 xPreHeight = 0;

					int32 yPreHeight = 0;


					if (!bHaveyPre)
					{
						if (bWesternNeighbor)
						{
							int32* HeightPtr = WesternNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(x, YDim - 1, 0));

							if (HeightPtr)
							{
								bHaveyPre = true;
								yPreHeight = WesternNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						yPreHeight = OutVertexArray[yPre].Z;
					}

					if (!bHavexPost)
					{
						if (bEasternNeighbor)
						{
							int32* HeightPtr = EasternNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(x, 0, 0));

							if (HeightPtr)
							{
								bHavexPost = true;
								xPreHeight = EasternNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						xPreHeight = OutVertexArray[xPost].Z;
					}

					if (!bHaveyPost)
					{
						if (bNorthernNeighbor)
						{
							int32* HeightPtr = NorthernNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(0, y, 0));

							if (HeightPtr)
							{
								bHaveyPost = true;
								yPreHeight = NorthernNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						if (yPreHeight == 0)
						{
							yPreHeight = OutVertexArray[yPost].Z;
						}
					}

					if (!bHavexPre)
					{
						if (bSouthernNeighbor)
						{
							int32* HeightPtr = SouthernNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(XDim - 1, y, 0));

							if (HeightPtr)
							{
								bHavexPre = true;
								xPreHeight = SouthernNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						if (xPreHeight == 0)
						{
							xPreHeight = OutVertexArray[xPre].Z;
						}
					}

					int32 Divisor = ((bHavexPost ? 1 : 0) + (bHaveyPre ? 1 : 0));

					int32 AvgPreHeight = (xPreHeight + yPreHeight) / (Divisor > 0 ? Divisor : 1);

					int32 RandHeightDiff = FMath::RandRange(AvgPreHeight - InLandscapeProperties.MaximumHeightDifference, AvgPreHeight + InLandscapeProperties.MaximumHeightDifference);

					//HeightMod = HeightMod + AvgPreHeight;

					int32 HeightMod = RandHeightDiff;

					/*}*/
					iMinus = (y * YDim) + x;
					//OutVertexArray.Add(FVector(x * InLandscapeProperties.TileSize, y * InLandscapeProperties.TileSize, HeightMod));
					FVector NewLoc = FVector(x * InLandscapeProperties.TileSize, y * InLandscapeProperties.TileSize, HeightMod);
					
					OutVertexArray[iMinus] = NewLoc;

					InOutProcMeshData.FaceVertexMapArray[0].VertexIndexMap.Add(FVector(x, y, 0), iMinus);
					//VertexIndexMap.Add(FVector2D(x, y), i);

					//iMinus--;
				}
			}
		}
		else if (bEasternNeighbor)
		{
			int32 iMinus = (XDim * YDim);
			OutVertexArray.SetNumZeroed(iMinus);
			iMinus--;
			//backward
			for (y = YDim-1; y > -1; y--)
			{
				for (x = 0; x < XDim; x++)
				{
					int32 xPre = InOutProcMeshData.GetVertexIndex(FVector(x - 1, y, 0), 0);

					int32 yPre = InOutProcMeshData.GetVertexIndex(FVector(x, y - 1, 0), 0);

					int32 xPost = InOutProcMeshData.GetVertexIndex(FVector(x + 1, y, 0), 0);

					int32 yPost = InOutProcMeshData.GetVertexIndex(FVector(x, y + 1, 0), 0);

					bool bHavexPre = xPre > -1;
					bool bHaveyPre = yPre > -1;
					bool bHavexPost = xPost > -1;
					bool bHaveyPost = yPost > -1;

					int32 xPreHeight = 0;

					int32 yPreHeight = 0;

					if (!bHavexPre)
					{
						if (bSouthernNeighbor)
						{
							int32* HeightPtr = SouthernNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(XDim - 1, y, 0));

							if (HeightPtr)
							{
								bHavexPre = true;
								xPreHeight = SouthernNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						xPreHeight = OutVertexArray[xPre].Z;
					}

					if (!bHaveyPost)
					{
						if (bNorthernNeighbor)
						{
							int32* HeightPtr = NorthernNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(0, y, 0));

							if (HeightPtr)
							{
								bHaveyPost = true;
								yPreHeight = NorthernNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						yPreHeight = OutVertexArray[yPost].Z;
					}


					if (!bHaveyPre)
					{
						if (bWesternNeighbor)
						{
							int32* HeightPtr = WesternNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(x, YDim - 1, 0));

							if (HeightPtr)
							{
								bHaveyPre = true;
								yPreHeight = WesternNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						if (yPreHeight == 0)
						{
							yPreHeight = OutVertexArray[yPre].Z;
						}
					}

					if (!bHavexPost)
					{
						if (bEasternNeighbor)
						{
							int32* HeightPtr = EasternNeighbor.FaceVertexMapArray[0].VertexIndexMap.Find(FVector(x, 0, 0));

							if (HeightPtr)
							{
								bHavexPost = true;
								xPreHeight = EasternNeighbor.Vertices[*HeightPtr].Z;
							}
						}
					}
					else
					{
						if (xPreHeight == 0)
						{
							xPreHeight = OutVertexArray[yPost].Z;
						}
					}

					int32 Divisor = ((bHavexPre ? 1 : 0) + (bHaveyPost ? 1 : 0));

					int32 AvgPreHeight = (xPreHeight + yPreHeight) / (Divisor > 0 ? Divisor : 1);

					int32 RandHeightDiff = FMath::RandRange(AvgPreHeight - InLandscapeProperties.MaximumHeightDifference, AvgPreHeight + InLandscapeProperties.MaximumHeightDifference);

					//HeightMod = HeightMod + AvgPreHeight;

					int32 HeightMod = RandHeightDiff;

					/*}*/
					iMinus = (y * YDim) + x;
					//OutVertexArray.Add(FVector(x * InLandscapeProperties.TileSize, y * InLandscapeProperties.TileSize, HeightMod));
					FVector NewLoc = FVector(x * InLandscapeProperties.TileSize, y * InLandscapeProperties.TileSize, HeightMod);

					OutVertexArray[iMinus] = NewLoc;

					InOutProcMeshData.FaceVertexMapArray[0].VertexIndexMap.Add(FVector(x, y, 0), iMinus);
					//VertexIndexMap.Add(FVector2D(x, y), i);

					//iMinus--;
				}
			}
		}
		
	}

	return OutVertexArray;
}

TArray<FVector> AJetLandscapeMesh::CreateLandscapeVertexArrayNew(const FLandscapeProperties& InLandscapeProperties, FProcMeshData& InOutProcMeshData, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap)
{
	if (!InOutProcMeshData.FaceVertexMapArray.IsValidIndex(0))
	{
		return TArray<FVector>();
	}

	int32 VertexNum = (InLandscapeProperties.LandscapeSize + 1) * (InLandscapeProperties.LandscapeSize + 1);

	int32 x = 0;
	int32 y = 0;

	int32 XDim = InLandscapeProperties.LandscapeSize + 1;
	int32 YDim = InLandscapeProperties.LandscapeSize + 1;

	/*if (!InOutProcMeshData.FaceVertexMapArray.IsValidIndex(0))
	{
		return TArray<FVector>();
	}*/

	TArray<FVector> OutVertexArray;
	TArray<FVector> TempVertices;

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			TempVertices.Add(FVector(x, y, 0));
		}
	}

	const int32 TotalNumVertices = TempVertices.Num();

	OutVertexArray.SetNumZeroed(TotalNumVertices);
	InOutProcMeshData.Vertices.SetNumZeroed(TotalNumVertices);
	int32 CurrentNumVertices = TempVertices.Num();


	for (int32 i = 0; i < TotalNumVertices; i++)
	{
		const int32 CurrentRandomVertexIndex = UKismetMathLibrary::RandomIntegerInRange(0, CurrentNumVertices-1);

		const FVector& CurrentMapKey = TempVertices[CurrentRandomVertexIndex];
		//do stuff

		const int32 AvgNeighborHeight = FindAverageVertexNeighborHeight(CurrentMapKey, InLandscapeProperties, InOutProcMeshData, InOutLandscapeDataMap);

		const int32 NewHeight = UKismetMathLibrary::RandomIntegerInRange(AvgNeighborHeight - InLandscapeProperties.MaximumHeightDifference, AvgNeighborHeight + InLandscapeProperties.MaximumHeightDifference);

		const FVector NewVertex = (CurrentMapKey * InLandscapeProperties.TileSize) + FVector(0, 0, NewHeight);


		const int32 NewVertexIndex = CurrentMapKey.X + (CurrentMapKey.Y * (YDim));

		OutVertexArray[NewVertexIndex] = NewVertex;

		InOutProcMeshData.Vertices[NewVertexIndex] = NewVertex;

		InOutProcMeshData.FaceVertexMapArray[0].VertexIndexMap.Add(CurrentMapKey, NewVertexIndex);

		TempVertices.RemoveAt(CurrentRandomVertexIndex);
		CurrentNumVertices--;
	}


	return OutVertexArray;
}

int32 AJetLandscapeMesh::FindAverageVertexNeighborHeight(const FVector& InVertex, const FLandscapeProperties& InLandscapeProperties, const FProcMeshData& InProcMeshData, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap)
{
	const int32 Radius = InLandscapeProperties.LandscapeSize + 1;

	FProcMeshData EasternNeighbor;
	bool bEasternNeighbor = GetNeighborLandscapeData(InProcMeshData, ECardinalDirection::East, EasternNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

	FProcMeshData NorthernNeighbor;
	bool bNorthernNeighbor = GetNeighborLandscapeData(InProcMeshData, ECardinalDirection::North, NorthernNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

	FProcMeshData SouthernNeighbor;
	bool bSouthernNeighbor = GetNeighborLandscapeData(InProcMeshData, ECardinalDirection::South, SouthernNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

	FProcMeshData WesternNeighbor;
	bool bWesternNeighbor = GetNeighborLandscapeData(InProcMeshData, ECardinalDirection::West, WesternNeighbor, InLandscapeProperties.GetVectorScale(), InOutLandscapeDataMap);

	FVector NorthNeighborVector;
	bool bFoundNorthNeighborVertex = false;

	FVector SouthNeighborVector;
	bool bFoundSouthNeighborVertex = false;

	FVector EastNeighborVector;
	bool bFoundEastNeighborVertex = false;

	FVector WestNeighborVector;
	bool bFoundWestNeighborVertex = false;

	int32 XDim = InLandscapeProperties.LandscapeSize + 1;
	int32 YDim = InLandscapeProperties.LandscapeSize + 1;

	for (int32 i = 0; i < Radius; i++)
	{
		if (!bFoundNorthNeighborVertex)
		{
			bFoundNorthNeighborVertex = InProcMeshData.FindVertexVector(FVector(InVertex.X + 1, InVertex.Y, 0), NorthNeighborVector, 0);
		}

		if (!bFoundSouthNeighborVertex)
		{
			bFoundSouthNeighborVertex = InProcMeshData.FindVertexVector(FVector(InVertex.X - 1, InVertex.Y, 0), SouthNeighborVector, 0);
		}

		if (!bFoundEastNeighborVertex)
		{
			bFoundEastNeighborVertex = InProcMeshData.FindVertexVector(FVector(InVertex.X, InVertex.Y + 1, 0), EastNeighborVector, 0);
		}

		if (!bFoundWestNeighborVertex)
		{
			bFoundWestNeighborVertex = InProcMeshData.FindVertexVector(FVector(InVertex.X, InVertex.Y - 1, 0), WestNeighborVector, 0);
		}
	}

	if (!bFoundNorthNeighborVertex)
	{
		bFoundNorthNeighborVertex = NorthernNeighbor.FindVertexVector(FVector(0, InVertex.Y, 0), NorthNeighborVector, 0);

		if (bFoundNorthNeighborVertex)
		{
			int32 sixnine = 69;
		}
	}

	if (!bFoundSouthNeighborVertex)
	{
		bFoundSouthNeighborVertex = SouthernNeighbor.FindVertexVector(FVector(XDim, InVertex.Y, 0), SouthNeighborVector, 0);

		if (bFoundSouthNeighborVertex)
		{
			int32 sixnine = 69;
		}
	}

	if (!bFoundEastNeighborVertex)
	{
		bFoundEastNeighborVertex = EasternNeighbor.FindVertexVector(FVector(InVertex.X, 0, 0), EastNeighborVector, 0);

		if (bFoundEastNeighborVertex)
		{
			int32 sixnine = 69;
		}
	}

	if (!bFoundWestNeighborVertex)
	{
		bFoundWestNeighborVertex = WesternNeighbor.FindVertexVector(FVector(InVertex.X, YDim, 0), WestNeighborVector, 0);

		if (bFoundWestNeighborVertex)
		{
			int32 sixnine = 69;
		}
	}

	int32 NeighborHeightSum = (bFoundNorthNeighborVertex * NorthNeighborVector.Z)
		+ (bFoundSouthNeighborVertex * SouthNeighborVector.Z)
		+ (bFoundWestNeighborVertex * WestNeighborVector.Z)
		+ (bFoundEastNeighborVertex * EastNeighborVector.Z);

	int32 NeighborSum = (bFoundNorthNeighborVertex
		+ bFoundSouthNeighborVertex
		+ bFoundWestNeighborVertex
		+ bFoundEastNeighborVertex);

	int32 NeighborHeightAvg = 0;

	if (NeighborSum > 0)
	{
		NeighborHeightAvg = NeighborHeightSum / NeighborSum;
	}

	return NeighborHeightAvg;
}

TArray<FVector2D> AJetLandscapeMesh::CreateLandscapeUVArray(int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation)
{
	int32 VertexNum = (InLandscapeSize + 1) * (InLandscapeSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector2D> OutUVArray;

	int32 i = 0;

	int32 XDim = InLandscapeSize + 1;
	int32 YDim = InLandscapeSize + 1;

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			OutUVArray.Add(FVector2D(x, y));
		}
	}

	return OutUVArray;
}

TArray<int32> AJetLandscapeMesh::CreateLandscapeTriangleArray(const TMap<FVector, int32>& InVertexIndexMap, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation)
{

	int32 VertexNum = (InLandscapeSize + 1) * (InLandscapeSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<int32> OutTriangleArray;

	bool bReverseTriangles = false;

	bool bReversed = false;

	bool bEven = (InLandscapeSize % 2) == 0;

	for (y = 0; y < InLandscapeSize + 1; y++)
	{
		for (x = 0; x < InLandscapeSize + 1; x++)
		{
			TArray<int32> CurrentTriangles;

			if (!bReverseTriangles)
			{
				int32 IndexOne = GetVertexIndex(InVertexIndexMap, FVector(x, y, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexTwo = GetVertexIndex(InVertexIndexMap, FVector(x, y + 1, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexThree = GetVertexIndex(InVertexIndexMap, FVector(x + 1, y + 1, 0), FVector(InLandscapeSize, InLandscapeSize, 0));

				CurrentTriangles.Add(IndexOne);
				CurrentTriangles.Add(IndexTwo);
				CurrentTriangles.Add(IndexThree);

				int32 IndexFour = GetVertexIndex(InVertexIndexMap, FVector(x, y, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexFive = GetVertexIndex(InVertexIndexMap, FVector(x + 1, y + 1, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexSix = GetVertexIndex(InVertexIndexMap, FVector(x + 1, y, 0), FVector(InLandscapeSize, InLandscapeSize, 0));

				CurrentTriangles.Add(IndexFour);
				CurrentTriangles.Add(IndexFive);
				CurrentTriangles.Add(IndexSix);
			}
			else
			{
				/*
				*	|\
				*/


				int32 IndexOne = GetVertexIndex(InVertexIndexMap, FVector(x, y, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexTwo = GetVertexIndex(InVertexIndexMap, FVector(x, y + 1, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexThree = GetVertexIndex(InVertexIndexMap, FVector(x + 1, y, 0), FVector(InLandscapeSize, InLandscapeSize, 0));

				CurrentTriangles.Add(IndexOne);
				CurrentTriangles.Add(IndexTwo);
				CurrentTriangles.Add(IndexThree);

				int32 IndexFour = GetVertexIndex(InVertexIndexMap, FVector(x+1, y+1, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexFive = GetVertexIndex(InVertexIndexMap, FVector(x + 1, y, 0), FVector(InLandscapeSize, InLandscapeSize, 0));
				int32 IndexSix = GetVertexIndex(InVertexIndexMap, FVector(x, y + 1, 0), FVector(InLandscapeSize, InLandscapeSize, 0));

				CurrentTriangles.Add(IndexFour);
				CurrentTriangles.Add(IndexFive);
				CurrentTriangles.Add(IndexSix);

			}

			bReverseTriangles = !bReverseTriangles;

			OutTriangleArray.Append(CurrentTriangles);
		}

		if (!bEven)
		{
			bReverseTriangles = !bReverseTriangles;
		}
	}

	return OutTriangleArray;
}

void AJetLandscapeMesh::BeginPlay()
{
	int32 VectorScale = TileSize * LandscapeSize;

	/*NeighborSpawnTransformMap.Add(ECardinalDirection::North, FTransform(LandscapeStatics::North * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Northeast, FTransform(LandscapeStatics::Northeast * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Northwest, FTransform(LandscapeStatics::Northwest * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::West, FTransform(LandscapeStatics::West * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::East, FTransform(LandscapeStatics::East * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Southeast, FTransform(LandscapeStatics::Southeast * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::South, FTransform(LandscapeStatics::South * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Southwest, FTransform(LandscapeStatics::Southwest * VectorScale));

*/

	SphereCollider->SetSphereRadius(100.0f);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionObjectType(ECC_GameTraceChannel2);

	//SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	SphereCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	TArray<FVector> FeatureArray;

	FeatureArray.Add(FVector(0, 0, 0));
	FeatureArray.Add(FVector(0, 1, 0));
	FeatureArray.Add(FVector(1, 0, 0));
	FeatureArray.Add(FVector(1, 1, 0));

	LandscapeProperties.LandscapeSize = LandscapeSize;
	LandscapeProperties.TileSize = TileSize;
	LandscapeProperties.HeightVariation = HeightVariation;
	LandscapeProperties.NeighborSpawnRadius = NeighborSpawnRadius;
	LandscapeProperties.LandscapeClass = GetClass();

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (bAutoCreateLandscape)
	{
		
		//ProcMeshData = CreateLandscapeData(GetActorTransform(), LandscapeProperties);

		////CreateLandscape(LandscapeSize);

		//AddLandscapeFeature(FVector(0, 0, 0), FeatureArray);

		//AddLandscapeFeature(FVector(2, 2, 0), FeatureArray);

		//CreateMesh();

		//if (GameState)
		//{
		//	GameState->OnLandscapeSpawned(this, ProcMeshData);
		//}

		/*if (GameState)
		{
			GameState->CreateWorldLandscapes(GetActorLocation(), WorldRadius, LandscapeProperties);
		}*/
	}

	Super::BeginPlay();

	//OnLandscapeDataCreated(this, ProcMeshData);
	//if (bSpawnNeighborLandscapesAtBeginPlay)
	//{
	//	//SpawnNeighborLandscapesInRadius();
	//	SpawnNeighborLandscapesInRadius(this, GetActorLocation(), NeighborSpawnRadius, LandscapeProperties);
	//}
}

void AJetLandscapeMesh::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (GameState)
	{
		GameState->OnLandscapeDestroyed(this, ProcMeshData);
	}

	Super::EndPlay(EndPlayReason);
}

void AJetLandscapeMesh::AddLandscapeFeature(const FVector InFeatureLocation, TArray<FVector> InFeatureVertexArray)
{
	for (FVector Vertex : InFeatureVertexArray)
	{
		int32 VertexIndex = GetVertexIndex(ProcMeshData.FaceVertexMapArray[0].VertexIndexMap, InFeatureLocation + Vertex, FVector(LandscapeSize, LandscapeSize, 0));

		ProcMeshData.Vertices[VertexIndex].Z = Vertex.Z;
	}
}

int32 AJetLandscapeMesh::GetVertexIndexOld(const FVector2D InVertexLocation, const int32 InSize)
{
	FVector AdjVertexLocation = FVector(InVertexLocation.X, InVertexLocation.Y, 0);

	if (AdjVertexLocation.X > InSize)
	{
		AdjVertexLocation.X = InSize;
	}
	else if (AdjVertexLocation.X < 0)
	{
		AdjVertexLocation.X = 0;
	}

	if (AdjVertexLocation.Y > InSize)
	{
		AdjVertexLocation.Y = InSize;
	}
	else if (AdjVertexLocation.Y < 0)
	{
		AdjVertexLocation.Y = 0;
	}

	int32* IndexPtr = GetLandscapeVertexMap().Find(AdjVertexLocation);

	if (IndexPtr)
	{
		return *IndexPtr;
	}
	
	return -1;

}

TMap<FVector, int32> AJetLandscapeMesh::GetLandscapeVertexMap()
{
	if (ProcMeshData.FaceVertexMapArray.Num() == 0)
	{
		return TMap<FVector, int32>();
	}

	return ProcMeshData.FaceVertexMapArray[0].VertexIndexMap;
}

PRAGMA_ENABLE_OPTIMIZATION