// Fill out your copyright notice in the Description page of Project Settings.


#include "JetLandscapeMesh.h"
#include "Kismet/GameplayStatics.h"
#include "../JetBot.h"
#include "JetGameState.h"
#include "Components/SphereComponent.h"

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

	SpawnNeighborLandscapesInRadius();
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

	TArray<AJetLandscapeMesh*> OldNeighbors = GetAllNeighborLandscapes();

	TArray<AJetLandscapeMesh*> NewNeighbors = NewLandscape->GetAllNeighborLandscapes();

	bHasSpawnedNeighborLandscapes = false;

	OldNeighbors.Remove(NewLandscape);

	for (AJetLandscapeMesh* Land : NewNeighbors)
	{
		OldNeighbors.Remove(Land);
	}

	for (AJetLandscapeMesh* OldNeighbor : OldNeighbors)
	{
		if (OldNeighbor)
		{
			OldNeighbor->Destroy();
		}
	}
}

FProcMeshData AJetLandscapeMesh::CreateLandscapeData(const FTransform& InSpawnTransform, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation)
{
	FProcMeshData OutProcMeshData;

	FProcMeshFaceVertexMap LandscapeVertexMap;
	OutProcMeshData.SpawnTransform = InSpawnTransform;
	OutProcMeshData.Vertices = CreateLandscapeVertexArray(InLandscapeSize, InTileSize, InHeightVariation, LandscapeVertexMap);
	OutProcMeshData.FaceVertexMapArray.Add(LandscapeVertexMap);

	OutProcMeshData.UVs = CreateLandscapeUVArray(InLandscapeSize, InTileSize, InHeightVariation);

	OutProcMeshData.Triangles = CreateLandscapeTriangleArray(LandscapeVertexMap.VertexIndexMap, InLandscapeSize, InTileSize, InHeightVariation);

	return OutProcMeshData;
}

void AJetLandscapeMesh::ZipLandscapeDataWithNeighbors(UObject* WorldContextObject, FProcMeshData& InOutLandscapeData, const FLandscapeProperties& InLandscapeProperties)
{
	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	for (dir = 0; dir < MaxDirection + 1; dir++)
	{
		ECardinalDirection CardDir = (ECardinalDirection)dir;

		ZipLandscapeDataWithNeighbor(WorldContextObject, CardDir, InOutLandscapeData, InLandscapeProperties);
	}
}

void AJetLandscapeMesh::ZipLandscapeDataWithNeighbor(UObject* WorldContextObject, ECardinalDirection InNeighborDirection, FProcMeshData& InOutLandscapeData, const FLandscapeProperties& InLandscapeProperties)
{
	FVector2D ZippeeVector;
	FVector2D ZipperVector;

	bool bCorner = false;

	FProcMeshData NeighborData;
	bool bGotNeighborData = GetNeighborLandscapeData(WorldContextObject, InOutLandscapeData, InNeighborDirection, NeighborData, InLandscapeProperties.GetVectorScale());

	/*if (InNeighborDirection == ECardinalDirection::West)
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
	else if (InNeighborDirection == ECardinalDirection::East)
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
	else if (InNeighborDirection == ECardinalDirection::North)
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
	else if (InNeighborDirection == ECardinalDirection::South)
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
	else if (InNeighborDirection == ECardinalDirection::Northwest)
	{
		int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(0, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

		int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(LandscapeSize, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

		FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

		InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}
	else if (InNeighborDirection == ECardinalDirection::Northeast)
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
	else if (InNeighborDirection == ECardinalDirection::Southeast)
	{
		int32 ZippeeIndex = InZippee->GetVertexIndex(InZippee->GetLandscapeVertexMap(), FVector(LandscapeSize, 0, 0), FVector(LandscapeSize, LandscapeSize, 0));

		int32 ZipperIndex = InZipper->GetVertexIndex(InZipper->GetLandscapeVertexMap(), FVector(0, LandscapeSize, 0), FVector(LandscapeSize, LandscapeSize, 0));

		FVector WorldZipperLoc = InZipper->GetActorTransform().TransformPosition(InZipper->ProcMeshData.Vertices[ZipperIndex]);

		FVector ZippeeRelative = InZippee->GetActorTransform().InverseTransformPosition(WorldZipperLoc);

		InZippee->ProcMeshData.Vertices[ZippeeIndex].Z = ZippeeRelative.Z;
	}*/
}

