// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "JetProceduralMesh.generated.h"

UCLASS()
class JETBOT_API AJetProceduralMesh : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int32 LandscapeSize = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int32 HeightVariation = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int32 TileSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FVector> Vertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<int32> Triangles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	UProceduralMeshComponent* ProcMesh;

	void CreateMesh();
	
	UFUNCTION(BlueprintCallable, Category="Procedural Mesh")
	void CreateLandscape(int32 InSize);

	TArray<FVector> CreateLandscapeVertexArray(const int32 InSize);
	
	TArray<FVector2D> CreateLandscapeUVArray(const int32 InSize);

	TArray<int32> CreateLandscapeTriangleArray(const int32 InSize);

	void AddLandscapeFeature(const FVector2D InFeatureLocation, TArray<FVector> InFeatureVertexArray);

	int32 GetVertexIndex(const FVector2D InVertexLocation, const int32 InSize);

public:	
	// Sets default values for this actor's properties
	AJetProceduralMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<UMaterialInterface*> Materials;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
