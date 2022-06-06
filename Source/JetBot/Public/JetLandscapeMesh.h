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

	//the radius at which the player's tile spawns its neighbors.
	//A radius of 1 will spawn 8 tiles around the origin tile, 2 will spawn 8 tiles,
	//then another 16 around the 8, 3 will do another layer, and so on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	int32 NeighborSpawnRadius = 1;

	//Creates data needed to make a landscape
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	static FProcMeshData CreateLandscapeData(const FTransform& InSpawnTransform, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation);

	void ZipLandscapeDataWithNeighbors(FProcMeshData& InOutLandscapeData);

	void ZipLandscapeDataWithNeighbor(ECardinalDirection InNeighborDirection, FProcMeshData& InOutLandscapeData);

	UFUNCTION(BlueprintCallable,BlueprintPure, BlueprintNativeEvent, Category = "Procedural Mesh")
	AJetLandscapeMesh* GetNeighborLandscape(ECardinalDirection InNeighborDirection);

	UFUNCTION(BlueprintCallable,BlueprintPure, Category = "Procedural Mesh")
	bool GetNeighborLandscapeData(ECardinalDirection InNeighborDirection, FProcMeshData& OutProcMeshData);

	UFUNCTION(BlueprintCallable,BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Procedural Mesh")
	static bool Static_GetNeighborLandscapeData(UObject* WorldContextObject, const FProcMeshData& InLandscapeData, ECardinalDirection InNeighborDirection, FProcMeshData& OutNeighborData, int32 InVectorScale);


	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void SpawnNeighborLandscapesInRadius();

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	TArray<AJetLandscapeMesh*> GetNeighborLandscapesInRadius();

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void SpawnNeighborLandscapes();

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	TArray<AJetLandscapeMesh*> GetAllNeighborLandscapes();

	//Spawn a landscape neighboring this landscape
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
		AJetLandscapeMesh* SpawnNeighborLandscape(ECardinalDirection InNeighborDirection);

	//Spawn a landscape neighboring this landscape
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	bool CreateNewNeighborLandscapeData(ECardinalDirection InNeighborDirection, FProcMeshData& OutProcMeshData);

private:
	//Spawn a landscape neighboring this landscape with data
	AJetLandscapeMesh* SpawnNeighboringLandscapeWithData(const FProcMeshData& InNeighborData);
public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Procedural Mesh")
	static AJetLandscapeMesh* SpawnLandscapeWithData(UObject* WorldContextObject, const FProcMeshData& InProcMeshData, int32 InLandscapeSize, int32 InTileSize, int32 InHeightVariation, int32 InNeighborSpawnRadius);

	//"Zip" Landscape two to be congruent with landscape one
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void ZipNeighborLandscape(AJetLandscapeMesh* InZipper, AJetLandscapeMesh* InZippee);

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	static FTransform GetNeighborLandscapeSpawnTransform(ECardinalDirection InNeighborDirection, int32 InVectorScale);

	/*UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh")
	TMap<ECardinalDirection, FTransform> NeighborSpawnTransformMap;*/

	//Returns the direction needed to get from LandscapeOne to LandscapeTwo
	ECardinalDirection GetNeighborCardinality(AJetLandscapeMesh* InLandscapeOne, AJetLandscapeMesh* InLandscapeTwo);

	//Returns the direction needed to get from LandscapeOne to LandscapeTwo
	static ECardinalDirection GetNeighborDataCardinality(const FProcMeshData& InLandscapeDataOne, const FProcMeshData& InLandscapeDataTwo);


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