AJetLandscapeMesh* AJetLandscapeMesh::GetNeighborLandscape_Implementation(ECardinalDirection InNeighborDirection)
{
	FTransform* TransPtr = LandscapeStatics::NeighborSpawnTransformMap.Find(InNeighborDirection);

	if (!TransPtr)
	{
		return nullptr;
	}

	const FTransform& TransRef = *TransPtr;
	FHitResult HitResult;

	const FVector& NeighborLocation = GetActorLocation() + TransRef.GetLocation();

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

bool AJetLandscapeMesh::GetNeighborLandscapeData(UObject* WorldContextObject, const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutProcMeshData, int32 InVectorScale)
{
	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState)
	{
		return false;
	}

	return GameState->GameState_GetNeighborLandscapeData(InLandscapeData, InNeighborDirection, OutProcMeshData, InVectorScale);

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

void AJetLandscapeMesh::SpawnNeighborLandscapesInRadius()
{
	if (bHasSpawnedNeighborLandscapes)
	{
		return;
	}

	int32 i = 0;

	TArray<FProcMeshData> NeighborLandscapeDatas;
	TArray<FProcMeshData> NewNeighborLandscapeDatas;

	NeighborLandscapeDatas.Add(ProcMeshData);

	for (i = 0; i < NeighborSpawnRadius; i++)
	{
		for (const FProcMeshData& n : NeighborLandscapeDatas)
		{
			int32 MaxDirection = (int32)ECardinalDirection::Northwest;
			int32 dir = 0;

			for (dir = 0; dir < MaxDirection + 1; dir++)
			{
				ECardinalDirection CardDir = (ECardinalDirection)dir;

				//AJetLandscapeMesh* CurrentNeighbor = n->GetNeighborLandscape(CardDir);

				AJetLandscapeMesh* CurrentNeighbor = nullptr;

				//TODO: Don't spawn the tiles here. Check the tiles existence using the GetNeighborLandscapeData, add those to an array, and spawn all of them at the end using their data

				FProcMeshData NewNeighborLandscape;
				bool bCreatedNewNeighbor = false;
				bCreatedNewNeighbor = CreateNewNeighborLandscapeData(this, n, CardDir, NewNeighborLandscape, LandscapeProperties);
					//CurrentNeighbor = n->SpawnNeighborLandscape(CardDir);

				if (bCreatedNewNeighbor)
				{
					/*if (!NewNeighborLandscapeDatas.Contains(n))
					{*/
						NewNeighborLandscapeDatas.Add(n);
					/*}*/
				}
			}
		}

		NeighborLandscapeDatas.Empty();
		NeighborLandscapeDatas = NewNeighborLandscapeDatas;
		NewNeighborLandscapeDatas.Empty();
	}


	// Spawn all of the new neighbors

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
				AJetLandscapeMesh* NeighborLandscape = CurrentNeighbor->GetNeighborLandscape((ECardinalDirection)dir);

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
	AJetLandscapeMesh* NeighborLandscape = GetNeighborLandscape(InNeighborDirection);

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

	if (GameState)
	{
		FProcMeshData* DataPtr = GameState->LandscapeDataMap.Find(NeighborTransform.GetLocation() * FVector(1,1,0));

		if (DataPtr)
		{
			return SpawnLandscapeWithData(this, *DataPtr, LandscapeSize, TileSize, HeightVariation, NeighborSpawnRadius);
		}
	}

	//Create a new landscape and spawn it
	FProcMeshData NewLandscapeData = CreateLandscapeData(NeighborTransform, LandscapeSize, TileSize, HeightVariation);

	return SpawnNeighboringLandscapeWithData(NewLandscapeData);
	//ZipLandscapeDataWithNeighbors(NewLandscapeData);
}

bool AJetLandscapeMesh::CreateNewNeighborLandscapeData(UObject* WorldContextObject, const FProcMeshData& InLandscape, ECardinalDirection InNeighborDirection, FProcMeshData& OutProcMeshData, const FLandscapeProperties& InLandscapeProperties)
{
	//if the neighbor landscape already exists, we don't need to spawn it, return
	bool bFoundNeighbor = GetNeighborLandscapeData(WorldContextObject, InLandscape, InNeighborDirection, OutProcMeshData, InLandscapeProperties.GetVectorScale());

	if (bFoundNeighbor)
	{
		return false;
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
	FProcMeshData NewLandscapeData = CreateLandscapeData(NeighborTransform, InLandscapeProperties.LandscapeSize, InLandscapeProperties.TileSize, InLandscapeProperties.HeightVariation);
	
	ZipLandscapeDataWithNeighbors(WorldContextObject, NewLandscapeData, InLandscapeProperties);

	OutProcMeshData = NewLandscapeData;

	return true;
	//ZipLandscapeDataWithNeighbors(NewLandscapeData);
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
		AJetLandscapeMesh* CurrentNeighbor = SpawnedActor->GetNeighborLandscape(CardDir);

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

AJetLandscapeMesh* AJetLandscapeMesh::SpawnLandscapeWithData(UObject* WorldContextObject, const FProcMeshData& InProcMeshData, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation, int32 InNeighborSpawnRadius)
{
	FActorSpawnParameters ActorSpawnParams = FActorSpawnParameters();
	UClass* LandscapeClass = WorldContextObject->GetClass();

	//Spawn the landscape deferred
	AJetLandscapeMesh* SpawnedActor = WorldContextObject->GetWorld()->SpawnActorDeferred<AJetLandscapeMesh>(LandscapeClass, InProcMeshData.SpawnTransform);

	if (!SpawnedActor)
	{
		return nullptr;
	}

	SpawnedActor->bAutoCreateLandscape = false;
	SpawnedActor->LandscapeSize = InLandscapeSize;
	SpawnedActor->TileSize = InTileSize;
	SpawnedActor->HeightVariation = InHeightVariation;
	SpawnedActor->NeighborSpawnRadius = InNeighborSpawnRadius;

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

TArray<FVector> AJetLandscapeMesh::CreateLandscapeVertexArray(int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation, FProcMeshFaceVertexMap& OutLandscapeVertexMap)
{
	int32 VertexNum = (InLandscapeSize + 1) * (InLandscapeSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector> OutVertexArray;

	int32 XDim = InLandscapeSize + 1;
	int32 YDim = InLandscapeSize + 1;

	int32 i = 0;

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			int32 RandInt = FMath::RandRange(-InHeightVariation, InHeightVariation);

			int32 HeightMod = RandInt;
			if (x > 0 && y > 0)
			{
				int32 xPre = GetVertexIndex(OutLandscapeVertexMap.VertexIndexMap, FVector(x - 1, y, 0), FVector(InLandscapeSize, InLandscapeSize, 0));

				int32 yPre = GetVertexIndex(OutLandscapeVertexMap.VertexIndexMap, FVector(x, y-1, 0), FVector(InLandscapeSize, InLandscapeSize, 0));

				HeightMod = HeightMod + ((OutVertexArray[xPre].Z + OutVertexArray[yPre].Z) / 2);
			}

			OutVertexArray.Add(FVector(x * InTileSize, y * InTileSize, RandInt));

			OutLandscapeVertexMap.VertexIndexMap.Add(FVector(x, y, 0), i);
			//VertexIndexMap.Add(FVector2D(x, y), i);

			i++;
		}
	}

	return OutVertexArray;
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

	if (bAutoCreateLandscape)
	{
		LandscapeProperties.LandscapeSize = LandscapeSize;
		LandscapeProperties.TileSize = TileSize;
		LandscapeProperties.HeightVariation = HeightVariation;
		ProcMeshData = CreateLandscapeData(GetActorTransform(), LandscapeSize, TileSize, HeightVariation);

		//CreateLandscape(LandscapeSize);

		AddLandscapeFeature(FVector(0, 0, 0), FeatureArray);

		AddLandscapeFeature(FVector(2, 2, 0), FeatureArray);

		CreateMesh();

		AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

		if (GameState)
		{
			GameState->OnLandscapeSpawned(this, ProcMeshData);
		}
	}

	Super::BeginPlay();

	if (bSpawnNeighborLandscapesAtBeginPlay)
	{
		SpawnNeighborLandscapesInRadius();
	}
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