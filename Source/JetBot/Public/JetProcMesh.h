// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "JetProcMesh.generated.h"

UCLASS()
class JETBOT_API AJetProcMesh : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FVector> Vertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<int32> Triangles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadWrite, Category = "Procedural Mesh")
		UProceduralMeshComponent* ProcMesh;

	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh")
	void CreateMesh();

public:
	// Sets default values for this actor's properties
	AJetProcMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
		TArray<UMaterialInterface*> Materials;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
