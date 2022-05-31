// Fill out your copyright notice in the Description page of Project Settings.


#include "JetLandscapeMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

namespace LandscapeVars
{
	FVector North = FVector(1, 0, 0);
	FVector Northeast = FVector(1, 1, 0);
	FVector Northwest = FVector(1, -1, 0);
	FVector West = FVector(0, -1, 0);
	FVector East = FVector(0, 1, 0);
	FVector South = FVector(-1, 0, 0);
	FVector Southwest = FVector(-1, 1, 0);
	FVector Southeast = FVector(-1, -1, 0);
}

PRAGMA_DISABLE_OPTIMIZATION
AJetLandscapeMesh::AJetLandscapeMesh()
{
	//
	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->SetupAttachment(RootComponent);

	SphereCollider->SetSphereRadius(100.0f);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionObjectType(ECollisionChannel::ECC_EngineTraceChannel2);

	SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	SphereCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	SphereCollider->SetVisibility(true);
	SphereCollider->SetHiddenInGame(false);
}

void AJetLandscapeMesh::CreateLandscape(int32 InSize)
{
	Vertices.Empty();
	UVs.Empty();
	Triangles.Empty();
	Vertices = CreateLandscapeVertexArray(InSize);

	UVs = CreateLandscapeUVArray(InSize);

	Triangles = CreateLandscapeTriangleArray(InSize);
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

	FVector TraceStart = NeighborLocation - FVector(0,0,200.0f);

	FVector TraceEnd = NeighborLocation + FVector(0,0,200.0f);

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

void AJetLandscapeMesh::SpawnNeighborLandscapes()
{
	if (!bSpawnNeighborLandscapes)
	{
		return;
	}
	FVector CurrentLocation = GetActorLocation();

	FVector NeighborLocation = CurrentLocation + (FVector(0, -1, 0) * LandscapeSize * TileSize);

	FTransform NeighborTransform = GetActorTransform();

	NeighborTransform.SetLocation(NeighborLocation);

	FActorSpawnParameters ActorSpawnParams = FActorSpawnParameters();
	UClass* LandscapeClass = AJetLandscapeMesh::StaticClass();

	AJetLandscapeMesh* SpawnedActor = GetWorld()->SpawnActorDeferred<AJetLandscapeMesh>(LandscapeClass, NeighborTransform);

	SpawnedActor->bAutoCreateLandscape = false;
	SpawnedActor->LandscapeSize = LandscapeSize;
	SpawnedActor->TileSize = TileSize;
	SpawnedActor->HeightVariation = HeightVariation;

	SpawnedActor->CreateLandscape(SpawnedActor->LandscapeSize);

	for (int32 i = 0; i < LandscapeSize + 1; i++)
	{
		int32 SpawnedIndex = SpawnedActor->GetVertexIndex(FVector2D(i, LandscapeSize), LandscapeSize);

		int32 ThisIndex = GetVertexIndex(FVector2D(i, 0), LandscapeSize);

		SpawnedActor->Vertices[SpawnedIndex].Z = Vertices[ThisIndex].Z;
	}


	UGameplayStatics::FinishSpawningActor(SpawnedActor, NeighborTransform);

	//AJetLandscapeMesh* Neighbor = GetNeighborLandscape(ECardinalDirection::West);

	//if (Neighbor)
	//{
	//	//we did it!

	//	int32 WeDidit = 69;
	//}

}

void AJetLandscapeMesh::SpawnNeighborLandscape(ECardinalDirection InNeighborDirection)
{
	if (!bSpawnNeighborLandscapes)
	{
		return;
	}

	FVector CurrentLocation = GetActorLocation();

	FTransform NeighborTransform = GetNeighborLandscapeSpawnTransform(InNeighborDirection);

	NeighborTransform.SetLocation(NeighborTransform.GetLocation() + CurrentLocation);

	FActorSpawnParameters ActorSpawnParams = FActorSpawnParameters();
	UClass* LandscapeClass = AJetLandscapeMesh::StaticClass();

	//Spawn the landscape deferred
	AJetLandscapeMesh* SpawnedActor = GetWorld()->SpawnActorDeferred<AJetLandscapeMesh>(LandscapeClass, NeighborTransform);

	if (!SpawnedActor)
	{
		return;
	}

	SpawnedActor->bAutoCreateLandscape = false;
	SpawnedActor->LandscapeSize = LandscapeSize;
	SpawnedActor->TileSize = TileSize;
	SpawnedActor->HeightVariation = HeightVariation;

	UGameplayStatics::FinishSpawningActor(SpawnedActor, NeighborTransform);

	SpawnedActor->CreateLandscape(SpawnedActor->LandscapeSize);

	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	for (dir = 0; dir < MaxDirection; dir++)
	{
		ECardinalDirection CardDir = (ECardinalDirection)dir;
		AJetLandscapeMesh* CurrentNeighbor = SpawnedActor->GetNeighborLandscape(CardDir);

		if (CurrentNeighbor)
		{
			ZipNeighborLandscapes(SpawnedActor, CurrentNeighbor);
		}
	}

	SpawnedActor->CreateMesh();

	////"zip" the landscape up with its neighbors
	//for (int32 i = 0; i < LandscapeSize + 1; i++)
	//{
	//	int32 SpawnedIndex = SpawnedActor->GetVertexIndex(FVector2D(i, LandscapeSize), LandscapeSize);

	//	int32 ThisIndex = GetVertexIndex(FVector2D(i, 0), LandscapeSize);

	//	SpawnedActor->Vertices[SpawnedIndex].Z = Vertices[ThisIndex].Z;
	//}

	//finish spawning, trigger BeginPlay() on the new landscape
	
}

void AJetLandscapeMesh::ZipNeighborLandscapes(AJetLandscapeMesh* InLandscapeOne, AJetLandscapeMesh* InLandscapeTwo)
{

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

ECardinalDirection AJetLandscapeMesh::GetLandscapeCardinality(AJetLandscapeMesh* InLandscapeOne, AJetLandscapeMesh* InLandscapeTwo)
{
	FVector PosOne = InLandscapeOne->GetActorLocation();
	FVector PosTwo = InLandscapeTwo->GetActorLocation();

	FVector Dir = PosTwo - PosOne;

	Dir.Z = 0;

	FVector Normal = Dir.GetSafeNormal();

	int32 MaxDirection = (int32)ECardinalDirection::Northwest;
	int32 dir = 0;

	for (dir = 0; dir < MaxDirection; dir++)
	{
		ECardinalDirection CardDir = (ECardinalDirection)dir;

		FTransform* TransPtr = NeighborSpawnTransformMap.Find(CardDir);

		if (TransPtr)
		{
			FVector CardDirVector = TransPtr->GetLocation().GetSafeNormal();

			if (CardDirVector.Equals(Normal, 1.0f))
			{
				return CardDir;
			}
		}
	}

	return ECardinalDirection::None;
}

TArray<FVector> AJetLandscapeMesh::CreateLandscapeVertexArray(const int32 InSize)
{
	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector> OutVertexArray;

	int32 XDim = InSize + 1;
	int32 YDim = InSize + 1;

	int32 i = 0;

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			int32 RandInt = FMath::RandRange(-HeightVariation, HeightVariation);

			OutVertexArray.Add(FVector(x * TileSize, y * TileSize, RandInt));

			VertexIndexMap.Add(FVector2D(x, y), i);

			i++;
		}
	}

	//for (i = 0; i < VertexNum; i++)
	//{
	//	//Height variation adds a degree of randomness to the landscape geometry
	//	int32 RandInt = FMath::RandRange(-HeightVariation, HeightVariation);

	//	OutVertexArray.Add(FVector(x * TileSize, y * TileSize, RandInt));

	//	x++;

	//	if (x >= InSize + 1)
	//	{
	//		x = 0;
	//		y++;
	//	}
	//}

	return OutVertexArray;
}

