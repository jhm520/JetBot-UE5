// Fill out your copyright notice in the Description page of Project Settings.


#include "JetLandscapeMesh.h"
#include "Kismet/GameplayStatics.h"
#include "../JetBot.h"
#include "JetGameState.h"
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

void AJetLandscapeMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

FVector AJetLandscapeMesh::StaticGetLandscapeCenter(const FProcMeshData& InLandscapeData, const FLandscapeProperties& InLandscapeProperties)
{
	FVector OutLandscapeCenter;

	float Mod = ((float) InLandscapeProperties.GetVectorScale()) / 2.0f;

	FVector VectorMod = FVector(Mod, Mod, 0);

	return InLandscapeData.SpawnTransform.GetLocation() + VectorMod;
}

FVector AJetLandscapeMesh::GetLandscapeCenter()
{
	return AJetLandscapeMesh::StaticGetLandscapeCenter(ProcMeshData, LandscapeProperties);
}

void AJetLandscapeMesh::OnPlayerEnteredLandscape(ACharacter* InPlayer, const FVector& InLandscapeSegmentVector)
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

	/*if (WorldSpawner)
	{
		WorldSpawner->WorldSpawner_OnPlayerEnteredLandscape(this, InPlayer, InLandscapeSegmentVector);

	}*/


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

FProcMeshData AJetLandscapeMesh::CreateLandscapeData(const FTransform& InSpawnTransform, const FLandscapeProperties& InLandscapeProperties, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap, TMap<FVector, FLandscapeVertexData>& InOutLandscapeVerticesMap, TSet<FVector>& OutNewVertexSet)
{
	FProcMeshData OutProcMeshData;

	FProcMeshFaceVertexMap LandscapeVertexMap;
	OutProcMeshData.FaceVertexMapArray.Add(LandscapeVertexMap);

	OutProcMeshData.SpawnTransform = InSpawnTransform;
	//OutProcMeshData.Vertices = CreateLandscapeVertexArray(InLandscapeProperties, OutProcMeshData, InOutLandscapeDataMap);

	OutProcMeshData.Vertices = CreateLandscapeVertexArrayImproved(InLandscapeProperties, OutProcMeshData, InOutLandscapeDataMap, InOutLandscapeVerticesMap, InSpawnTransform.GetLocation(), OutNewVertexSet);

	OutProcMeshData.UVs = CreateLandscapeUVArray(InLandscapeProperties.LandscapeSize, InLandscapeProperties.TileSize, InLandscapeProperties.HeightVariation, InSpawnTransform.GetLocation());

	OutProcMeshData.Triangles = CreateLandscapeTriangleArray(OutProcMeshData.FaceVertexMapArray[0].VertexIndexMap, InLandscapeProperties.LandscapeSize, InLandscapeProperties.TileSize, InLandscapeProperties.HeightVariation);

	OutProcMeshData.PopulateTriangleData();

	OutProcMeshData.PopulateNormals();

	OutProcMeshData.Materials.Add(InLandscapeProperties.LandscapeMaterialClass);

	return OutProcMeshData;
}

void AJetLandscapeMesh::SmoothLandscapeVertices(const TArray<FVector>& InVerticesArray, const FLandscapeProperties& InLandscapeProperties, TMap<FVector, FLandscapeVertexData>& InOutWorldLandscapeVerticesMap)
{
}

bool AJetLandscapeMesh::UpdateWorldVertex(const FVector& InWorldVertex, const int32 InHeight, const FLandscapeProperties& InLandscapeProperties, FProcMeshData& InOutLandscapeData)
{
	FVector LandscapeRelativeLoc = InWorldVertex - InOutLandscapeData.SpawnTransform.GetLocation();

	FVector IndexMapLoc = LandscapeRelativeLoc / InLandscapeProperties.TileSize;

	int32* VertexIndexPtr = InOutLandscapeData.FaceVertexMapArray[0].VertexIndexMap.Find(IndexMapLoc);

	if (!VertexIndexPtr)
	{
		return false;
	}

	FVector NewVertex = LandscapeRelativeLoc;

	NewVertex.Z = InHeight;


	InOutLandscapeData.Vertices[*VertexIndexPtr] = NewVertex;

	return true;
}

