// Fill out your copyright notice in the Description page of Project Settings.


#include "JetCuboidMesh.h"

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

void AJetCuboidMesh::BeginPlay()
{

	ProcMeshData = CreateCuboidData(Dimensions, TileSize);

	TArray<UMaterialInterface*> MatArray;
	CreateMesh(MatArray);

	Super::BeginPlay();
}
