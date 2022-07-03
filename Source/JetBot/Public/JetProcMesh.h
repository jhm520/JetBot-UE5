// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "JetProcMesh.generated.h"


//a struct of integers representing the indices of the Vertices array
USTRUCT(Blueprintable)
struct FJetTriangle
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> TriangleVertexIndices;

	FJetTriangle() {};
};

//A struct to contain Triangle data related to a specific vertex

USTRUCT(Blueprintable)
struct FVertexTriangles
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Triangles")
	TArray<FJetTriangle> Triangles;

	FVertexTriangles() {};
};

//A struct to contain the vertex map for a single face of a Proc mesh
USTRUCT(Blueprintable)
struct FProcMeshFaceVertexMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TMap<FVector, int32> VertexIndexMap;

	FProcMeshFaceVertexMap() {};
};

USTRUCT(BlueprintType)
struct FProcMeshData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	FTransform SpawnTransform = FTransform();

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FVector> Vertices;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<int32> Triangles;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FVertexTriangles> TriangleData;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FVector> Normals;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FProcMeshFaceVertexMap> FaceVertexMapArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<UMaterialInterface*> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	bool bIsActive = false;

	int32 GetVertexIndex(const FVector& InVectorKey, int32 InFaceIndex = 0) const
	{
		if (!FaceVertexMapArray.IsValidIndex(InFaceIndex))
		{
			return -1;
		}
		const FProcMeshFaceVertexMap& FaceVertexMap = FaceVertexMapArray[InFaceIndex];

		const int32* IndexPtr = FaceVertexMap.VertexIndexMap.Find(InVectorKey);

		if (!IndexPtr)
		{
			return -1;
		}

		return *IndexPtr;

	}

	bool FindVertexVector(const FVector& InVectorKey, FVector& OutVertexVector, int32 InFaceIndex = 0) const
	{
		const int32 FoundVertexIndex = GetVertexIndex(InVectorKey, InFaceIndex);

		if (FoundVertexIndex == -1)
		{
			return false;
		}

		if (!Vertices.IsValidIndex(FoundVertexIndex))
		{
			return false;
		}

		OutVertexVector = Vertices[FoundVertexIndex];

		return true;

	}

	FVector GetMapKey() const
	{
		return SpawnTransform.GetLocation() * FVector(1, 1, 0);
	}

	void PopulateTriangleData()
	{
		if (TriangleData.Num() != 0)
		{
			//already done, return
			return;
		}

		if (Vertices.Num() == 0)
		{
			//no vertices yet, return
			return;
		}

		if (Triangles.Num() == 0)
		{
			//no triangles yet, return
			return;
		}

		TriangleData.AddZeroed(Vertices.Num());

		int32 triCount = 0;

		FJetTriangle NewTriangle = FJetTriangle();

		const int32 TriNum = Triangles.Num();

		for (int32 i = 0; i < TriNum; i++)
		{
			NewTriangle.TriangleVertexIndices.Add(Triangles[i]);
			i++;
			NewTriangle.TriangleVertexIndices.Add(Triangles[i]);
			i++;
			NewTriangle.TriangleVertexIndices.Add(Triangles[i]);

			for (int32 ti : NewTriangle.TriangleVertexIndices)
			{
				TriangleData[ti].Triangles.Add(NewTriangle);
			}

			NewTriangle = FJetTriangle();
	
		}
	}

	FVector GetTriangleSurfaceNormal(const FJetTriangle& InTriangle)
	{
		if (InTriangle.TriangleVertexIndices.Num() != 3)
		{
			return FVector::ZeroVector;
		}

		int32 ai = InTriangle.TriangleVertexIndices[0];
		int32 bi = InTriangle.TriangleVertexIndices[1];
		int32 ci = InTriangle.TriangleVertexIndices[2];

		const FVector& A = Vertices[ai];
		const FVector& B = Vertices[bi];
		const FVector& C = Vertices[ci];

		FVector OutNormal = FVector::CrossProduct(B - A, C - A);

		OutNormal = OutNormal.GetSafeNormal();

		return OutNormal;
	}

	FVector GetTriangleCrossProductSum(const FJetTriangle& InTriangle)
	{
		if (InTriangle.TriangleVertexIndices.Num() != 3)
		{
			return FVector::ZeroVector;
		}

		int32 ai = InTriangle.TriangleVertexIndices[0];
		int32 bi = InTriangle.TriangleVertexIndices[1];
		int32 ci = InTriangle.TriangleVertexIndices[2];

		const FVector& A = Vertices[ai];
		const FVector& B = Vertices[bi];
		const FVector& C = Vertices[ci];

		FVector OutNormal = FVector::CrossProduct(B - A, C - A);

		return OutNormal;
	}

	FVector GetVertexNormal(int32 InVertexIndex)
	{
		if (!TriangleData.IsValidIndex(InVertexIndex))
		{
			return FVector::ZeroVector;
		}

		const FVertexTriangles& VT = TriangleData[InVertexIndex];

		FVector NormalSum = FVector::ZeroVector;
		int32 NormalCount = 0;


		for (const FJetTriangle& Tri : VT.Triangles)
		{
			NormalSum += GetTriangleCrossProductSum(Tri);
			NormalCount++;
		}

		FVector NormalAvg = NormalSum/* / NormalCount*/;

		//NormalAvg = NormalAvg.GetSafeNormal();

		return NormalAvg;


	}

	void PopulateNormals()
	{
		if (TriangleData.Num() == 0)
		{
			//no triangle data yet, return
			return;
		}

		if (Vertices.Num() == 0)
		{
			//no vertices yet, return
			return;
		}

		if (Triangles.Num() == 0)
		{
			//no triangles yet, return
			return;
		}

		const int32 VerticesNum = Vertices.Num();

		Normals.AddZeroed(VerticesNum);

		for (int32 i = 0; i < VerticesNum; i++)
		{
			Normals[i] = GetVertexNormal(i)*-1.0f;
		}
	}


	FProcMeshData() {}
};

UCLASS()
class JETBOT_API AJetProcMesh : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
		FProcMeshData ProcMeshData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Procedural Mesh")
		UProceduralMeshComponent* ProcMesh;

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
		void CreateMesh(TArray<UMaterialInterface*> InMaterialArray);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void CreateMeshSection(const FProcMeshData& InProcMeshData);

	UPROPERTY()
	TArray<FProcMeshData> MeshSections;

public:
	// Sets default values for this actor's properties
	AJetProcMesh();

	static int32 GetVertexIndex(const TMap<FVector, int32>& InVertexIndexMap, const FVector& InVertexLocation, const FVector& InDimensions);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