TArray<FVector2D> AJetLandscapeMesh::CreateLandscapeUVArray(const int32 InSize)
{
	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector2D> OutUVArray;

	int32 i = 0;

	int32 XDim = InSize + 1;
	int32 YDim = InSize + 1;

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			OutUVArray.Add(FVector2D(x, y));
		}
	}

	//for ( i < VertexNum; i++)
	//{
	//	OutUVArray.Add(FVector2D(x, y));

	//	x++;

	//	if (x >= InSize + 1)
	//	{
	//		x = 0;
	//		y++;
	//	}
	//}

	return OutUVArray;
}

TArray<int32> AJetLandscapeMesh::CreateLandscapeTriangleArray(const int32 InSize)
{

	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<int32> OutTriangleArray;

	bool bReverseTriangles = false;

	bool bReversed = false;

	bool bEven = (InSize % 2) == 0;

	for (y = 0; y < InSize + 1; y++)
	{
		for (x = 0; x < InSize + 1; x++)
		{
			TArray<int32> CurrentTriangles;

			if (!bReverseTriangles)
			{
				int32 IndexOne = GetVertexIndex(FVector2D(x, y), InSize);
				int32 IndexTwo = GetVertexIndex(FVector2D(x, y + 1), InSize);
				int32 IndexThree = GetVertexIndex(FVector2D(x + 1, y + 1), InSize);

				CurrentTriangles.Add(IndexOne);
				CurrentTriangles.Add(IndexTwo);
				CurrentTriangles.Add(IndexThree);

				int32 IndexFour = GetVertexIndex(FVector2D(x, y), InSize);
				int32 IndexFive = GetVertexIndex(FVector2D(x + 1, y + 1), InSize);
				int32 IndexSix = GetVertexIndex(FVector2D(x + 1, y), InSize);

				CurrentTriangles.Add(IndexFour);
				CurrentTriangles.Add(IndexFive);
				CurrentTriangles.Add(IndexSix);
			}
			else
			{
				/*
				*	|\
				*/


				int32 IndexOne = GetVertexIndex(FVector2D(x, y), InSize);
				int32 IndexTwo = GetVertexIndex(FVector2D(x, y + 1), InSize);
				int32 IndexThree = GetVertexIndex(FVector2D(x + 1, y), InSize);

				CurrentTriangles.Add(IndexOne);
				CurrentTriangles.Add(IndexTwo);
				CurrentTriangles.Add(IndexThree);

				int32 IndexFour = GetVertexIndex(FVector2D(x+1, y+1), InSize);
				int32 IndexFive = GetVertexIndex(FVector2D(x + 1, y), InSize);
				int32 IndexSix = GetVertexIndex(FVector2D(x, y + 1), InSize);

				CurrentTriangles.Add(IndexFour);
				CurrentTriangles.Add(IndexFive);
				CurrentTriangles.Add(IndexSix);

				//old

				//CurrentTriangles.Add(i);
				//CurrentTriangles.Add(i + InSize + 1);
				//CurrentTriangles.Add(i + 1);

				///*
				//*	\|
				//*/
				//CurrentTriangles.Add(i + InSize + 2);
				//CurrentTriangles.Add(i + 1);
				//CurrentTriangles.Add(i + InSize + 1);

			}

			bReverseTriangles = !bReverseTriangles;

			OutTriangleArray.Append(CurrentTriangles);
		}

		if (!bEven)
		{
			bReverseTriangles = !bReverseTriangles;
		}
	}
	

	//for (int32 i = 0; i < VertexNum; i++)
	//{
	//	int32 iMod = i % (InSize + 1);

	//	//we don't have to spawn a tile on the last vertex of the row, so we just skip it and move on to the next row
	//	//if the size of the landscape is odd, we reverse the triangles for the first tile of the next row
	//	if (iMod >= InSize)
	//	{
	//		if (!bEven && !bReversed)
	//		{
	//			bReverseTriangles = !bReverseTriangles;
	//			bReversed = true;
	//		}

	//		continue;
	//	}

	//	bReversed = false;

	//	TArray<int32> CurrentTriangles;

	//	if (!bReverseTriangles)
	//	{
	//		/*
	//		*	|/
	//		*/
	//		CurrentTriangles.Add(i);
	//		CurrentTriangles.Add(i + InSize + 1);
	//		CurrentTriangles.Add(i + InSize + 2);

	//		/*
	//		*	/|
	//		*/
	//		CurrentTriangles.Add(i);
	//		CurrentTriangles.Add(i + InSize + 2);
	//		CurrentTriangles.Add(i + 1);

	//	}
	//	else
	//	{
	//		/*
	//		*	|\
	//		*/
	//		CurrentTriangles.Add(i);
	//		CurrentTriangles.Add(i + InSize + 1);
	//		CurrentTriangles.Add(i + 1);

	//		/*
	//		*	\|
	//		*/
	//		CurrentTriangles.Add(i + InSize + 2);
	//		CurrentTriangles.Add(i + 1);
	//		CurrentTriangles.Add(i + InSize + 1);

	//	}

	//	bReverseTriangles = !bReverseTriangles;

	//	OutTriangleArray.Append(CurrentTriangles);
	//}

	return OutTriangleArray;
}

