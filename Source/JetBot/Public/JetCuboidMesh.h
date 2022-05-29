// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JetProcMesh.h"
#include "JetCuboidMesh.generated.h"

USTRUCT(Blueprintable)
struct FCuboidVertexArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Vertex")
	TArray<int> VertexArray;

	FCuboidVertexArray() {};
};

/**
 * 
 */
UCLASS()
class JETBOT_API AJetCuboidMesh : public AJetProcMesh
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	FVector Dimensions = FVector(1, 1, 1);;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int32 TileSize = 100;

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void CreateCuboid(const FVector& InDimensions, int32 InTileSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	TArray<FVector> CreateCuboidVertexArray_Old(const FVector& InDimensions, int32 InTileSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	TArray<FVector> CreateCuboidVertexArray(const FVector& InDimensions, int32 InTileSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	TArray<FVector2D> CreateCuboidUVArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<FVector2D> CreateCuboidUVArray_Bottom(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<FVector2D> CreateCuboidUVArray_Left(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<FVector2D> CreateCuboidUVArray_Front(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	TArray<int32> CreateCuboidTriangleArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<int32> CreateCuboidTriangleArray_Bottom(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<int32> CreateCuboidTriangleArray_Left(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<int32> CreateCuboidTriangleArray_Front(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<int32> CreateCuboidTriangleArray_Top(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<int32> CreateCuboidTriangleArray_Right(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	TArray<int32> CreateCuboidTriangleArray_Back(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);


	//InVertexLocation is a vector of the cartesian coordinates. Returns the index
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	static int32 GetCuboidVertexIndex_Old(const TArray<FVector>& InVertices, const FVector& InVertexLocation, const FVector& InDimensions);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	int32 GetCuboidVertexIndex(const FVector& InVertexLocation);


	//a map from cartesian coordinates to vertex index
	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh")
	TMap<FVector, int> VertexIndexMap;

	//a map from cartesian coordinates to vertex indices, each indices array is ordered by (bottom/top, left/right, front/back)
	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh")
	TMap<FVector, FCuboidVertexArray> VertexIndicesMap;

protected:
	virtual void BeginPlay() override;
	
};
