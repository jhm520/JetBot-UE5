// Fill out your copyright notice in the Description page of Project Settings.


#include "JetCuboidMesh.h"

PRAGMA_DISABLE_OPTIMIZATION
void AJetCuboidMesh::CreateCuboid(const FVector& InDimensions, int32 InTileSize)
{
	Vertices = CreateCuboidVertexArray(InDimensions, InTileSize);
	UVs = CreateCuboidUVArray(Vertices, InDimensions, InTileSize);
	Triangles = CreateCuboidTriangleArray(Vertices, InDimensions, InTileSize);
}

TArray<FVector> AJetCuboidMesh::CreateCuboidVertexArray(const FVector& InDimensions, int32 InTileSize)
{
	TArray<FVector> OutVertexArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	while (z < ZDim + 1)
	{
		while (y < YDim + 1)
		{
			while (x < XDim + 1)
			{
				bool bIsValidCuboidVertex = (x == 0 || x == XDim || y == 0 || y == YDim || z == 0 || z == ZDim);

				if (bIsValidCuboidVertex)
				{
					OutVertexArray.Add(FVector(x * InTileSize, y * InTileSize, z * InTileSize));
				}

				x++;
			}

			x = 0;
			y++;
		}

		y = 0;
		z++;
	}



	return OutVertexArray;
}

TArray<FVector2D> AJetCuboidMesh::CreateCuboidUVArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<FVector2D> OutUVArray;

	OutUVArray.Append(CreateCuboidUVArray_Bottom(InVertices, InDimensions, InTileSize));

	OutUVArray.AddZeroed(InVertices.Num() - OutUVArray.Num());

	return OutUVArray;
}

TArray<FVector2D> AJetCuboidMesh::CreateCuboidUVArray_Bottom(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<FVector2D> OutUVArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	OutUVArray.AddZeroed(((int32)InDimensions.X+1) * ((int32)InDimensions.Y+1));

	int32 NumVertices = InVertices.Num();

	int32 i = 0;

	for (y = 0; y < YDim + 1; y++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			OutUVArray[i] = FVector2D(x, y);

			i++;
		}
	}

	return OutUVArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	OutTriangleArray.Append(CreateCuboidTriangleArray_Bottom(InVertices, InDimensions, InTileSize));

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Bottom(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;

	for (const FVector& Vertex : InVertices)
	{
		if (Vertex.Z > 0)
		{
			break;
		}

		int32 ixMod = i % ((int32)InDimensions.X + 1);

		int32 iyMod = i / ((int32)InDimensions.Y + 2);

		if (ixMod >= XDim || iyMod >= YDim)
		{
			i++;
			continue;
		}

		TArray<int32> CurrentTriangles;
		
		CurrentTriangles.Add(i);
		CurrentTriangles.Add(i + XDim + 2);
		CurrentTriangles.Add(i + XDim + 1);
		

		CurrentTriangles.Add(i);
		CurrentTriangles.Add(i + 1);
		CurrentTriangles.Add(i + XDim + 2);
		

		OutTriangleArray.Append(CurrentTriangles);

		i++;
	}

	return OutTriangleArray;
}

void AJetCuboidMesh::BeginPlay()
{
	CreateCuboid(Dimensions, TileSize);

	Super::BeginPlay();
}
PRAGMA_ENABLE_OPTIMIZATION