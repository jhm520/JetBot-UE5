// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "JetProcMesh.generated.h"

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
	TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<FProcMeshFaceVertexMap> FaceVertexMapArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	TArray<UMaterialInterface*> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	bool bIsActive = false;

	FVector GetMapKey() const
	{
		return SpawnTransform.GetLocation() * FVector(1, 1, 0);
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

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
		UProceduralMeshComponent* ProcMesh;

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void CreateMesh();

public:
	// Sets default values for this actor's properties
	AJetProcMesh();

	static int32 GetVertexIndex(TMap<FVector, int32> InVertexIndexMap, const FVector& InVertexLocation, const FVector& InDimensions);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