void AJetLandscapeMesh::UpdateLandscapeVertexMap(const TArray<FVector>& InUpdatedVertices, const FLandscapeProperties& InLandscapeProperties, TArray<FProcMeshData>& InOutLandscapeDataArray, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap, const TMap<FVector, FLandscapeVertexData>& InOutWorldLandscapeVerticesMap)
{
	for (const FVector& UpdatedVertex : InUpdatedVertices)
	{
		const FLandscapeVertexData* WorldVertexPtr = InOutWorldLandscapeVerticesMap.Find(UpdatedVertex);

		if (!WorldVertexPtr)
		{
			continue;
		}

		FVector NewVertex = UpdatedVertex;

		NewVertex.Z = WorldVertexPtr->Height;

		for (FProcMeshData& Landscape : InOutLandscapeDataArray)
		{
			bool bUpdated = UpdateWorldVertex(UpdatedVertex, WorldVertexPtr->Height, InLandscapeProperties, Landscape);

			if (!bUpdated)
			{
				continue;
			}
		}
	}

	//update the landscape map
	for (FProcMeshData& Landscape : InOutLandscapeDataArray)
	{
		InOutLandscapeDataMap.Add(Landscape.SpawnTransform.GetLocation() * FVector(1, 1, 0), Landscape);
	}

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

//void AJetLandscapeMesh::SpawnNeighborLandscapesInRadius(UObject* WorldContextObject, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, AJetWorldSpawner* InWorldSpawner, TMap<FVector, FProcMeshData>& InLandscapeDataMap)
//{
//	/*if (bHasSpawnedNeighborLandscapes)
//	{
//		return;
//	}*/
//
//	int32 i = 0;
//
//	TMap<FVector, FProcMeshData> OutProcMeshData;
//
//	TArray<FProcMeshData> LocalLandscapeSpawnQueue;
//
//	int32 x = 0;
//	int32 y = 0;
//
//	int32 xDim = (InLandscapeProperties.NeighborSpawnRadius * 2) + 1;
//	int32 yDim = xDim;
//
//	int32 Modifier = xDim/2;
//
//	TArray<FTransform> LocalSpawnTransformQueue;
//
//	for (y = 0; y < yDim; y++)
//	{
//		for (x = 0; x < xDim; x++)
//		{
//
//			FProcMeshData Landscape;
//
//			FVector MapKey = FVector(InLocation + (InLandscapeProperties.GetVectorScale() * (FVector(x - Modifier, y - Modifier, 0))));
//
//			MapKey = MapKey * FVector(1, 1, 0);
//
//
//			bool bFoundLandscape = FindLandscapeData(InLandscapeDataMap, MapKey, Landscape, InLandscapeProperties);
//
//			if (bFoundLandscape)
//			{
//				if (!Landscape.bIsActive)
//				{
//					LocalLandscapeSpawnQueue.Add(Landscape);
//				}
//
//				continue;
//			}
//
//			FTransform NewTileSpawnTransform = FTransform(MapKey);
//
//			TSet<FVector> VertexSet;
//
//			Landscape = CreateLandscapeData(NewTileSpawnTransform, InLandscapeProperties, InLandscapeDataMap, InWorldSpawner->WorldLandscapeVertexMap, VertexSet);
//
//			if (x > 0)
//			{
//				ZipLandscapeDataWithNeighbor(ECardinalDirection::South, Landscape, InLandscapeProperties, InLandscapeDataMap);
//			}
//			else
//			{
//				ZipLandscapeDataWithNeighbor(ECardinalDirection::North, Landscape, InLandscapeProperties, InLandscapeDataMap);
//			}
//
//			if (y > 0)
//			{
//				ZipLandscapeDataWithNeighbor(ECardinalDirection::West, Landscape, InLandscapeProperties, InLandscapeDataMap);
//			}
//			else
//			{
//				ZipLandscapeDataWithNeighbor(ECardinalDirection::East, Landscape, InLandscapeProperties, InLandscapeDataMap);
//			}
//
//			LocalLandscapeSpawnQueue.Add(Landscape);
//		}
//	}
//
//	// Spawn all of the new neighbors
//	for (FProcMeshData& Landscape : LocalLandscapeSpawnQueue)
//	{
//		Landscape.bIsActive = true;
//		OnLandscapeDataCreated(WorldContextObject, Landscape);
//		SpawnLandscapeWithData(WorldContextObject, Landscape, InLandscapeProperties, InWorldSpawner);
//	}
//
//	//bHasSpawnedNeighborLandscapes = true;
//}

void AJetLandscapeMesh::CreateLandscapesInRadius(const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TArray<FProcMeshData>& InOutLandscapeDataArray, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap, TMap<FVector, FLandscapeVertexData>& InOutLandscapeVerticesMap, TMap<FVector, FVector>& InOutLandscapeNormalMap)
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

	//int32 Selector = UKismetMathLibrary::RandomIntegerInRange(0, 1);

	TSet<FVector> LocalNewVerticesSet;

	static int32 Selector = 0;

	for (yEven = 0; yEven < 2; yEven++)
	{
		for (y = 0; y < yDim; y++)
		{
			if ((y % 2 == Selector) != (yEven == 0))
			{
				continue;
			}

			for (xEven = 0; xEven < 2; xEven++)
			{
				for (x = 0; x < xDim; x++)
				{
					if ((x % 2 == Selector) != (xEven == 0))
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
					
					Landscape = CreateLandscapeData(NewTileSpawnTransform, InLandscapeProperties, InOutLandscapeDataMap, InOutLandscapeVerticesMap, LocalNewVerticesSet);


					InOutLandscapeDataArray.Add(Landscape);
					InOutLandscapeDataMap.Add(Landscape.SpawnTransform.GetLocation() * FVector(1, 1, 0), Landscape);
				}
			}
		}
	}

	//for each new landscape data, smooth the landscape

	const TArray<FVector>& LocalNewVerticesArray = LocalNewVerticesSet.Array();
	
	//smooth the landscape vertices and update the World Landscape Vertices Map
	//TODO: Write the SmoothLandscapeVertices() function
	/*SmoothLandscapeVertices(LocalNewVerticesArray, InLandscapeProperties, InOutLandscapeVerticesMap);

	UpdateLandscapeVertexMap(LocalNewVerticesArray, InLandscapeProperties, InOutLandscapeDataArray, InOutLandscapeDataMap, InOutLandscapeVerticesMap);*/

	UpdateLandscapeNormals(InOutLandscapeDataArray, InOutLandscapeNormalMap, InLandscapeProperties);

	//TODO: Set the world UVs

	Selector++;

	if (Selector > 1)
	{
		Selector = 0;
	}
}

void AJetLandscapeMesh::UpdateLandscapeNormals(TArray<FProcMeshData>& InOutProcMeshArray, TMap<FVector, FVector>& InOutNormalsMap, const FLandscapeProperties& InLandscapeProperties)
{
	//for each new landscape data, compute the normals

	for (const FProcMeshData& LandscapeItr : InOutProcMeshArray)
	{
		const int32 VerticesNum = LandscapeItr.Vertices.Num();
		for (int32 i = 0; i < VerticesNum; i++)
		{
			FVector WorldMapKey = LandscapeItr.SpawnTransform.GetLocation() + LandscapeItr.Vertices[i];
			WorldMapKey *= FVector(1, 1, 0);

			FVector* NormalPtr = InOutNormalsMap.Find(WorldMapKey);

			FVector CurrentNormal = FVector::ZeroVector;

			if (NormalPtr)
			{
				CurrentNormal = *NormalPtr;
			}
			else
			{
				CurrentNormal = LandscapeItr.Normals[i];
			}


			InOutNormalsMap.Add(WorldMapKey, CurrentNormal);
		}
	}

	//now set the normals
	for (FProcMeshData& LandscapeItr : InOutProcMeshArray)
	{
		const int32 VerticesNum = LandscapeItr.Vertices.Num();
		for (int32 i = 0; i < VerticesNum; i++)
		{
			FVector WorldMapKey = LandscapeItr.SpawnTransform.GetLocation() + LandscapeItr.Vertices[i];
			WorldMapKey *= FVector(1, 1, 0);
			FVector* NormalPtr = InOutNormalsMap.Find(WorldMapKey);

			if (NormalPtr)
			{
				FVector NewNormal = *NormalPtr;
				NewNormal = NewNormal.GetSafeNormal();
				LandscapeItr.Normals[i] = NewNormal;
			}
		}
	}
}

