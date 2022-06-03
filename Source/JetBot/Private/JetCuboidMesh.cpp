// Fill out your copyright notice in the Description page of Project Settings.


#include "JetCuboidMesh.h"

PRAGMA_DISABLE_OPTIMIZATION
FProcMeshData AJetCuboidMesh::CreateCuboidData(const FVector& InDimensions, int32 InTileSize)
{
	FProcMeshData OutProcMeshData = FProcMeshData();

	OutProcMeshData.FaceVertexMapArray = {
		FProcMeshFaceVertexMap(),
		FProcMeshFaceVertexMap(),
		FProcMeshFaceVertexMap(),
		FProcMeshFaceVertexMap(),
		FProcMeshFaceVertexMap(),
		FProcMeshFaceVertexMap()
	};

	OutProcMeshData.Vertices = CreateCuboidVertexArray(InDimensions, InTileSize, OutProcMeshData.FaceVertexMapArray);
	OutProcMeshData.UVs = CreateCuboidUVArray(OutProcMeshData.Vertices, InDimensions, InTileSize);
	OutProcMeshData.Triangles = CreateCuboidTriangleArray(OutProcMeshData, OutProcMeshData.Vertices, InDimensions, InTileSize);

	return OutProcMeshData;
}

TArray<FVector> AJetCuboidMesh::CreateCuboidVertexArray_Old(const FVector& InDimensions, int32 InTileSize)
{
	TArray<FVector> OutVertexArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 i = 0;

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
					VertexIndexMap.Add(FVector(x, y, z), i);
					i++;
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

TArray<FVector> AJetCuboidMesh::CreateCuboidVertexArray(const FVector& InDimensions, int32 InTileSize, TArray<FProcMeshFaceVertexMap>& OutFaceVertexMapArray)
{
	TArray<FVector> OutVertexArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 i = 0;

	//Bottom
	for (y = 0; y < YDim+1; y++)
	{
		for (x = 0; x < XDim + 1;x++)
		{
			FCuboidVertexArray NewCVA = FCuboidVertexArray();

			//Bottom
			FVector NewVertex = FVector(x, y, 0);

			OutVertexArray.Add(NewVertex*InTileSize);

			int32 ib = (int32)ECuboidSide::Bottom;

			OutFaceVertexMapArray[ib].VertexIndexMap.Add(NewVertex, i);

			i++;	
		}
	}

	//Top
	for (y = 0; y < YDim + 1; y++)
	{
		for (x = 0; x < XDim + 1; x++)
		{

			FCuboidVertexArray NewCVA = FCuboidVertexArray();

			FVector NewVertex = FVector(x, y, ZDim);

			OutVertexArray.Add(NewVertex * InTileSize);

			int32 it = (int32)ECuboidSide::Top;

			OutFaceVertexMapArray[it].VertexIndexMap.Add(NewVertex, i);

			i++;
		}
	}

	//Left
	for (z = 0; z < ZDim + 1; z++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			FCuboidVertexArray NewCVA = FCuboidVertexArray();

			FVector NewVertex = FVector(x, 0, z);

			OutVertexArray.Add(NewVertex * InTileSize);

			int32 il = (int32)ECuboidSide::Left;

			OutFaceVertexMapArray[il].VertexIndexMap.Add(NewVertex, i);

			i++;
		}
	}

	//Right
	for (z = 0; z < ZDim + 1; z++)
	{
		for (x = 0; x < XDim + 1; x++)
		{

			FCuboidVertexArray NewCVA = FCuboidVertexArray();;

			FVector NewVertex = FVector(x, YDim, z);

			OutVertexArray.Add(NewVertex * InTileSize);

			int32 ir = (int32)ECuboidSide::Right;

			OutFaceVertexMapArray[ir].VertexIndexMap.Add(NewVertex, i);

			i++;
		}
	}

	//Front
	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < YDim + 1; y++)
		{
			FCuboidVertexArray NewCVA = FCuboidVertexArray();

			//Front
			FVector NewVertex = FVector(0, y, z);

			OutVertexArray.Add(NewVertex * InTileSize);

			int32 ifr = (int32)ECuboidSide::Front;

			OutFaceVertexMapArray[ifr].VertexIndexMap.Add(NewVertex, i);

			i++;

		}
	}

	//Back

	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < YDim + 1; y++)
		{
			FCuboidVertexArray NewCVA = FCuboidVertexArray();;

			FVector NewVertex = FVector(XDim, y, z);

			OutVertexArray.Add(NewVertex * InTileSize);

			int32 iba = (int32)ECuboidSide::Back;

			OutFaceVertexMapArray[iba].VertexIndexMap.Add(NewVertex, i);

			i++;
		}
	}

	return OutVertexArray;
}

