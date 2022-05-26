// Fill out your copyright notice in the Description page of Project Settings.


#include "JetProceduralMesh.h"
PRAGMA_DISABLE_OPTIMIZATION
AJetProceduralMesh::AJetProceduralMesh()
{
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = ProcMesh;

	//Default values

	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(100, 0, 0));
	Vertices.Add(FVector(0, 100, 0));
	Vertices.Add(FVector(100, 100, 0));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(0, 1));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));

	//Triangle1
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	//Triangle2
	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(3);

}

void AJetProceduralMesh::CreateMesh()
{

	ProcMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	for (UMaterialInterface* Mat : Materials)
	{
		int32 i = 0;

		ProcMesh->SetMaterial(i, Mat);

		i++;
	}
}



void AJetProceduralMesh::CreateLandscape(int32 InSize)
{
	Vertices = CreateLandscapeVertexArray(InSize);

	UVs = CreateLandscapeUVArray(InSize);

	Triangles = CreateLandscapeTriangleArray(InSize);
}

TArray<FVector> AJetProceduralMesh::CreateLandscapeVertexArray(const int32 InSize)
{
	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector> OutVertexArray;

	for (int32 i = 0; i < VertexNum; i++)
	{
		OutVertexArray.Add(FVector(x * 100, y * 100, 0));

		x++;
		
		if (x >= InSize+1)
		{
			x = 0;
			y++;
		}
	}

	return OutVertexArray;
}

TArray<FVector2D> AJetProceduralMesh::CreateLandscapeUVArray(const int32 InSize)
{
	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector2D> OutUVArray;

	for (int32 i = 0; i < VertexNum; i++)
	{
		OutUVArray.Add(FVector2D(x, y));

		x++;

		if (x >= InSize)
		{
			x = 0;
			y++;
		}
	}

	return OutUVArray;
}

TArray<int32> AJetProceduralMesh::CreateLandscapeTriangleArray(const int32 InSize)
{

	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<int32> OutTriangleArray;

	bool bReverseTriangles = false;

	bool bEven = (InSize % 2) == 0;

	for (int32 i = 0; i < VertexNum; i++)
	{
		if (i % InSize >= InSize - 1)
		{
			if (!bEven)
			{
				bReverseTriangles = !bReverseTriangles;
			}

			continue;
		}

		TArray<int32> CurrentTriangles;

		if (!bReverseTriangles)
		{
			CurrentTriangles.Add(i);
			CurrentTriangles.Add(i + InSize + 1);
			CurrentTriangles.Add(i + InSize);

			CurrentTriangles.Add(i);
			CurrentTriangles.Add(i + 1);
			CurrentTriangles.Add(i + InSize + 1);
		}
		else
		{
			CurrentTriangles.Add(i);
			CurrentTriangles.Add(i + 1);
			CurrentTriangles.Add(i + InSize);

			CurrentTriangles.Add(i + InSize + 1);
			CurrentTriangles.Add(i + InSize);
			CurrentTriangles.Add(i + 1);
		}

		bReverseTriangles = !bReverseTriangles;

		OutTriangleArray.Append(CurrentTriangles);
	}

	return OutTriangleArray;
}

// Called when the game starts or when spawned
void AJetProceduralMesh::BeginPlay()
{
	Super::BeginPlay();

	CreateLandscape(LandscapeSize);
	CreateMesh();
}

// Called every frame
void AJetProceduralMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
PRAGMA_ENABLE_OPTIMIZATION