void AJetLandscapeMesh::SmoothLandscape(FProcMeshData& InOutProcMesh, float InSmoothingConstant, const FLandscapeProperties& InLandscapeProperties, TMap<FVector, FLandscapeVertexData>& InOutVertexDataMap)
{
	int32 XDim = InLandscapeProperties.LandscapeSize + 1;
	int32 YDim = InLandscapeProperties.LandscapeSize + 1;

	const int32 VerticesNum = InOutProcMesh.Vertices.Num();

	TArray<FVector> TempVertices;

	for (int32 y = 0; y < YDim; y++)
	{
		for (int32 x = 0; x < XDim; x++)
		{
			TempVertices.Add(FVector(x, y, 0));
		}
	}

	TArray<FVector> RandomizedVertices;

	for (int32 i = 0; i < VerticesNum; i++)
	{
		int32 RandInt = UKismetMathLibrary::RandomInteger(VerticesNum - 1 - i);

		RandomizedVertices.Add(TempVertices[i]);

		TempVertices.RemoveAt(i);
	}

	for (int32 i = 0; i < VerticesNum; i++)
	{
		const FVector CurrentMapKey = RandomizedVertices[i];

		const FLandscapeVertexData AvgNeighborData = FindAverageVertexNeighborData(CurrentMapKey, InLandscapeProperties, InOutProcMesh, InOutVertexDataMap);

	}
}

void AJetLandscapeMesh::CreateLandscapeDataInRadius(const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TArray<FProcMeshData>& InOutLandscapeDataArray, FProcMeshData& InOutSuperLandscapeData, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap, TMap<FVector, FLandscapeVertexData>& InOutLandscapeVerticesMap)
{
	/*int32 LandscapeTileDimensions = (InLandscapeProperties.NeighborSpawnRadius * 2) + 1;

	int32 XDim = InLandscapeProperties.LandscapeSize * LandscapeTileDimensions;
	int32 YDim = XDim;

	FLandscapeProperties SuperLandscapeProperties = InLandscapeProperties;

	SuperLandscapeProperties.TileSize = InLandscapeProperties.TileSize;
	SuperLandscapeProperties.LandscapeSize = InLandscapeProperties.LandscapeSize * LandscapeTileDimensions;

	float Modifier = (float)LandscapeTileDimensions / 2.0f;*/

	FVector MapKey = FVector(InLocation + (InLandscapeProperties.GetVectorScale() * (FVector(-0.5f, -0.5f, 0.0f))));

	MapKey = MapKey * FVector(1, 1, 0);

	TSet<FVector> VertexSet;

	InOutSuperLandscapeData = CreateLandscapeData(FTransform(MapKey), InLandscapeProperties, InOutLandscapeDataMap, InOutLandscapeVerticesMap, VertexSet);
	
	//TODO: Split this super-landscape into smaller landscapes

	//InOutSuperLandscapeData.SpawnTransform.SetLocation(MapKey);

	InOutLandscapeDataArray.Add(InOutSuperLandscapeData);

	InOutLandscapeDataMap.Add(MapKey, InOutSuperLandscapeData);
}

