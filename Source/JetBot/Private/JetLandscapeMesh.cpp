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

void AJetLandscapeMesh::ZipLandscapeDataWithNeighbors(AJetLandscapeMesh* InZippee, FProcMeshData& InOutLandscapeData)
{

}

AJetLandscapeMesh* AJetLandscapeMesh::GetNeighborLandscape_Implementation(ECardinalDirection InNeighborDirection)
{
	FTransform* TransPtr = NeighborSpawnTransformMap.Find(InNeighborDirection);

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

bool AJetLandscapeMesh::GetNeighborLandscapeExists(ECardinalDirection InNeighborDirection)
{
	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return false;
	}

	return GameState->GameState_GetNeighborLandscapeExists(this, InNeighborDirection);

}

void AJetLandscapeMesh::SpawnNeighborLandscapesInRadius()
{
	if (bHasSpawnedNeighborLandscapes)
	{
		return;
	}

	int32 i = 0;

	TArray<AJetLandscapeMesh*> NeighborLandscapes;
	TArray<AJetLandscapeMesh*> NewNeighbors;

	NeighborLandscapes.Add(this);

	for (i = 0; i < NeighborSpawnRadius; i++)
	{
		for (AJetLandscapeMesh* n : NeighborLandscapes)
		{
			int32 MaxDirection = (int32)ECardinalDirection::Northwest;
			int32 dir = 0;

			if (!n)
			{
				continue;
			}

			for (dir = 0; dir < MaxDirection + 1; dir++)
			{
				ECardinalDirection CardDir = (ECardinalDirection)dir;

				AJetLandscapeMesh* CurrentNeighbor = n->GetNeighborLandscape(CardDir);

				bool bHasNeighbor = n->GetNeighborLandscapeExists(CardDir);

				if (!bHasNeighbor)
				{
					CurrentNeighbor = n->SpawnNeighborLandscape(CardDir);
				}

				if (CurrentNeighbor)
				{
					if (!NewNeighbors.Contains(CurrentNeighbor))
					{
						NewNeighbors.Add(CurrentNeighbor);
					}
				}
			}
		}

		NeighborLandscapes.Empty();
		NeighborLandscapes = NewNeighbors;
		NewNeighbors.Empty();
	}

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

	FTransform NeighborTransform = GetNeighborLandscapeSpawnTransform(InNeighborDirection);
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

FTransform AJetLandscapeMesh::GetNeighborLandscapeSpawnTransform(ECardinalDirection InNeighborDirection)
{
	FTransform* TransformPtr = NeighborSpawnTransformMap.Find(InNeighborDirection);

	if (TransformPtr)
	{
		return *TransformPtr;
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

		FTransform* TransPtr = NeighborSpawnTransformMap.Find(CardDir);

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

	NeighborSpawnTransformMap.Add(ECardinalDirection::North, FTransform(LandscapeStatics::North * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Northeast, FTransform(LandscapeStatics::Northeast * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Northwest, FTransform(LandscapeStatics::Northwest * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::West, FTransform(LandscapeStatics::West * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::East, FTransform(LandscapeStatics::East * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Southeast, FTransform(LandscapeStatics::Southeast * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::South, FTransform(LandscapeStatics::South * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Southwest, FTransform(LandscapeStatics::Southwest * VectorScale));



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