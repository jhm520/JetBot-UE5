// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JetProcMesh.h"
#include "JetLandscapeMesh.generated.h"

/**
 * 
 */
UCLASS()
class JETBOT_API AJetLandscapeMesh : public AJetProcMesh
{
	GENERATED_BODY()

public:

	AJetLandscapeMesh();

	/*UPROPERTY(EditAnywhere, Category = "Procedural Mesh")
	TSubclassOf<AJetLandscapeMesh> NeighborLandscapeClass;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Procedural Mesh")
	bool bSpawnNeighborLandscapes = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Procedural Mesh")
	bool bAutoCreateLandscape = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int32 LandscapeSize = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int32 HeightVariation = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int32 TileSize = 100;

	//a map from cartesian coordinates to vertex index
	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh")
	TMap<FVector2D, int32> VertexIndexMap;

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void CreateLandscape(int32 InSize);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void SpawnNeighborLandscapes();

	TArray<FVector> CreateLandscapeVertexArray(const int32 InSize);

	TArray<FVector2D> CreateLandscapeUVArray(const int32 InSize);

	TArray<int32> CreateLandscapeTriangleArray(const int32 InSize);

	void AddLandscapeFeature(const FVector2D InFeatureLocation, TArray<FVector> InFeatureVertexArray);

	int32 GetVertexIndex(const FVector2D InVertexLocation, const int32 InSize);

protected:
	virtual void BeginPlay() override;
};
