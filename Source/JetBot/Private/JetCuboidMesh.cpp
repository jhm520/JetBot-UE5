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

TArray<FVector2D> AJetCuboidMesh::CreateCuboidUVArray_Left(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<FVector2D> OutUVArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	OutUVArray.AddZeroed(((int32)InDimensions.X + 1) * ((int32)InDimensions.Y + 1));

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
	//OutTriangleArray.Append(CreateCuboidTriangleArray_Left(InVertices, InDimensions, InTileSize));

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

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Left(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	for (const FVector& Vertex : InVertices)
	{
		if (Vertex.Y > 0)
		{
			continue;
		}

		int32 ixMod = (int32)(Vertex.X / InTileSize);

		int32 iyMod = (int32)(Vertex.Y / InTileSize);

		int32 izMod = (int32)(Vertex.Z / InTileSize);

		if (ixMod >= XDim || izMod >= ZDim)
		{
			i++;
			continue;
		}

		TArray<int32> CurrentTriangles;

		int32 VertexZero = i;
		int32 VertexOne = i == 0 ? i + ((XDim + 1) * (YDim + 1)) : i + (((XDim + 1) * 2) + (YDim - 1) * 2);
		int32 VertexTwo = i + 1;
		int32 VertexThree = VertexOne + 1;

		CurrentTriangles.Add(VertexZero);
		CurrentTriangles.Add(VertexThree);
		CurrentTriangles.Add(VertexOne);

		CurrentTriangles.Add(VertexZero);
		CurrentTriangles.Add(VertexTwo);
		CurrentTriangles.Add(VertexThree);

		OutTriangleArray.Append(CurrentTriangles);

		i++;
	}

	return OutTriangleArray;
}

int32 AJetCuboidMesh::GetCuboidVertexIndex(const TArray<FVector>& InVertices, const FVector& InVertexLocation, const FVector& InDimensions)
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

	//bottom
	if (AdjVertexLocation.Z == 0)
	{
		return (AdjVertexLocation.Y * (InDimensions.X + 1)) + AdjVertexLocation.X;
	}
	//top
	else if (AdjVertexLocation.Z >= InDimensions.Z)
	{
		int32 Mod = InVertices.Num() - ((InDimensions.X + 1) * (InDimensions.Y + 1));
		return Mod + ((AdjVertexLocation.Y * (InDimensions.X + 1)) + AdjVertexLocation.X);
	}
	//left
	else if (AdjVertexLocation.Y == 0)
	{
		int32 BottomMod = ((InDimensions.Y + 1) * (InDimensions.X + 1));
		int32 PerimeterMod = ((InDimensions.X + 1) * 2) + ((InDimensions.Y - 1) * 2);

		int32 ModTwo = (AdjVertexLocation.Z - 1) * PerimeterMod;
		int32 ModThree = AdjVertexLocation.X;

		return BottomMod + ModTwo + ModThree;
	}
	//right
	else if (AdjVertexLocation.Y >= InDimensions.Y)
	{
		int32 BottomMod = ((InDimensions.Y + 1) * (InDimensions.X + 1));

		//int32 Mod = ((AdjVertexLocation.Y * (InDimensions.X + 1)) + AdjVertexLocation.X);
		int32 PerimeterMod = ((InDimensions.X + 1) * 2) + ((InDimensions.Y - 1) * 2);

		int32 ModTwo = ((AdjVertexLocation.Z) * PerimeterMod) - (InDimensions.X - 1);
		int32 ModThree = AdjVertexLocation.X;

		return BottomMod + ModTwo + ModThree;

	}
	//front
	else if (AdjVertexLocation.X == 0)
	{
		int32 BottomMod = ((InDimensions.Y + 1) * (InDimensions.X + 1));
		int32 PerimeterMod = ((InDimensions.X + 1) * 2) + ((InDimensions.Y - 1) * 2);

		int32 ModTwo = ((AdjVertexLocation.Z - 1) * PerimeterMod);

		int32 YMod = ((AdjVertexLocation.Y - 1) * 2) + (InDimensions.X + 1);


		return BottomMod + ModTwo + YMod;

	}
	else if (AdjVertexLocation.X >= (InDimensions.X))
	{
		int32 BottomMod = ((InDimensions.Y + 1) * (InDimensions.X + 1));
		int32 PerimeterMod = ((InDimensions.X + 1) * 2) + ((InDimensions.Y - 1) * 2);

		int32 ModTwo = ((AdjVertexLocation.Z - 1) * PerimeterMod);

		int32 YMod = ((AdjVertexLocation.Y - 1) * 2) + (InDimensions.X + 1);


		return BottomMod + ModTwo + YMod + 1;
	}

	return 0;
}

void AJetCuboidMesh::BeginPlay()
{
	CreateCuboid(Dimensions, TileSize);

	Super::BeginPlay();
}
PRAGMA_ENABLE_OPTIMIZATION