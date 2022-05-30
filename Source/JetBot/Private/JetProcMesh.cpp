// Fill out your copyright notice in the Description page of Project Settings.


#include "JetProcMesh.h"


AJetProcMesh::AJetProcMesh()
{
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = ProcMesh;

	//Default values

	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(100, 0, 0));
	Vertices.Add(FVector(0, 100, 0));
	Vertices.Add(FVector(100, 100, 0));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(0, 1));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));

	//Triangle1
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	//Triangle2
	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(3);

}

void AJetProcMesh::CreateMesh()
{

	ProcMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	for (UMaterialInterface* Mat : Materials)
	{
		int32 i = 0;

		ProcMesh->SetMaterial(i, Mat);

		i++;
	}
}

// Called when the game starts or when spawned
void AJetProcMesh::BeginPlay()
{
	Super::BeginPlay();

	//CreateMesh();
}

// Called every frame
void AJetProcMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

