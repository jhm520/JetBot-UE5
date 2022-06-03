// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JetProcMesh.h"
#include "JetLandscapeMesh.generated.h"

/**
 * 
 */

class USphereComponent;

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	None,
	North,
	Northeast,
	East,
	Southeast,
	South,
	Southwest,
	West,
	Northwest
};

UCLASS()
class JETBOT_API AJetLandscapeMesh : public AJetProcMesh
{
	GENERATED_BODY()

public:

	AJetLandscapeMesh();

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	USphereComponent* SphereCollider = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void OnPlayerEnteredLandscape(ACharacter* InPlayer);

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void OnPlayerExitedLandscape(ACharacter* InPlayer, AJetLandscapeMesh* NewLandscape);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	bool bSpawnNeighborLandscapesAtBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	bool bHasSpawnedNeighborLandscapes = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	bool bAutoCreateLandscape = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	int32 LandscapeSize = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	int32 HeightVariation = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	int32 TileSize = 100;

	//Creates data needed to make a landscape
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	static FProcMeshData CreateLandscapeData(const FTransform& InSpawnTransform, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation);

	void ZipLandscapeDataWithNeighbors(AJetLandscapeMesh* InZippee, FProcMeshData& InOutLandscapeData);

	UFUNCTION(BlueprintCallable,BlueprintPure, BlueprintNativeEvent, Category = "Procedural Mesh")
	AJetLandscapeMesh* GetNeighborLandscape(ECardinalDirection InNeighborDirection);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void SpawnNeighborLandscapes();

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	TArray<AJetLandscapeMesh*> GetAllNeighborLandscapes();

	//Spawn a landscape neighboring this landscape
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void SpawnNeighborLandscape(ECardinalDirection InNeighborDirection);

private:
	//Spawn a landscape neighboring this landscape with data
	void SpawnNeighboringLandscapeWithData(const FProcMeshData& InNeighborData);
public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Procedural Mesh")
	static void SpawnLandscapeWithData(UObject* WorldContextObject, const FProcMeshData& InProcMeshData, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation);

	//"Zip" Landscape two to be congruent with landscape one
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void ZipNeighborLandscape(AJetLandscapeMesh* InZipper, AJetLandscapeMesh* InZippee);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	FTransform GetNeighborLandscapeSpawnTransform(ECardinalDirection InNeighborDirection);

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh")
	TMap<ECardinalDirection, FTransform> NeighborSpawnTransformMap;

	//Returns the direction needed to get from LandscapeOne to LandscapeTwo
	ECardinalDirection GetNeighborCardinality(AJetLandscapeMesh* InLandscapeOne, AJetLandscapeMesh* InLandscapeTwo);

	static TArray<FVector> CreateLandscapeVertexArray(int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation, FProcMeshFaceVertexMap& OutLandscapeVertexMap);

	static TArray<FVector2D> CreateLandscapeUVArray(int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation);

	static TArray<int32> CreateLandscapeTriangleArray(const TMap<FVector, int32>& InVertexIndexMap, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation);

	void AddLandscapeFeature(const FVector InFeatureLocation, TArray<FVector> InFeatureVertexArray);

	int32 GetVertexIndexOld(const FVector2D InVertexLocation, const int32 InSize);


	TMap<FVector, int32> GetLandscapeVertexMap();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

};