TArray<FVector2D> AJetCuboidMesh::CreateCuboidUVArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<FVector2D> OutUVArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 i = 0;

	//Bottom
	for (y = 0; y < YDim + 1; y++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			OutUVArray.Add(FVector2D(x, y));

			i++;

		}
	}

	//Top
	for (y = 0; y < YDim + 1; y++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			OutUVArray.Add(FVector2D(x, y));

			i++;
		}
	}

	//Left
	for (z = 0; z < ZDim + 1; z++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			OutUVArray.Add(FVector2D(x, z));

			i++;
		}
	}

	//Right
	for (z = 0; z < ZDim + 1; z++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			OutUVArray.Add(FVector2D(x, z));

			i++;
		}
	}

	//Front
	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < YDim + 1; y++)
		{
			OutUVArray.Add(FVector2D(y, z));

			i++;
		}
	}

	//Back
	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < YDim + 1; y++)
		{
			OutUVArray.Add(FVector2D(y, z));

			i++;
		}
	}

	//OutUVArray.Append(CreateCuboidUVArray_Bottom(InVertices, InDimensions, InTileSize));

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

	int32 NumVertices = InVertices.Num();

	int32 i = 0;

	for (i = 0; i < NumVertices; i++)
	{
		const FVector& Vertex = InVertices[i];

		x = Vertex.X / InTileSize;
		y = Vertex.Y / InTileSize;
		z = Vertex.Z / InTileSize;

		if (z == 0)
		{
			OutUVArray.EmplaceAt(i, FVector2D(x, y));
		}
		else if (z >= ZDim)
		{
			//OutUVArray.EmplaceAt(i, FVector2D(x, y));
			OutUVArray.EmplaceAt(i, FVector2D(XDim-x, YDim-y));
		}
		else if (y == 0)
		{
			OutUVArray.EmplaceAt(i, FVector2D(x, z));
		}
		else if (y >= YDim)
		{
			OutUVArray.EmplaceAt(i, FVector2D(x*-1, z*-1));
		}
		else if (x == 0)
		{
			OutUVArray.EmplaceAt(i, FVector2D(y, z));
		}
		else if (x >= XDim)
		{
			OutUVArray.EmplaceAt(i, FVector2D(y*-1, z*-1));
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

TArray<FVector2D> AJetCuboidMesh::CreateCuboidUVArray_Front(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
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

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	//cuboids must have 6 faces
	if (InProcMeshData.FaceVertexMapArray.Num() != 6)
	{
		return OutTriangleArray;
	}

	OutTriangleArray.Append(CreateCuboidTriangleArray_Bottom(InProcMeshData, InVertices, InDimensions, InTileSize));

	OutTriangleArray.Append(CreateCuboidTriangleArray_Top(InProcMeshData, InVertices, InDimensions, InTileSize));

	OutTriangleArray.Append(CreateCuboidTriangleArray_Left(InProcMeshData, InVertices, InDimensions, InTileSize));

	OutTriangleArray.Append(CreateCuboidTriangleArray_Right(InProcMeshData, InVertices, InDimensions, InTileSize));

	OutTriangleArray.Append(CreateCuboidTriangleArray_Front(InProcMeshData, InVertices, InDimensions, InTileSize));

	OutTriangleArray.Append(CreateCuboidTriangleArray_Back(InProcMeshData, InVertices, InDimensions, InTileSize));


	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidFaceTriangleArray(TMap<FVector, int32> InVertexIndexMap, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize, int32 InFaceDepth)
{
	TArray<int32> OutTriangleArray;

	int32 XDim = InDimensions.X;
	int32 YDim = InDimensions.Y;
	int32 ZDim = InDimensions.Z;

	bool bSkipX = XDim == 0;

	if (bSkipX)
	{
		XDim = 1;
	}

	bool bSkipY = YDim == 0;

	if (bSkipY)
	{
		YDim = 1;
	}

	bool bSkipZ = ZDim == 0;

	if (bSkipZ)
	{
		ZDim = 1;
	}

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 i = 0;

	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < YDim; y++)
		{
			for (x = 0; x < XDim; x++)
			{
				/*TArray<int32> CurrentTriangles;

				int32 IndexOne = GetVertexIndex(VertexIndexMap, FVector(x, y, 0), InDimensions);
				int32 IndexTwo = GetCuboidVertexIndex(FVector(x + 1, y + 1, 0));
				int32 IndexThree = GetCuboidVertexIndex(FVector(x, y + 1, 0));

				CurrentTriangles.Add(IndexOne);
				CurrentTriangles.Add(IndexTwo);
				CurrentTriangles.Add(IndexThree);

				IndexOne = GetCuboidVertexIndex(FVector(x, y, 0));
				IndexTwo = GetCuboidVertexIndex(FVector(x + 1, y, 0));
				IndexThree = GetCuboidVertexIndex(FVector(x + 1, y + 1, 0));


				CurrentTriangles.Add(IndexOne);
				CurrentTriangles.Add(IndexTwo);
				CurrentTriangles.Add(IndexThree);

				OutTriangleArray.Append(CurrentTriangles);*/
			}
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Bottom(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 i = 0;

	//cuboids must have 6 faces
	if (InProcMeshData.FaceVertexMapArray.Num() != 6)
	{
		return OutTriangleArray;
	}

	//Bottom/Top
	TMap<FVector, int32> BottomVertexMap = InProcMeshData.FaceVertexMapArray[0].VertexIndexMap;

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			//Bottom
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetVertexIndex(BottomVertexMap, FVector(x, y, 0), InDimensions);
			int32 IndexTwo = GetVertexIndex(BottomVertexMap, FVector(x + 1, y + 1, 0), InDimensions);
			int32 IndexThree = GetVertexIndex(BottomVertexMap, FVector(x, y + 1, 0), InDimensions);

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			int32 IndexFour = GetVertexIndex(BottomVertexMap, FVector(x, y, 0), InDimensions);
			int32 IndexFive = GetVertexIndex(BottomVertexMap, FVector(x + 1, y, 0), InDimensions);
			int32 IndexSix = GetVertexIndex(BottomVertexMap, FVector(x + 1, y + 1, 0), InDimensions);

			CurrentTriangles.Add(IndexFour);
			CurrentTriangles.Add(IndexFive);
			CurrentTriangles.Add(IndexSix);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Left(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	//Left/Right

	TMap<FVector, int32> LeftVertexMap = InProcMeshData.FaceVertexMapArray[2].VertexIndexMap;


	for (z = 0; z < ZDim; z++)
	{
		for (x = 0; x < XDim; x++)
		{
			//Left
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetVertexIndex(LeftVertexMap, FVector(x, 0, z), InDimensions);
			int32 IndexTwo = GetVertexIndex(LeftVertexMap, FVector(x, 0, z + 1), InDimensions);
			int32 IndexThree = GetVertexIndex(LeftVertexMap, FVector(x + 1, 0, z + 1), InDimensions);

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			int32 IndexFour = GetVertexIndex(LeftVertexMap, FVector(x, 0, z), InDimensions);
			int32 IndexFive = GetVertexIndex(LeftVertexMap, FVector(x + 1, 0, z + 1), InDimensions);
			int32 IndexSix = GetVertexIndex(LeftVertexMap, FVector(x + 1, 0, z), InDimensions);

			CurrentTriangles.Add(IndexFour);
			CurrentTriangles.Add(IndexFive);
			CurrentTriangles.Add(IndexSix);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Front(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	TMap<FVector, int32> FrontVertexMap = InProcMeshData.FaceVertexMapArray[4].VertexIndexMap;

	//Front/Back
	for (z = 0; z < ZDim; z++)
	{
		for (y = 0; y < XDim; y++)
		{
			//Front
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetVertexIndex(FrontVertexMap, FVector(0, y, z), InDimensions);
			int32 IndexTwo = GetVertexIndex(FrontVertexMap, FVector(0, y + 1, z + 1), InDimensions);
			int32 IndexThree = GetVertexIndex(FrontVertexMap, FVector(0, y, z + 1), InDimensions);

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			int32 IndexFour = GetVertexIndex(FrontVertexMap, FVector(0, y, z), InDimensions);
			int32 IndexFive = GetVertexIndex(FrontVertexMap, FVector(0, y + 1, z), InDimensions);
			int32 IndexSix = GetVertexIndex(FrontVertexMap, FVector(0, y + 1, z + 1), InDimensions);

			CurrentTriangles.Add(IndexFour);
			CurrentTriangles.Add(IndexFive);
			CurrentTriangles.Add(IndexSix);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Top(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	//cuboids must have 6 faces
	if (InProcMeshData.FaceVertexMapArray.Num() != 6)
	{
		return OutTriangleArray;
	}

	//Bottom/Top
	TMap<FVector, int32> TopVertexMap = InProcMeshData.FaceVertexMapArray[1].VertexIndexMap;


	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			//top
			TArray<int32> CurrentTriangles;

			int32 IndexSeven = GetVertexIndex(TopVertexMap, FVector(x, y, ZDim), InDimensions);
			int32 IndexEight = GetVertexIndex(TopVertexMap, FVector(x, y + 1, ZDim), InDimensions);
			int32 IndexNine = GetVertexIndex(TopVertexMap, FVector(x + 1, y + 1, ZDim), InDimensions);

			CurrentTriangles.Add(IndexSeven);
			CurrentTriangles.Add(IndexEight);
			CurrentTriangles.Add(IndexNine);

			int32 IndexTen = GetVertexIndex(TopVertexMap, FVector(x, y, ZDim), InDimensions);
			int32 IndexEleven = GetVertexIndex(TopVertexMap, FVector(x + 1, y + 1, ZDim), InDimensions);
			int32 IndexTwelve = GetVertexIndex(TopVertexMap, FVector(x + 1, y, ZDim), InDimensions);

			CurrentTriangles.Add(IndexTen);
			CurrentTriangles.Add(IndexEleven);
			CurrentTriangles.Add(IndexTwelve);

			OutTriangleArray.Append(CurrentTriangles);

		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Right(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	TMap<FVector, int32> RightVertexMap = InProcMeshData.FaceVertexMapArray[3].VertexIndexMap;

	for (z = 0; z < ZDim; z++)
	{
		for (x = 0; x < XDim; x++)
		{
			TArray<int32> CurrentTriangles;

			//right
			int32 IndexSeven = GetVertexIndex(RightVertexMap, FVector(x, YDim, z), InDimensions);
			int32 IndexEight = GetVertexIndex(RightVertexMap, FVector(x + 1, YDim, z + 1), InDimensions);
			int32 IndexNine = GetVertexIndex(RightVertexMap, FVector(x, YDim, z + 1), InDimensions);

			CurrentTriangles.Add(IndexSeven);
			CurrentTriangles.Add(IndexEight);
			CurrentTriangles.Add(IndexNine);

			int32 IndexTen = GetVertexIndex(RightVertexMap, FVector(x, YDim, z), InDimensions);
			int32 IndexEleven = GetVertexIndex(RightVertexMap, FVector(x + 1, YDim, z), InDimensions);
			int32 IndexTwelve = GetVertexIndex(RightVertexMap, FVector(x + 1, YDim, z + 1), InDimensions);

			CurrentTriangles.Add(IndexTen);
			CurrentTriangles.Add(IndexEleven);
			CurrentTriangles.Add(IndexTwelve);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Back(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	TMap<FVector, int32> BackVertexMap = InProcMeshData.FaceVertexMapArray[5].VertexIndexMap;

	for (z = 0; z < ZDim; z++)
	{
		for (y = 0; y < XDim; y++)
		{
			//Back

			TArray<int32> CurrentTriangles;

			int32 IndexSeven = GetVertexIndex(BackVertexMap, FVector(XDim, y, z), InDimensions);
			int32 IndexEight = GetVertexIndex(BackVertexMap, FVector(XDim, y, z + 1), InDimensions);
			int32 IndexNine = GetVertexIndex(BackVertexMap, FVector(XDim, y + 1, z + 1), InDimensions);

			CurrentTriangles.Add(IndexSeven);
			CurrentTriangles.Add(IndexEight);
			CurrentTriangles.Add(IndexNine);

			int32 IndexTen = GetVertexIndex(BackVertexMap, FVector(XDim, y, z), InDimensions);
			int32 IndexEleven = GetVertexIndex(BackVertexMap, FVector(XDim, y + 1, z + 1), InDimensions);
			int32 IndexTwelve = GetVertexIndex(BackVertexMap, FVector(XDim, y + 1, z), InDimensions);

			CurrentTriangles.Add(IndexTen);
			CurrentTriangles.Add(IndexEleven);
			CurrentTriangles.Add(IndexTwelve);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

int32 AJetCuboidMesh::GetCuboidVertexIndex_Old(const TArray<FVector>& InVertices, const FVector& InVertexLocation, const FVector& InDimensions)
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

int32 AJetCuboidMesh::GetCuboidVertexIndex(const FVector& InVertexLocation)
{
	FVector AdjVertexLocation = InVertexLocation;

	if (AdjVertexLocation.X > Dimensions.X)
	{
		AdjVertexLocation.X = Dimensions.X;
	}
	else if (AdjVertexLocation.X < 0)
	{
		AdjVertexLocation.X = 0;
	}

	if (AdjVertexLocation.Y > Dimensions.Y)
	{
		AdjVertexLocation.Y = Dimensions.Y;
	}
	else if (AdjVertexLocation.Y < 0)
	{
		AdjVertexLocation.Y = 0;
	}

	if (AdjVertexLocation.Z > Dimensions.Z)
	{
		AdjVertexLocation.Z = Dimensions.Z;
	}
	else if (AdjVertexLocation.Z < 0)
	{
		AdjVertexLocation.Z = 0;
	}

	int32* FoundVertex = VertexIndexMap.Find(AdjVertexLocation);

	if (FoundVertex)
	{
		return *FoundVertex;
	}
	else
	{
		return -1;
	}
}

int32 AJetCuboidMesh::GetCuboidVertexSideIndex(const FCuboidVertexArray& InCuboidVertexArray, ECuboidSide InCuboidSide)
{
	if (InCuboidVertexArray.VertexArray.Num() == 0)
	{
		return -1;
	}

	int32 i = (int32)InCuboidSide;

	if (InCuboidVertexArray.VertexArray.IsValidIndex(i))
	{
		return InCuboidVertexArray.VertexArray[i];
	}

	/*return 

	switch (InCuboidSide)
	{
	case ECuboidSide::Bottom:
	{
		return InCuboidVertexArray.VertexArray[0];
	}
	case ECuboidSide::Top:
	{
		return InCuboidVertexArray.VertexArray[0];
	}
	case ECuboidSide::Left:
	{
		return InCuboidVertexArray.VertexArray.IsValidIndex(1) ? InCuboidVertexArray.VertexArray[1] : InCuboidVertexArray.VertexArray[0];
	}
	case ECuboidSide::Right:
	{
		return InCuboidVertexArray.VertexArray.IsValidIndex(1) ? InCuboidVertexArray.VertexArray[1] : InCuboidVertexArray.VertexArray[0];
	}
	case ECuboidSide::Front:
	{
		if (InCuboidVertexArray.VertexArray.IsValidIndex(2))
		{
			return InCuboidVertexArray.VertexArray[2];
		}
		else if (InCuboidVertexArray.VertexArray.IsValidIndex(1))
		{
			return InCuboidVertexArray.VertexArray[1];
		}

		return InCuboidVertexArray.VertexArray[0];
	}
	case ECuboidSide::Back:
	{
		if (InCuboidVertexArray.VertexArray.IsValidIndex(2))
		{
			return InCuboidVertexArray.VertexArray[2];
		}
		else if (InCuboidVertexArray.VertexArray.IsValidIndex(1))
		{
			return InCuboidVertexArray.VertexArray[1];
		}

		return InCuboidVertexArray.VertexArray[0];
	}
	}*/

	return -1;
}

const FCuboidVertexArray& AJetCuboidMesh::GetCuboidVertexIndices(const FVector& InVertexLocation)
{
	FVector AdjVertexLocation = InVertexLocation;

	if (AdjVertexLocation.X > Dimensions.X)
	{
		AdjVertexLocation.X = Dimensions.X;
	}
	else if (AdjVertexLocation.X < 0)
	{
		AdjVertexLocation.X = 0;
	}

	if (AdjVertexLocation.Y > Dimensions.Y)
	{
		AdjVertexLocation.Y = Dimensions.Y;
	}
	else if (AdjVertexLocation.Y < 0)
	{
		AdjVertexLocation.Y = 0;
	}

	if (AdjVertexLocation.Z > Dimensions.Z)
	{
		AdjVertexLocation.Z = Dimensions.Z;
	}
	else if (AdjVertexLocation.Z < 0)
	{
		AdjVertexLocation.Z = 0;
	}

	FCuboidVertexArray* VertexPtr = VertexIndicesMap.Find(AdjVertexLocation);

	if (VertexPtr)
	{
		return *VertexPtr;
	}
	else
	{
		return EmptyVertex;
	}


}

void AJetCuboidMesh::BeginPlay()
{

	ProcMeshData = CreateCuboidData(Dimensions, TileSize);

	CreateMesh();

	Super::BeginPlay();
}
PRAGMA_ENABLE_OPTIMIZATION