void AJetLandscapeMesh::SplitSuperLandscape(const FLandscapeProperties& InSuperLandscapeProperties, const FProcMeshData& InSuperLandscape, const FLandscapeProperties& InSplitLandscapeProperties, TArray<FProcMeshData>& OutSplitLandscapeArray)
{

	int32 xSplit = 0;
	int32 ySplit = 0;

	const int32 SuperXDim = (InSplitLandscapeProperties.NeighborSpawnRadius * 2) + 1;

	const int32& LandscapeTileDimensions = SuperXDim;

	const int32 SuperYDim = SuperXDim;

	FProcMeshData CurrentSplitLandscape = FProcMeshData();

	CurrentSplitLandscape.SpawnTransform = InSuperLandscape.SpawnTransform;

	int32 SuperI = 0;

	for (ySplit = 0; ySplit < SuperYDim; ySplit++)
	{
		for (xSplit = 0; xSplit < SuperXDim; xSplit++)
		{
			int32 XDim = InSplitLandscapeProperties.LandscapeSize;
			int32 YDim = InSplitLandscapeProperties.LandscapeSize;
			
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
		return nullptr/*SpawnLandscapeWithData(this, *DataPtr, LandscapeProperties, WorldSpawner)*/;
	}
	TMap<FVector, FLandscapeVertexData> VectorMap;

	TSet<FVector> VertexSet;

	//Create a new landscape and spawn it
	FProcMeshData NewLandscapeData = CreateLandscapeData(NeighborTransform, LandscapeProperties, GameState->LandscapeDataMap, VectorMap, VertexSet);

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

	TMap<FVector, FLandscapeVertexData> VectorMap;
	TSet<FVector> VertexSet;


	//Create a new landscape and spawn it
	FProcMeshData NewLandscapeData = CreateLandscapeData(NeighborTransform, InLandscapeProperties, GameState->LandscapeDataMap, VectorMap, VertexSet);
	
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
	TArray<UMaterialInterface*> MatArray;

	SpawnedActor->CreateMesh(MatArray);

	

	return SpawnedActor;
}

//AJetLandscapeMesh* AJetLandscapeMesh::SpawnLandscapeWithData(UObject* WorldContextObject, const FProcMeshData& InProcMeshData, const FLandscapeProperties& InLandscapeProperties, AJetWorldSpawner* InWorldSpawner)
//{
//	FActorSpawnParameters ActorSpawnParams = FActorSpawnParameters();
//	UClass* LandscapeClass = InLandscapeProperties.LandscapeClass;
//
//	//Spawn the landscape deferred
//	AJetLandscapeMesh* SpawnedActor = WorldContextObject->GetWorld()->SpawnActorDeferred<AJetLandscapeMesh>(LandscapeClass, InProcMeshData.SpawnTransform);
//
//	if (!SpawnedActor)
//	{
//		return nullptr;
//	}
//
//	if (!InWorldSpawner)
//	{
//		return nullptr;
//	}
//
//	SpawnedActor->bAutoCreateLandscape = false;
//	SpawnedActor->LandscapeSize = InLandscapeProperties.LandscapeSize;
//	SpawnedActor->TileSize = InLandscapeProperties.TileSize;
//	SpawnedActor->HeightVariation = InLandscapeProperties.HeightVariation;
//	SpawnedActor->NeighborSpawnRadius = InLandscapeProperties.NeighborSpawnRadius;
//
//	SpawnedActor->LandscapeProperties = InLandscapeProperties;
//	SpawnedActor->WorldSpawner = InWorldSpawner;
//
//
//	SpawnedActor->ProcMeshData = InProcMeshData;
//
//	UGameplayStatics::FinishSpawningActor(SpawnedActor, InProcMeshData.SpawnTransform);
//	
//	TArray<UMaterialInterface*> MatArray;
//
//	if (InWorldSpawner)
//	{
//		MatArray.Add(InWorldSpawner->WorldLandscapeMaterial);
//	}
//
//	SpawnedActor->CreateMesh(MatArray);
//
//	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());
//
//	if (GameState)
//	{
//		GameState->OnLandscapeSpawned(SpawnedActor, SpawnedActor->ProcMeshData);
//	}
//
//	return SpawnedActor;
//}

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

TArray<FVector> AJetLandscapeMesh::CreateLandscapeVertexArrayNew(const FLandscapeProperties& InLandscapeProperties, FProcMeshData& InOutProcMeshData, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap, TMap<FVector, FLandscapeVertexData>& InOutLandscapeVerticesMap, const FVector& InSpawnLocation)
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

	int32 SelectStartCorner = UKismetMathLibrary::RandomIntegerInRange(0, 3);

	//static int32 SelectStartCorner = 0;


	//if (SelectStartCorner == 4)
	//{
	//	//"OUTSIDE IN" ITERATION
	//	int32 LandscapeRadius = XDim / 2;

	//	if (XDim % 2 == 1)
	//	{
	//		LandscapeRadius++;
	//	}

	//	for (int32 r = 0; r < LandscapeRadius; r++)
	//	{
	//		for (int32 i = r; i < XDim - (r); i++)
	//		{
	//			TempVertices.AddUnique(FVector(i, r, 0));
	//		}

	//		for (int32 i = r; i < XDim - (r); i++)
	//		{
	//			TempVertices.AddUnique(FVector(r, i, 0));
	//		}

	//		for (int32 i = r; i < XDim - (r); i++)
	//		{
	//			TempVertices.AddUnique(FVector(XDim - 1 - r, i, 0));
	//		}

	//		for (int32 i = r; i < XDim - (r); i++)
	//		{
	//			TempVertices.AddUnique(FVector(i, XDim - 1 - r, 0));
	//		}
	//	}
	//}
	//else
	//{
		//randomly selected iteration
		switch (SelectStartCorner)
		{
		case 0:
		{
			for (y = 0; y < YDim; y++)
			{
				for (x = 0; x < XDim; x++)
				{
					TempVertices.Add(FVector(x, y, 0));
				}
			}
			break;
		}
		case 1:
		{
			for (y = 0; y < YDim; y++)
			{
				for (x = XDim - 1; x > -1; x--)
				{
					TempVertices.Add(FVector(x, y, 0));
				}
			}
			break;
		}
		case 2:
		{
			for (y = YDim - 1; y > -1; y--)
			{
				for (x = 0; x < XDim; x++)
				{
					TempVertices.Add(FVector(x, y, 0));
				}
			}
			break;
		}
		case 3:
		{
			for (y = YDim - 1; y > -1; y--)
			{
				for (x = XDim - 1; x > -1; x--)
				{
					TempVertices.Add(FVector(x, y, 0));
				}
			}
			break;
		}
		}
	//}
	
	
	/*SelectStartCorner++;

	if (SelectStartCorner > 3)
	{
		SelectStartCorner = 0;
	}*/

	const int32 TotalNumVertices = TempVertices.Num();

	OutVertexArray.SetNumZeroed(TotalNumVertices);
	InOutProcMeshData.Vertices.SetNumZeroed(TotalNumVertices);
	int32 CurrentNumVertices = TempVertices.Num();


	for (int32 i = 0; i < TotalNumVertices; i++)
	{
		//const int32 CurrentRandomVertexIndex = UKismetMathLibrary::RandomIntegerInRange(0, CurrentNumVertices-1);

		//const FVector& CurrentMapKey = TempVertices[CurrentRandomVertexIndex];
		const FVector& CurrentMapKey = TempVertices[i];
		const FVector& CurrentScaledWorldMapKey = InSpawnLocation + CurrentMapKey*InLandscapeProperties.TileSize;

		FLandscapeVertexData* VertexPtr = InOutLandscapeVerticesMap.Find(CurrentScaledWorldMapKey);

		FLandscapeVertexData NewVertexData;
		int32 NewHeight = 0;

		if (VertexPtr)
		{
			NewVertexData = *VertexPtr;

			const FLandscapeVertexData AvgNeighborData = FindAverageVertexNeighborData(CurrentMapKey, InLandscapeProperties, InOutProcMeshData, InOutLandscapeVerticesMap);

			NewVertexData.AvgNeighborHeight = AvgNeighborData.Height;

			NewHeight = NewVertexData.Height;
		}
		else
		{
			NewVertexData = FLandscapeVertexData();

			const FLandscapeVertexData AvgNeighborData = FindAverageVertexNeighborData(CurrentMapKey, InLandscapeProperties, InOutProcMeshData, InOutLandscapeVerticesMap);

			NewVertexData.AvgNeighborHeight = AvgNeighborData.Height;

			const int32 HeightVariation = UKismetMathLibrary::RandomIntegerInRange(-InLandscapeProperties.HeightVariation, InLandscapeProperties.HeightVariation);

			const int32 NewMod = AvgNeighborData.Height == 0 ? HeightVariation : AvgNeighborData.Height;

			float Rise = AvgNeighborData.Height - AvgNeighborData.AvgNeighborHeight;

			float AvgNeighborSlope = Rise / (float) InLandscapeProperties.TileSize;

			float NewSlopeAdd = 0.0f;

			if (AvgNeighborData.Height > InLandscapeProperties.MaximumHeight)
			{
				NewSlopeAdd += UKismetMathLibrary::RandomFloatInRange(-InLandscapeProperties.MaximumSlopeDifference, 0);

			}
			else if (AvgNeighborData.Height < InLandscapeProperties.MinimumHeight)
			{
				NewSlopeAdd += UKismetMathLibrary::RandomFloatInRange(0, InLandscapeProperties.MaximumSlopeDifference);
			}
			else
			{
				NewSlopeAdd += UKismetMathLibrary::RandomFloatInRange(-InLandscapeProperties.MaximumSlopeDifference, InLandscapeProperties.MaximumSlopeDifference);
			}

			float NewSlope = AvgNeighborSlope + NewSlopeAdd;

			NewSlope = UKismetMathLibrary::FClamp(NewSlope, -InLandscapeProperties.MaximumSlope, InLandscapeProperties.MaximumSlope);


			float NewHeightDiff = NewSlope * InLandscapeProperties.TileSize;

			NewHeight = NewMod + NewHeightDiff;

			//int32 AvgHeightDiff = NewMod - AvgNeighborData.AvgNeighborHeight;

			////NewHeight = UKismetMathLibrary::RandomIntegerInRange(NewMod - AvgHeightDiff, NewMod + AvgHeightDiff);

			//NewHeight = NewMod + AvgHeightDiff/* + UKismetMathLibrary::RandomIntegerInRange(-20, 20)*/;

			//if (NewHeight > InLandscapeProperties.MaximumHeight)
			//{
			//	NewHeight += UKismetMathLibrary::RandomIntegerInRange(-InLandscapeProperties.MaximumHeightDifference, 0);

			//}
			//else if (NewHeight < InLandscapeProperties.MinimumHeight)
			//{
			//	NewHeight += UKismetMathLibrary::RandomIntegerInRange(0, InLandscapeProperties.MaximumHeightDifference);
			//}
			//else
			//{
			//	NewHeight += UKismetMathLibrary::RandomIntegerInRange(-InLandscapeProperties.MaximumHeightDifference, InLandscapeProperties.MaximumHeightDifference);
			//}

			//int32 Rise = NewHeight - AvgNeighborData.Height;
			//float Slope = (float) Rise/(float) InLandscapeProperties.TileSize;

			//Slope = UKismetMathLibrary::FClamp(Slope, -InLandscapeProperties.MaximumSlope, InLandscapeProperties.MaximumSlope);

			//float SlopeDiff = Slope - AvgNeighborData.AvgNeighborSlope;

			//SlopeDiff = UKismetMathLibrary::FClamp(SlopeDiff, -InLandscapeProperties.MaximumSlopeDifference, InLandscapeProperties.MaximumSlopeDifference);

			//Slope = AvgNeighborData.AvgNeighborSlope + SlopeDiff;

			//NewHeight = NewMod + (Slope * InLandscapeProperties.TileSize);

			//NewHeight = UKismetMathLibrary::Clamp(NewHeight, InLandscapeProperties.MinimumHeight, InLandscapeProperties.MaximumHeight);
			//NewVertexData.AvgNeighborSlope = Slope;

			NewVertexData.AvgNeighborSlope = NewSlope;

			NewVertexData.Height = NewHeight;
		}
		//do stuff

		const FVector NewVertex = (CurrentMapKey * InLandscapeProperties.TileSize) + FVector(0, 0, NewHeight);


		const int32 NewVertexIndex = CurrentMapKey.X + (CurrentMapKey.Y * (YDim));

		OutVertexArray[NewVertexIndex] = NewVertex;

		InOutProcMeshData.Vertices[NewVertexIndex] = NewVertex;

		InOutProcMeshData.FaceVertexMapArray[0].VertexIndexMap.Add(CurrentMapKey, NewVertexIndex);

		InOutLandscapeVerticesMap.Add(CurrentScaledWorldMapKey, NewVertexData);

		//TempVertices.RemoveAt(CurrentRandomVertexIndex);
		//TempVertices.RemoveAt(i);
		CurrentNumVertices--;
	}


	return OutVertexArray;
}

