// Fill out your copyright notice in the Description page of Project Settings.


#include "JetLandscapeMesh.h"

AJetLandscapeMesh::AJetLandscapeMesh()
{
	//
}

void AJetLandscapeMesh::CreateLandscape(int32 InSize)
{
	Vertices = CreateLandscapeVertexArray(InSize);

	UVs = CreateLandscapeUVArray(InSize);

	Triangles = CreateLandscapeTriangleArray(InSize);
}

TArray<FVector> AJetLandscapeMesh::CreateLandscapeVertexArray(const int32 InSize)
{
	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector> OutVertexArray;

	for (int32 i = 0; i < VertexNum; i++)
	{
		//Height variation adds a degree of randomness to the landscape geometry
		int32 RandInt = FMath::RandRange(-HeightVariation, HeightVariation);

		OutVertexArray.Add(FVector(x * TileSize, y * TileSize, RandInt));

		x++;

		if (x >= InSize + 1)
		{
			x = 0;
			y++;
		}
	}

	return OutVertexArray;
}

TArray<FVector2D> AJetLandscapeMesh::CreateLandscapeUVArray(const int32 InSize)
{
	int32 VertexNum = (InSize + 1) * (InSize + 1);

	int32 x = 0;
	int32 y = 0;

	TArray<FVector2D> OutUVArray;

	for (int32 i = 0; i < VertexNum; i++)
	{
		OutUVArray.Add(FVector2D(x, y));

		x++;

		if (x >= InSize + 1)
		{
			x = 0;
			y++;
		}
	}

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

	for (int32 i = 0; i < VertexNum; i++)
	{
		int32 iMod = i % (InSize + 1);

		//we don't have to spawn a tile on the last vertex of the row, so we just skip it and move on to the next row
		//if the size of the landscape is odd, we reverse the triangles for the first tile of the next row
		if (iMod >= InSize)
		{
			if (!bEven && !bReversed)
			{
				bReverseTriangles = !bReverseTriangles;
				bReversed = true;
			}

			continue;
		}

		bReversed = false;

		TArray<int32> CurrentTriangles;

		if (!bReverseTriangles)
		{
			/*
			*	|/
			*/
			CurrentTriangles.Add(i);
			CurrentTriangles.Add(i + InSize + 1);
			CurrentTriangles.Add(i + InSize + 2);

			/*
			*	/|
			*/
			CurrentTriangles.Add(i);
			CurrentTriangles.Add(i + InSize + 2);
			CurrentTriangles.Add(i + 1);

		}
		else
		{
			/*
			*	|\
			*/
			CurrentTriangles.Add(i);
			CurrentTriangles.Add(i + InSize + 1);
			CurrentTriangles.Add(i + 1);

			/*
			*	\|
			*/
			CurrentTriangles.Add(i + InSize + 2);
			CurrentTriangles.Add(i + 1);
			CurrentTriangles.Add(i + InSize + 1);

		}

		bReverseTriangles = !bReverseTriangles;

		OutTriangleArray.Append(CurrentTriangles);
	}

	return OutTriangleArray;
}

void AJetLandscapeMesh::BeginPlay()
{
	TArray<FVector> FeatureArray;

	FeatureArray.Add(FVector(0, 0, 0));
	FeatureArray.Add(FVector(0, 1, 0));
	FeatureArray.Add(FVector(1, 0, 0));
	FeatureArray.Add(FVector(1, 1, 0));

	CreateLandscape(LandscapeSize);

	AddLandscapeFeature(FVector2D(0, 0), FeatureArray);

	AddLandscapeFeature(FVector2D(2, 2), FeatureArray);

	Super::BeginPlay();
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
	int32 OutVertexIndex = InVertexLocation.X + (InVertexLocation.Y * (InSize + 1));


	return OutVertexIndex;
}