void AJetLandscapeMesh::BeginPlay()
{
	int32 VectorScale = TileSize * LandscapeSize;

	NeighborSpawnTransformMap.Add(ECardinalDirection::North, FTransform(LandscapeVars::North * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Northeast, FTransform(LandscapeVars::Northeast * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Northwest, FTransform(LandscapeVars::Northwest * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::West, FTransform(LandscapeVars::West * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::East, FTransform(LandscapeVars::East * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Southeast, FTransform(LandscapeVars::Southeast * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::South, FTransform(LandscapeVars::South * VectorScale));
	NeighborSpawnTransformMap.Add(ECardinalDirection::Southwest, FTransform(LandscapeVars::Southwest * VectorScale));

	SphereCollider->SetSphereRadius(100.0f);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionObjectType(ECC_GameTraceChannel2);

	//SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	SphereCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	SphereCollider->SetVisibility(true);
	SphereCollider->SetHiddenInGame(false);

	TArray<FVector> FeatureArray;

	FeatureArray.Add(FVector(0, 0, 0));
	FeatureArray.Add(FVector(0, 1, 0));
	FeatureArray.Add(FVector(1, 0, 0));
	FeatureArray.Add(FVector(1, 1, 0));

	if (bAutoCreateLandscape)
	{
		CreateLandscape(LandscapeSize);

		AddLandscapeFeature(FVector2D(0, 0), FeatureArray);

		AddLandscapeFeature(FVector2D(2, 2), FeatureArray);

		CreateMesh();
	}

	Super::BeginPlay();

	if (bSpawnNeighborLandscapes)
	{
		SpawnNeighborLandscape(ECardinalDirection::West);
	}

	//SpawnNeighborLandscapes();
}

void AJetLandscapeMesh::AddLandscapeFeature(const FVector2D InFeatureLocation, TArray<FVector> InFeatureVertexArray)
{
	for (FVector Vertex : InFeatureVertexArray)
	{
		FVector2D Vertex2D = FVector2D(Vertex.X, Vertex.Y);

		int32 VertexIndex = GetVertexIndex(InFeatureLocation + Vertex2D, LandscapeSize);

		Vertices[VertexIndex].Z = Vertex.Z;
	}
}

int32 AJetLandscapeMesh::GetVertexIndex(const FVector2D InVertexLocation, const int32 InSize)
{
	FVector2D AdjVertexLocation = InVertexLocation;

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

	int32* IndexPtr = VertexIndexMap.Find(AdjVertexLocation);

	if (IndexPtr)
	{
		return *IndexPtr;
	}
	
	return -1;

	/*int32 OutVertexIndex = InVertexLocation.X + (InVertexLocation.Y * (InSize + 1));


	return OutVertexIndex;*/
}
PRAGMA_ENABLE_OPTIMIZATION