TArray<FVector> AJetLandscapeMesh::CreateLandscapeVertexArrayImproved(const FLandscapeProperties& InLandscapeProperties, FProcMeshData& InOutProcMeshData, TMap<FVector, FProcMeshData>& InOutLandscapeDataMap, TMap<FVector, FLandscapeVertexData>& InOutLandscapeVerticesMap, const FVector& InSpawnLocation, TSet<FVector>& OutNewVertexSet)
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

	TArray<FVector> OutVertexArray;
	TArray<FVector> TempVertices;

	int32 SelectStartCorner = UKismetMathLibrary::RandomIntegerInRange(0, 3);

	//SelectStartCorner = 3;
	switch (SelectStartCorner)
	{
	case 0:
	{
		for (y = 0; y < YDim; y++)
		{
			for (x = 0; x < XDim; x++)
			{
				TempVertices.Add(FVector(x, y, 0));
			}
		}
		break;
	}
	case 1:
	{
		for (y = 0; y < YDim; y++)
		{
			for (x = XDim - 1; x > -1; x--)
			{
				TempVertices.Add(FVector(x, y, 0));
			}
		}
		break;
	}
	case 2:
	{
		for (y = YDim - 1; y > -1; y--)
		{
			for (x = 0; x < XDim; x++)
			{
				TempVertices.Add(FVector(x, y, 0));
			}
		}
		break;
	}
	case 3:
	{
		for (y = YDim - 1; y > -1; y--)
		{
			for (x = XDim - 1; x > -1; x--)
			{
				TempVertices.Add(FVector(x, y, 0));
			}
		}
		break;
	}
	}

	const int32 TotalNumVertices = TempVertices.Num();

	OutVertexArray.SetNumZeroed(TotalNumVertices);
	InOutProcMeshData.Vertices.SetNumZeroed(TotalNumVertices);
	InOutProcMeshData.WorldVertices.SetNumZeroed(TotalNumVertices);

	int32 CurrentNumVertices = TempVertices.Num();


	for (int32 i = 0; i < TotalNumVertices; i++)
	{
		const FVector& CurrentMapKey = TempVertices[i];
		const FVector& CurrentScaledWorldMapKey = InSpawnLocation + CurrentMapKey * InLandscapeProperties.TileSize;

		FLandscapeVertexData* VertexPtr = InOutLandscapeVerticesMap.Find(CurrentScaledWorldMapKey);

		FLandscapeVertexData NewVertexData;

		if (VertexPtr)
		{
			NewVertexData = *VertexPtr;

			TArray<FLandscapeVertexData> NeighborDatas = GetAllVertexNeighborDatas(CurrentMapKey, InLandscapeProperties, InOutProcMeshData, InOutLandscapeVerticesMap);

			float AvgNeighborHeightSum = 0.0f;

			int32 AvgNeighborHeightCount = 0;


			//Update the vertex data, upon accessing a vertex we've already added
			for (const FLandscapeVertexData& NeighborData : NeighborDatas)
			{
				if (NeighborData.NeighborDistance == 1)
				{
					AvgNeighborHeightSum += NeighborData.Height;
					AvgNeighborHeightCount++;
				}
			}

			if (AvgNeighborHeightCount > 0)
			{
				NewVertexData.AvgNeighborHeight = AvgNeighborHeightSum / AvgNeighborHeightCount;
			}

			float Rise = NewVertexData.Height - NewVertexData.AvgNeighborHeight;

			NewVertexData.AvgNeighborSlope = (Rise / InLandscapeProperties.TileSize) / AvgNeighborHeightCount;

			/*const FLandscapeVertexData AvgNeighborData = FindAverageVertexNeighborData(CurrentMapKey, InLandscapeProperties, InOutProcMeshData, InOutLandscapeVerticesMap);

			NewVertexData.AvgNeighborHeight = AvgNeighborData.Height;*/

		}
		else
		{
			NewVertexData = FLandscapeVertexData();
			
			TArray<FLandscapeVertexData> NeighborDatas = GetAllVertexNeighborDatas(CurrentMapKey, InLandscapeProperties, InOutProcMeshData, InOutLandscapeVerticesMap);

			float AvgProjectedHeight = 0.0f;

			float AvgProjectedHeightSum = 0.0f;

			float AvgNeighborHeightSum = 0.0f;

			int32 AvgNeighborHeightCount = 0;

			int32 AvgNeighborProjectedHeightCount = 0;

			bool bHasAdjacentNeighbor = false;

			TArray<FLandscapeVertexData> AdjacentNeighbors;

			for (const FLandscapeVertexData& NeighborData : NeighborDatas)
			{
				if (NeighborData.NeighborDistance == 1)
				{
					bHasAdjacentNeighbor = true;

					AdjacentNeighbors.Add(NeighborData);
				}
			}

			for (const FLandscapeVertexData& NeighborData : NeighborDatas)
			{

				/*if (NeighborData.NeighborDistance > 1)
				{
					continue;
				}*/

				if (NeighborData.NeighborDistance > 1)
				{
					int32 Sixnine = 69;
				}
				float SlopeAdd = 0;

				if (NeighborData.Height > InLandscapeProperties.MaximumHeight)
				{
					SlopeAdd = UKismetMathLibrary::RandomFloatInRange(-InLandscapeProperties.MaximumSlopeDifference, 0);

				}
				else if (NeighborData.Height < -InLandscapeProperties.MaximumHeight)
				{
					SlopeAdd = UKismetMathLibrary::RandomFloatInRange(0, InLandscapeProperties.MaximumSlopeDifference);
				}
				else
				{
					SlopeAdd = UKismetMathLibrary::RandomFloatInRange(-InLandscapeProperties.MaximumSlopeDifference, InLandscapeProperties.MaximumSlopeDifference);
				}


				float NewSlope = NeighborData.AvgNeighborSlope + SlopeAdd;

				NewSlope = UKismetMathLibrary::FClamp(NewSlope, -InLandscapeProperties.MaximumSlope, InLandscapeProperties.MaximumSlope);

				if (NeighborData.NeighborDistance > 1)
				{
					if (!bHasAdjacentNeighbor)
					{
						for (int32 ni = 1; ni < NeighborData.NeighborDistance; ni++)
						{
							NewSlope += UKismetMathLibrary::RandomFloatInRange(-InLandscapeProperties.MaximumSlopeDifference, InLandscapeProperties.MaximumSlopeDifference);
						}

						NewSlope = UKismetMathLibrary::FClamp(NewSlope, -InLandscapeProperties.MaximumSlope, InLandscapeProperties.MaximumSlope);

						//NewSlope = UKismetMathLibrary::RandomFloatInRange(-InLandscapeProperties.MaximumSlope, InLandscapeProperties.MaximumSlope);

						AvgProjectedHeightSum += NeighborData.Height + (NewSlope * InLandscapeProperties.TileSize * NeighborData.NeighborDistance);
						AvgNeighborProjectedHeightCount++;
					}
					else
					{
						float LocalAdjacentNeighborsAverageHeightSum = 0;
						int32 LocalAdjacentNeighborsAverageHeightCount = 0;

						for (const FLandscapeVertexData& AdjacentNeighborData : AdjacentNeighbors)
						{
							LocalAdjacentNeighborsAverageHeightSum += AdjacentNeighborData.Height;
							LocalAdjacentNeighborsAverageHeightCount++;
						}

						float LocalAdjacentNeigborsAverageHeight = LocalAdjacentNeighborsAverageHeightSum / LocalAdjacentNeighborsAverageHeightCount;


						float NeighborRise = NeighborData.Height - LocalAdjacentNeigborsAverageHeight;

						float NeighborSlope = (NeighborRise / InLandscapeProperties.TileSize) / NeighborData.NeighborDistance;

						float NewNeighborSlope = UKismetMathLibrary::RandomFloatInRange(NeighborSlope - InLandscapeProperties.MaximumSlopeInterpolationDifference, NeighborSlope + InLandscapeProperties.MaximumSlopeInterpolationDifference);

						//NewNeighborSlope = UKismetMathLibrary::FClamp(NewNeighborSlope, -InLandscapeProperties.MaximumSlope, InLandscapeProperties.MaximumSlope);


						AvgProjectedHeightSum += LocalAdjacentNeigborsAverageHeight + (NewNeighborSlope * InLandscapeProperties.TileSize);

						//AvgProjectedHeightSum += UKismetMathLibrary::Lerp(LocalAdjacentNeigborsAverageHeight, NeighborData.Height, 1 - ((NeighborData.NeighborDistance-1) / (InLandscapeProperties.LandscapeSize + 1)));
						AvgNeighborProjectedHeightCount++;

						//AvgProjectedHeightSum += NeighborData.Height + (NewSlope * InLandscapeProperties.TileSize)
					}
				}
				else
				{
					AvgProjectedHeightSum += NeighborData.Height + (NewSlope * InLandscapeProperties.TileSize * NeighborData.NeighborDistance);
					AvgNeighborProjectedHeightCount++;
				}

				//for (int32 iDist = 0; iDist < NeighborData.NeighborDistance; iDist++)
				//{
				//	NewSlope = UKismetMathLibrary::FClamp(NewSlope, -InLandscapeProperties.MaximumSlope, InLandscapeProperties.MaximumSlope);

				//	AvgProjectedHeightSum += (NewSlope * InLandscapeProperties.TileSize/* * NeighborData.NeighborDistance*/);

				//	SlopeAdd = UKismetMathLibrary::RandomFloatInRange(-InLandscapeProperties.MaximumSlopeDifference, InLandscapeProperties.MaximumSlopeDifference);

				//	NewSlope += SlopeAdd;
				//}


				if (NeighborData.NeighborDistance == 1)
				{
					AvgNeighborHeightSum += NeighborData.Height;
					AvgNeighborHeightCount++;
				}
			}

			if (NeighborDatas.Num() > 0)
			{
				NewVertexData.Height = AvgProjectedHeightSum / AvgNeighborProjectedHeightCount;

				if (AvgNeighborHeightCount > 0)
				{
					NewVertexData.AvgNeighborHeight = AvgNeighborHeightSum / AvgNeighborHeightCount;

					float Rise = NewVertexData.Height - NewVertexData.AvgNeighborHeight;

					NewVertexData.AvgNeighborSlope = (Rise / InLandscapeProperties.TileSize) / AvgNeighborHeightCount;
				}

				
			}


			//Add it to the the set of new vertices

			OutNewVertexSet.Add(CurrentScaledWorldMapKey);
		}

		//do stuff

		const FVector NewVertex = (CurrentMapKey * InLandscapeProperties.TileSize) + FVector(0, 0, NewVertexData.Height);


		const int32 NewVertexIndex = CurrentMapKey.X + (CurrentMapKey.Y * (YDim));

		OutVertexArray[NewVertexIndex] = NewVertex;

		InOutProcMeshData.Vertices[NewVertexIndex] = NewVertex;
		InOutProcMeshData.WorldVertices[NewVertexIndex] = InOutProcMeshData.SpawnTransform.GetLocation() + NewVertex;

		InOutProcMeshData.FaceVertexMapArray[0].VertexIndexMap.Add(CurrentMapKey, NewVertexIndex);

		InOutLandscapeVerticesMap.Add(CurrentScaledWorldMapKey, NewVertexData);

		CurrentNumVertices--;
	}


	return OutVertexArray;
}

