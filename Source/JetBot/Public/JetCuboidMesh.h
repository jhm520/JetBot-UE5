// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JetProcMesh.h"
#include "JetCuboidMesh.generated.h"

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
	static TArray<FVector> CreateCuboidVertexArray(const FVector& InDimensions, int32 InTileSize);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	static TArray<FVector2D> CreateCuboidUVArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<FVector2D> CreateCuboidUVArray_Bottom(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<FVector2D> CreateCuboidUVArray_Left(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	static TArray<int32> CreateCuboidTriangleArray(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Bottom(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	static TArray<int32> CreateCuboidTriangleArray_Left(const TArray<FVector>& InVertices, const FVector& InDimensions, int32 InTileSize);

	//InVertexLocation is a vector of the cartesian coordinates. Returns the index
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Mesh")
	static int32 GetCuboidVertexIndex(const TArray<FVector>& InVertices, const FVector& InVertexLocation, const FVector& InDimensions);

protected:
	virtual void BeginPlay() override;
	
};
