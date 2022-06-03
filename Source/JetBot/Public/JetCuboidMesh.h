// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JetProcMesh.h"
#include "JetCuboidMesh.generated.h"

UENUM()
enum class ECuboidSide : uint8
{
	Bottom,
	Top,
	Left,
	Right,
	Front,
	Back
};

USTRUCT(Blueprintable)
struct FCuboidVertexArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Vertex")
	TArray<int32> VertexArray = { -1,-1,-1,-1,-1,-1 };

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
	static FProcMeshData CreateCuboidData(const FVector& InDimensions, int32 InTileSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	static TArray<FVector> CreateCuboidVertexArray(const FVector& InDimensions, int32 InTileSize, TArray<FProcMeshFaceVertexMap>& OutFaceVertexMapArray);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	static TArray<FVector2D> CreateCuboidUVArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	static TArray<int32> CreateCuboidTriangleArray(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Bottom(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Top(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Left(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Right(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Front(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Back(const FProcMeshData& InProcMeshData, const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

protected:
	virtual void BeginPlay() override;
	
};