FLandscapeVertexData AJetLandscapeMesh::FindAverageVertexNeighborData(const FVector& InVertex, const FLandscapeProperties& InLandscapeProperties, const FProcMeshData& InProcMeshData, TMap<FVector, FLandscapeVertexData>& InOutWorldLandscapeVertices)
{
	FLandscapeVertexData OutAverageVertexData = FLandscapeVertexData();
	const int32 Radius = InLandscapeProperties.LandscapeSize + 1;

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

	float NorthNeighborScalar = 0.0f;
	float SouthNeighborScalar = 0.0f;
	float EastNeighborScalar = 0.0f;
	float WestNeighborScalar = 0.0f;

	int32 NorthNeighborAverageHeight = 0;
	int32 SouthNeighborAverageHeight = 0;
	int32 EastNeighborAverageHeight = 0;
	int32 WestNeighborAverageHeight = 0;

	float NorthNeighborAverageSlope = 0;
	float SouthNeighborAverageSlope = 0;
	float EastNeighborAverageSlope = 0;
	float WestNeighborAverageSlope = 0;

	int32 NorthNeighborDistance = 0;
	int32 SouthNeighborDistance = 0;
	int32 EastNeighborDistance = 0;
	int32 WestNeighborDistance = 0;

	for (int32 NeighborDistance = 1; NeighborDistance < Radius; NeighborDistance++)
	{
		const float CurrentScalar = 1.0f - (((float) NeighborDistance - 1.0f) / ((float) Radius - 1.0f));

		if (!bFoundNorthNeighborVertex)
		{
			const FVector NorthNeighborKey = FVector(InVertex.X + NeighborDistance, InVertex.Y, 0);

			const FVector NorthNeighborWorldKey = NorthNeighborKey * InLandscapeProperties.TileSize;

			FLandscapeVertexData* VertexPtr = InOutWorldLandscapeVertices.Find(InProcMeshData.SpawnTransform.GetLocation() + NorthNeighborWorldKey);

			if (VertexPtr)
			{
				bFoundNorthNeighborVertex = true;
				NorthNeighborVector = NorthNeighborWorldKey;
				NorthNeighborVector.Z = VertexPtr->Height;
				NorthNeighborScalar = CurrentScalar;
				NorthNeighborAverageHeight = VertexPtr->AvgNeighborHeight;
				NorthNeighborAverageSlope = VertexPtr->AvgNeighborSlope;
				NorthNeighborDistance = NeighborDistance;

			}
		}

		if (!bFoundSouthNeighborVertex)
		{
			const FVector SouthNeighborKey = FVector(InVertex.X - NeighborDistance, InVertex.Y, 0);

			const FVector SouthNeighborWorldKey = SouthNeighborKey * InLandscapeProperties.TileSize;

			FLandscapeVertexData* VertexPtr = InOutWorldLandscapeVertices.Find(InProcMeshData.SpawnTransform.GetLocation() + SouthNeighborWorldKey);

			if (VertexPtr)
			{
				bFoundSouthNeighborVertex = true;
				SouthNeighborVector = SouthNeighborWorldKey;
				SouthNeighborVector.Z = VertexPtr->Height;
				SouthNeighborScalar = CurrentScalar;
				SouthNeighborAverageHeight = VertexPtr->AvgNeighborHeight;
				SouthNeighborAverageSlope = VertexPtr->AvgNeighborSlope;
				SouthNeighborDistance = NeighborDistance;

			}
		}

		if (!bFoundEastNeighborVertex)
		{
			const FVector EastNeighborKey = FVector(InVertex.X, InVertex.Y + NeighborDistance, 0);

			const FVector EastNeighborWorldKey = EastNeighborKey * InLandscapeProperties.TileSize;

			FLandscapeVertexData* VertexPtr = InOutWorldLandscapeVertices.Find(InProcMeshData.SpawnTransform.GetLocation() + EastNeighborWorldKey);

			if (VertexPtr)
			{
				bFoundEastNeighborVertex = true;
				EastNeighborVector = EastNeighborWorldKey;
				EastNeighborVector.Z = VertexPtr->Height;
				EastNeighborScalar = CurrentScalar;
				EastNeighborAverageHeight = VertexPtr->AvgNeighborHeight;
				EastNeighborAverageSlope = VertexPtr->AvgNeighborSlope;
				EastNeighborDistance = NeighborDistance;

			}
		}

		if (!bFoundWestNeighborVertex)
		{
			const FVector WestNeighborKey = FVector(InVertex.X, InVertex.Y - NeighborDistance, 0);

			const FVector WestNeighborWorldKey = WestNeighborKey * InLandscapeProperties.TileSize;

			FLandscapeVertexData* VertexPtr = InOutWorldLandscapeVertices.Find(InProcMeshData.SpawnTransform.GetLocation() + WestNeighborWorldKey);

			if (VertexPtr)
			{
				bFoundWestNeighborVertex = true;
				WestNeighborVector = WestNeighborWorldKey;
				WestNeighborVector.Z = VertexPtr->Height;
				WestNeighborScalar = CurrentScalar;
				WestNeighborAverageHeight = VertexPtr->AvgNeighborHeight;
				WestNeighborAverageSlope = VertexPtr->AvgNeighborSlope;
				WestNeighborDistance = NeighborDistance;


			}
		}
	}

	float NorthProjectedHeight = 0.0f;

	int32 NeighborHeightSum =
		(bFoundNorthNeighborVertex * NorthNeighborVector.Z/* * NorthNeighborScalar*/)
		+ (bFoundSouthNeighborVertex * SouthNeighborVector.Z/* * SouthNeighborScalar*/)
		+ (bFoundWestNeighborVertex * WestNeighborVector.Z/* * WestNeighborScalar*/)
		+ (bFoundEastNeighborVertex * EastNeighborVector.Z/* * EastNeighborScalar*/);

	int32 NeighborAverageHeightSum =
		(bFoundNorthNeighborVertex * NorthNeighborAverageHeight/* * NorthNeighborScalar*/)
		+ (bFoundSouthNeighborVertex * SouthNeighborAverageHeight/* * SouthNeighborScalar*/)
		+ (bFoundWestNeighborVertex * WestNeighborAverageHeight/* * WestNeighborScalar*/)
		+ (bFoundEastNeighborVertex * EastNeighborAverageHeight/* * EastNeighborScalar*/);

	float NeighborAverageSlopeSum =
		(bFoundNorthNeighborVertex * NorthNeighborAverageSlope/* * NorthNeighborScalar*/)
		+ (bFoundSouthNeighborVertex * SouthNeighborAverageSlope/* * SouthNeighborScalar*/)
		+ (bFoundWestNeighborVertex * WestNeighborAverageSlope/* * WestNeighborScalar*/)
		+ (bFoundEastNeighborVertex * EastNeighborAverageSlope/* * EastNeighborScalar*/);

	int32 NeighborSum = (bFoundNorthNeighborVertex
		+ bFoundSouthNeighborVertex
		+ bFoundWestNeighborVertex
		+ bFoundEastNeighborVertex);

	int32 NeighborHeightAvg = 0;
	int32 NeighborAverageHeightAverage = 0;
	float NeighborAverageSlopeAverage = 0;

	if (NeighborSum > 0)
	{
		NeighborHeightAvg = NeighborHeightSum / NeighborSum;
		NeighborAverageHeightAverage = NeighborAverageHeightSum / NeighborSum;
		NeighborAverageSlopeAverage = NeighborAverageSlopeSum / NeighborSum;
	}

	OutAverageVertexData.Height = NeighborHeightAvg;
	OutAverageVertexData.AvgNeighborHeight = NeighborAverageHeightAverage;
	OutAverageVertexData.AvgNeighborSlope = NeighborAverageSlopeAverage;
	//OutAverageVertexData.AvgNeighborSlope = OutAverageVertexData.Height - OutAverageVertexData.AvgNeighborHeight;

	return OutAverageVertexData;
}

