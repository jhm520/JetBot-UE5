// Fill out your copyright notice in the Description page of Project Settings.


#include "JetCuboidMesh.h"

PRAGMA_DISABLE_OPTIMIZATION
void AJetCuboidMesh::CreateCuboid(const FVector& InDimensions, int32 InTileSize)
{
	Vertices = CreateCuboidVertexArray(InDimensions, InTileSize);
	UVs = CreateCuboidUVArray(Vertices, InDimensions, InTileSize);
	Triangles = CreateCuboidTriangleArray(Vertices, InDimensions, InTileSize);
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

TArray<FVector> AJetCuboidMesh::CreateCuboidVertexArray(const FVector& InDimensions, int32 InTileSize)
{
	TArray<FVector> OutVertexArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 i = 0;

	//Bottom/Top
	for (y = 0; y < YDim+1; y++)
	{
		for (x = 0; x < XDim + 1;x++)
		{
			FCuboidVertexArray NewCVA = FCuboidVertexArray();

			//Bottom
			FVector NewVertex = FVector(x, y, 0);

			OutVertexArray.Add(NewVertex*InTileSize);

			FCuboidVertexArray* CVAPtr = VertexIndicesMap.Find(NewVertex);


			if (CVAPtr)
			{
				NewCVA = *CVAPtr;
				NewCVA.VertexArray.Add(i);

				VertexIndicesMap.Add(NewVertex, NewCVA);
			}
			else
			{
				NewCVA.VertexArray.Add(i);
				VertexIndicesMap.Add(NewVertex, NewCVA);
			}

			i++;

			////Top

			//NewCVA.VertexArray.Empty();

			//NewVertex = FVector(x, y, ZDim);

			//OutVertexArray.Add(NewVertex * InTileSize);

			//CVAPtr = VertexIndicesMap.Find(NewVertex);

			//if (CVAPtr)
			//{
			//	NewCVA = *CVAPtr;
			//	NewCVA.VertexArray.Add(i);

			//	VertexIndicesMap.Add(NewVertex, NewCVA);
			//}
			//else
			//{
			//	NewCVA.VertexArray.Add(i);
			//	VertexIndicesMap.Add(NewVertex, NewCVA);
			//}

			//i++;
		}
	}

	//Left/Right
	for (z = 0; z < ZDim + 1; z++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			FCuboidVertexArray NewCVA = FCuboidVertexArray();

			//Left
			FVector NewVertex = FVector(x, 0, z);

			OutVertexArray.Add(NewVertex * InTileSize);

			FCuboidVertexArray* CVAPtr = VertexIndicesMap.Find(NewVertex);


			if (CVAPtr)
			{
				NewCVA = *CVAPtr;
				NewCVA.VertexArray.Add(i);

				VertexIndicesMap.Add(NewVertex, NewCVA);
			}
			else
			{
				NewCVA.VertexArray.Add(i);
				VertexIndicesMap.Add(NewVertex, NewCVA);
			}

			i++;

			////Right

			//NewCVA.VertexArray.Empty();

			//NewVertex = FVector(x, YDim, z);

			//OutVertexArray.Add(NewVertex * InTileSize);

			//CVAPtr = VertexIndicesMap.Find(NewVertex);

			//if (CVAPtr)
			//{
			//	NewCVA = *CVAPtr;
			//	NewCVA.VertexArray.Add(i);

			//	VertexIndicesMap.Add(NewVertex, NewCVA);
			//}
			//else
			//{
			//	NewCVA.VertexArray.Add(i);
			//	VertexIndicesMap.Add(NewVertex, NewCVA);
			//}

			//i++;
		}
	}

	//Front/Back
	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < YDim + 1; y++)
		{
			FCuboidVertexArray NewCVA = FCuboidVertexArray();

			//Front
			FVector NewVertex = FVector(0, y, z);

			OutVertexArray.Add(NewVertex * InTileSize);

			FCuboidVertexArray* CVAPtr = VertexIndicesMap.Find(NewVertex);


			if (CVAPtr)
			{
				NewCVA = *CVAPtr;
				NewCVA.VertexArray.Add(i);

				VertexIndicesMap.Add(NewVertex, NewCVA);
			}
			else
			{
				NewCVA.VertexArray.Add(i);
				VertexIndicesMap.Add(NewVertex, NewCVA);
			}

			i++;

			////Back

			//NewCVA.VertexArray.Empty();

			//NewVertex = FVector(XDim, y, z);

			//OutVertexArray.Add(NewVertex * InTileSize);

			//CVAPtr = VertexIndicesMap.Find(NewVertex);

			//if (CVAPtr)
			//{
			//	NewCVA = *CVAPtr;
			//	NewCVA.VertexArray.Add(i);

			//	VertexIndicesMap.Add(NewVertex, NewCVA);
			//}
			//else
			//{
			//	NewCVA.VertexArray.Add(i);
			//	VertexIndicesMap.Add(NewVertex, NewCVA);
			//}

			//i++;
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

	//Bottom/Top
	for (y = 0; y < YDim + 1; y++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			OutUVArray.Add(FVector2D(x, y));

			i++;

			/*OutUVArray.Add(FVector2D(x, y));

			i++;*/
		}
	}

	//Left/Right
	for (z = 0; z < ZDim + 1; z++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			OutUVArray.Add(FVector2D(x, z));

			i++;

			/*OutUVArray.Add(FVector2D(x, z));

			i++;*/
		}
	}

	//Front/Back
	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < YDim + 1; y++)
		{
			OutUVArray.Add(FVector2D(y, z));

			i++;

			/*OutUVArray.Add(FVector2D(y, z));

			i++;*/
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

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	TArray<FVector2D> OutUVArray;

	const int32 XDim = InDimensions.X;
	const int32 YDim = InDimensions.Y;
	const int32 ZDim = InDimensions.Z;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;

	int32 i = 0;

	//Bottom/Top
	for (y = 0; y < YDim + 1; y++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			//Bottom
			TArray<int32> CurrentTriangles;

			const FCuboidVertexArray& IndexOne = GetCuboidVertexIndices(FVector(x, y, 0));
			const FCuboidVertexArray& IndexTwo = GetCuboidVertexIndices(FVector(x + 1, y + 1, 0));
			const FCuboidVertexArray& IndexThree = GetCuboidVertexIndices(FVector(x, y + 1, 0));

			CurrentTriangles.Add(IndexOne.VertexArray[0]);
			CurrentTriangles.Add(IndexTwo.VertexArray[0]);
			CurrentTriangles.Add(IndexThree.VertexArray[0]);

			const FCuboidVertexArray& IndexFour = GetCuboidVertexIndices(FVector(x, y, 0));
			const FCuboidVertexArray& IndexFive = GetCuboidVertexIndices(FVector(x + 1, y, 0));
			const FCuboidVertexArray& IndexSix = GetCuboidVertexIndices(FVector(x + 1, y + 1, 0));

			CurrentTriangles.Add(IndexFour.VertexArray[0]);
			CurrentTriangles.Add(IndexFive.VertexArray[0]);
			CurrentTriangles.Add(IndexSix.VertexArray[0]);

			//top
			/*const FCuboidVertexArray& IndexSeven = GetCuboidVertexIndices(FVector(x, y, ZDim));
			const FCuboidVertexArray& IndexEight = GetCuboidVertexIndices(FVector(x, y + 1, ZDim));
			const FCuboidVertexArray& IndexNine = GetCuboidVertexIndices(FVector(x + 1, y + 1, ZDim));

			CurrentTriangles.Add(IndexSeven.VertexArray[0]);
			CurrentTriangles.Add(IndexEight.VertexArray[0]);
			CurrentTriangles.Add(IndexNine.VertexArray[0]);

			const FCuboidVertexArray& IndexTen = GetCuboidVertexIndices(FVector(x, y, ZDim));
			const FCuboidVertexArray& IndexEleven = GetCuboidVertexIndices(FVector(x + 1, y + 1, ZDim));
			const FCuboidVertexArray& IndexTwelve = GetCuboidVertexIndices(FVector(x + 1, y, ZDim));

			CurrentTriangles.Add(IndexTen.VertexArray[0]);
			CurrentTriangles.Add(IndexEleven.VertexArray[0]);
			CurrentTriangles.Add(IndexTwelve.VertexArray[0]);*/

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	//Left/Right
	for (z = 0; z < ZDim + 1; z++)
	{
		for (x = 0; x < XDim + 1; x++)
		{
			//Left
			TArray<int32> CurrentTriangles;

			const FCuboidVertexArray& IndexOne = GetCuboidVertexIndices(FVector(x, 0, z));
			const FCuboidVertexArray& IndexTwo = GetCuboidVertexIndices(FVector(x, 0, z + 1));
			const FCuboidVertexArray& IndexThree = GetCuboidVertexIndices(FVector(x + 1, 0, z + 1));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexOne, ECuboidSide::Left));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexTwo, ECuboidSide::Left));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexThree, ECuboidSide::Left));

			const FCuboidVertexArray& IndexFour = GetCuboidVertexIndices(FVector(x, 0, z));
			const FCuboidVertexArray& IndexFive = GetCuboidVertexIndices(FVector(x + 1, 0, z + 1));
			const FCuboidVertexArray& IndexSix = GetCuboidVertexIndices(FVector(x + 1, 0, z));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexFour, ECuboidSide::Left));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexFive, ECuboidSide::Left));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexSix, ECuboidSide::Left));

			//right
			/*const FCuboidVertexArray& IndexSeven = GetCuboidVertexIndices(FVector(x, YDim, z));
			const FCuboidVertexArray& IndexEight = GetCuboidVertexIndices(FVector(x + 1, YDim, z + 1));
			const FCuboidVertexArray& IndexNine = GetCuboidVertexIndices(FVector(x, YDim, z + 1));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexSeven, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexEight, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexNine, ECuboidSide::Right));

			const FCuboidVertexArray& IndexTen = GetCuboidVertexIndices(FVector(x, YDim, z));
			const FCuboidVertexArray& IndexEleven = GetCuboidVertexIndices(FVector(x + 1, YDim, z));
			const FCuboidVertexArray& IndexTwelve = GetCuboidVertexIndices(FVector(x + 1, YDim, z + 1));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexTen, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexEleven, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexTwelve, ECuboidSide::Right));*/

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	//Front/Back
	for (z = 0; z < ZDim + 1; z++)
	{
		for (y = 0; y < XDim + 1; y++)
		{
			//Front
			TArray<int32> CurrentTriangles;

			const FCuboidVertexArray& IndexOne = GetCuboidVertexIndices(FVector(0, y, z));
			const FCuboidVertexArray& IndexTwo = GetCuboidVertexIndices(FVector(0, y + 1, z + 1));
			const FCuboidVertexArray& IndexThree = GetCuboidVertexIndices(FVector(0, y, z + 1));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexOne, ECuboidSide::Front));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexTwo, ECuboidSide::Front));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexThree, ECuboidSide::Front));

			const FCuboidVertexArray& IndexFour = GetCuboidVertexIndices(FVector(0, y, z));
			const FCuboidVertexArray& IndexFive = GetCuboidVertexIndices(FVector(0, y + 1, z));
			const FCuboidVertexArray& IndexSix = GetCuboidVertexIndices(FVector(0, y + 1, z + 1));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexFour, ECuboidSide::Left));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexFive, ECuboidSide::Left));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexSix, ECuboidSide::Left));

			//Back
			/*const FCuboidVertexArray& IndexSeven = GetCuboidVertexIndices(FVector(XDim, y, z));
			const FCuboidVertexArray& IndexEight = GetCuboidVertexIndices(FVector(XDim, y, z + 1));
			const FCuboidVertexArray& IndexNine = GetCuboidVertexIndices(FVector(XDim, y + 1, z + 1));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexSeven, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexEight, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexNine, ECuboidSide::Right));

			const FCuboidVertexArray& IndexTen = GetCuboidVertexIndices(FVector(XDim, y, z));
			const FCuboidVertexArray& IndexEleven = GetCuboidVertexIndices(FVector(XDim, y + 1, z + 1));
			const FCuboidVertexArray& IndexTwelve = GetCuboidVertexIndices(FVector(XDim, y + 1, z));

			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexTen, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexEleven, ECuboidSide::Right));
			CurrentTriangles.Add(GetCuboidVertexSideIndex(IndexTwelve, ECuboidSide::Right));*/

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	/*OutTriangleArray.Append(CreateCuboidTriangleArray_Bottom(InVertices, InDimensions, InTileSize));
	OutTriangleArray.Append(CreateCuboidTriangleArray_Left(InVertices, InDimensions, InTileSize));
	OutTriangleArray.Append(CreateCuboidTriangleArray_Front(InVertices, InDimensions, InTileSize));
	OutTriangleArray.Append(CreateCuboidTriangleArray_Top(InVertices, InDimensions, InTileSize));
	OutTriangleArray.Append(CreateCuboidTriangleArray_Right(InVertices, InDimensions, InTileSize));
	OutTriangleArray.Append(CreateCuboidTriangleArray_Back(InVertices, InDimensions, InTileSize));*/

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

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetCuboidVertexIndex(FVector(x, y, 0));
			int32 IndexTwo = GetCuboidVertexIndex(FVector(x+1, y+1, 0));
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

			OutTriangleArray.Append(CurrentTriangles);
		}
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

	for (z = 0; z < ZDim; z++)
	{
		for (x = 0; x < XDim; x++)
		{
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetCuboidVertexIndex(FVector(x, 0, z));
			int32 IndexTwo = GetCuboidVertexIndex(FVector(x, 0, z + 1));
			int32 IndexThree = GetCuboidVertexIndex(FVector(x + 1, 0, z + 1));

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			IndexOne = GetCuboidVertexIndex(FVector(x, 0, z));
			IndexTwo = GetCuboidVertexIndex(FVector(x + 1, 0, z + 1));
			IndexThree = GetCuboidVertexIndex(FVector(x + 1, 0, z));


			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Front(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	for (z = 0; z < ZDim; z++)
	{
		for (y = 0; y < YDim; y++)
		{
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetCuboidVertexIndex(FVector(0, y, z));
			int32 IndexTwo = GetCuboidVertexIndex(FVector(0, y+1, z + 1));
			int32 IndexThree = GetCuboidVertexIndex(FVector(0, y, z + 1));

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			IndexOne = GetCuboidVertexIndex(FVector(0, y, z));
			IndexTwo = GetCuboidVertexIndex(FVector(0, y+1, z));
			IndexThree = GetCuboidVertexIndex(FVector(0, y+1, z + 1));


			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Top(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)
		{
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetCuboidVertexIndex(FVector(x, y, ZDim));
			int32 IndexTwo = GetCuboidVertexIndex(FVector(x, y + 1, ZDim));
			int32 IndexThree = GetCuboidVertexIndex(FVector(x + 1, y + 1, ZDim));

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			IndexOne = GetCuboidVertexIndex(FVector(x, y, ZDim));
			IndexTwo = GetCuboidVertexIndex(FVector(x + 1, y + 1, ZDim));
			IndexThree = GetCuboidVertexIndex(FVector(x + 1, y, ZDim));


			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Right(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	for (z = 0; z < ZDim; z++)
	{
		for (x = 0; x < XDim; x++)
		{
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetCuboidVertexIndex(FVector(x, YDim, z));
			int32 IndexTwo = GetCuboidVertexIndex(FVector(x + 1, YDim, z + 1));
			int32 IndexThree = GetCuboidVertexIndex(FVector(x, YDim, z + 1));

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			IndexOne = GetCuboidVertexIndex(FVector(x, YDim, z));
			IndexTwo = GetCuboidVertexIndex(FVector(x + 1, YDim, z));
			IndexThree = GetCuboidVertexIndex(FVector(x + 1, YDim, z + 1));


			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			OutTriangleArray.Append(CurrentTriangles);
		}
	}

	return OutTriangleArray;
}

TArray<int32> AJetCuboidMesh::CreateCuboidTriangleArray_Back(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize)
{
	TArray<int32> OutTriangleArray;

	int32 i = 0;

	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 XDim = (int32)InDimensions.X;
	int32 YDim = (int32)InDimensions.Y;
	int32 ZDim = (int32)InDimensions.Z;

	for (z = 0; z < ZDim; z++)
	{
		for (y = 0; y < YDim; y++)
		{
			TArray<int32> CurrentTriangles;

			int32 IndexOne = GetCuboidVertexIndex(FVector(XDim, y, z));
			int32 IndexTwo = GetCuboidVertexIndex(FVector(XDim, y, z + 1));
			int32 IndexThree = GetCuboidVertexIndex(FVector(XDim, y + 1, z + 1));

			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

			IndexOne = GetCuboidVertexIndex(FVector(XDim, y, z));
			IndexTwo = GetCuboidVertexIndex(FVector(XDim, y + 1, z + 1));
			IndexThree = GetCuboidVertexIndex(FVector(XDim, y + 1, z));


			CurrentTriangles.Add(IndexOne);
			CurrentTriangles.Add(IndexTwo);
			CurrentTriangles.Add(IndexThree);

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
	}

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
	CreateCuboid(Dimensions, TileSize);

	Super::BeginPlay();
}
PRAGMA_ENABLE_OPTIMIZATION