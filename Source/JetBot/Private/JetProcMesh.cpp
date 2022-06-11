// Fill out your copyright notice in the Description page of Project Settings.


#include "JetProcMesh.h"


AJetProcMesh::AJetProcMesh()
{
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = ProcMesh;

	//Default values

	//Vertices.Add(FVector(0, 0, 0));
	//Vertices.Add(FVector(100, 0, 0));
	//Vertices.Add(FVector(0, 100, 0));
	//Vertices.Add(FVector(100, 100, 0));

	//UVs.Add(FVector2D(0, 0));
	//UVs.Add(FVector2D(0, 1));
	//UVs.Add(FVector2D(1, 0));
	//UVs.Add(FVector2D(1, 1));

	////Triangle1
	//Triangles.Add(0);
	//Triangles.Add(1);
	//Triangles.Add(2);

	////Triangle2
	//Triangles.Add(2);
	//Triangles.Add(1);
	//Triangles.Add(3);

}

int32 AJetProcMesh::GetVertexIndex(const TMap<FVector, int32>& InVertexIndexMap, const FVector& InVertexLocation, const FVector& InDimensions)
{
	FVector AdjVertexLocation = InVertexLocation;

	if (AdjVertexLocation.X > InDimensions.X)
	{
		AdjVertexLocation.X = InDimensions.X;
	}
	else if (AdjVertexLocation.X < 0)
	{
		AdjVertexLocation.X = 0;
	}


	if (AdjVertexLocation.Y > InDimensions.Y)
	{
		AdjVertexLocation.Y = InDimensions.Y;
	}
	else if (AdjVertexLocation.Y < 0)
	{
		AdjVertexLocation.Y = 0;
	}

	if (AdjVertexLocation.Z > InDimensions.Z)
	{
		AdjVertexLocation.Z = InDimensions.Z;
	}
	else if (AdjVertexLocation.Z < 0)
	{
		AdjVertexLocation.Z = 0;
	}

	const int32* IndexPtr = InVertexIndexMap.Find(AdjVertexLocation);

	if (IndexPtr)
	{
		return *IndexPtr;
	}

	return -1;
}

void AJetProcMesh::CreateMesh()
{

	ProcMesh->CreateMeshSection(0, ProcMeshData.Vertices, ProcMeshData.Triangles, TArray<FVector>(), ProcMeshData.UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	for (UMaterialInterface* Mat : ProcMeshData.Materials)
	{
		int32 i = 0;

		ProcMesh->SetMaterial(i, Mat);

		i++;
	}
}

// Called when the game starts or when spawned
void AJetProcMesh::BeginPlay()
{
	Super::BeginPlay();

	//CreateMesh();
}

// Called every frame
void AJetProcMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