bool AJetLandscapeMesh::FindNearestVertexNeighborData(ECardinalDirection InNeighborDirection, const FVector& InVertex, FLandscapeVertexData& OutLandscapeVertexData, int32& OutNeighborDistance, const FLandscapeProperties& InLandscapeProperties, const FProcMeshData& InProcMeshData, TMap<FVector, FLandscapeVertexData>& InOutWorldLandscapeVertices)
{
	FVector NeighborKey = FVector::ZeroVector;

	const int32 Radius = InLandscapeProperties.LandscapeSize + 1;

	for (int32 NeighborDistance = 1; NeighborDistance < Radius; NeighborDistance++)
	{
		//create neighbor vector mod
		switch (InNeighborDirection)
		{
			case ECardinalDirection::North:
			{
				NeighborKey = FVector(InVertex.X + NeighborDistance, InVertex.Y, 0);
				break;
			}
			case ECardinalDirection::South:
			{
				NeighborKey = FVector(InVertex.X - NeighborDistance, InVertex.Y, 0);
				break;
			}
			case ECardinalDirection::East:
			{
				NeighborKey = FVector(InVertex.X, InVertex.Y + NeighborDistance, 0);
				break;
			}
			case ECardinalDirection::West:
			{
				NeighborKey = FVector(InVertex.X, InVertex.Y - NeighborDistance, 0);
				break;
			}
		}

		const FVector NeighborWorldKey = NeighborKey * InLandscapeProperties.TileSize;

		FLandscapeVertexData* VertexPtr = InOutWorldLandscapeVertices.Find(InProcMeshData.SpawnTransform.GetLocation() + NeighborWorldKey);

		if (VertexPtr)
		{

			OutLandscapeVertexData = *VertexPtr;
			OutLandscapeVertexData.NeighborDistance = NeighborDistance;
			OutNeighborDistance = NeighborDistance;
			return true;
		}
	}

	return false;
}

TArray<FLandscapeVertexData> AJetLandscapeMesh::GetAllVertexNeighborDatas(const FVector& InVertex, const FLandscapeProperties& InLandscapeProperties, const FProcMeshData& InProcMeshData, TMap<FVector, FLandscapeVertexData>& InOutWorldLandscapeVertices)
{
	FLandscapeVertexData NorthNeighbor;
	int32 NorthNeighborDistance;
	FLandscapeVertexData SouthNeighbor;
	int32 SouthNeighborDistance;

	FLandscapeVertexData EastNeighbor;
	int32 EastNeighborDistance;

	FLandscapeVertexData WestNeighbor;
	int32 WestNeighborDistance;


	bool bFoundNorthNeighbor = FindNearestVertexNeighborData(ECardinalDirection::North, InVertex, NorthNeighbor, NorthNeighborDistance, InLandscapeProperties, InProcMeshData, InOutWorldLandscapeVertices);

	bool bFoundSouthNeighbor = FindNearestVertexNeighborData(ECardinalDirection::South, InVertex, SouthNeighbor, SouthNeighborDistance, InLandscapeProperties, InProcMeshData, InOutWorldLandscapeVertices);

	bool bFoundWestNeighbor = FindNearestVertexNeighborData(ECardinalDirection::West, InVertex, WestNeighbor, WestNeighborDistance, InLandscapeProperties, InProcMeshData, InOutWorldLandscapeVertices);

	bool bFoundEastNeighbor = FindNearestVertexNeighborData(ECardinalDirection::East, InVertex, EastNeighbor, EastNeighborDistance, InLandscapeProperties, InProcMeshData, InOutWorldLandscapeVertices);

	TArray<FLandscapeVertexData> NeighborDataArray;

	if (bFoundNorthNeighbor)
	{
		NeighborDataArray.Add(NorthNeighbor);
	}

	if (bFoundSouthNeighbor)
	{
		NeighborDataArray.Add(SouthNeighbor);
	}

	if (bFoundWestNeighbor)
	{
		NeighborDataArray.Add(WestNeighbor);
	}

	if (bFoundEastNeighbor)
	{
		NeighborDataArray.Add(EastNeighbor);
	}

	return NeighborDataArray;
}

TArray<FVector2D> AJetLandscapeMesh::CreateLandscapeUVArray(int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation, const FVector& InLocation)
{
	int32 VertexNum = (InLandscapeSize + 1) * (InLandscapeSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector2D> OutUVArray;

	int32 i = 0;

	int32 XDim = InLandscapeSize + 1;
	int32 YDim = InLandscapeSize + 1;

	float fMod = ((float)InLandscapeSize * InTileSize) / 2.0f;

	FVector2D ModVector = FVector2D(((InLocation + fMod) / 2.0f) / InTileSize);

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			OutUVArray.Add(ModVector + FVector2D(x, y));
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

	for (y = 0; y < InLandscapeSize; y++)
	{
		for (x = 0; x < InLandscapeSize; x++)
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