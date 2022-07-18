// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "JetProceduralMeshComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("JetProceduralMesh"), STATGROUP_JetProceduralMesh, STATCAT_Advanced);

/**
 * 
 */
UCLASS()
class JETBOT_API UJetProceduralMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
	/*UFUNCTION()
	TArray<UBodySetup*> GetAsyncBodySetupQueue();*/

public:

	/** Array of sections of mesh */
	UPROPERTY()
	TArray<FProcMeshSection> JetProcMeshSections;

	/**
	 *	Create/replace a section for this procedural mesh component.
	 *	This function is deprecated for Blueprints because it uses the unsupported 'Color' type. Use new 'Create Mesh Section' function which uses LinearColor instead.
	 *	@param	SectionIndex		Index of the section to create or replace.
	 *	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	 *	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	 *	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (DeprecatedFunction, DeprecationMessage = "This function is deprecated for Blueprints because it uses the unsupported 'Color' type. Use new 'Create Mesh Section' function which uses LinearColor instead.", DisplayName = "Create Mesh Section FColor", AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
		void JetCreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision)
	{
		TArray<FVector2D> EmptyArray;
		JetCreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UV0, EmptyArray, EmptyArray, EmptyArray, VertexColors, Tangents, bCreateCollision);
	}

	void JetCreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FVector2D>& UV1, const TArray<FVector2D>& UV2, const TArray<FVector2D>& UV3, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision);


	/** Once async physics cook is done, create needed state */
	void JetFinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup);

	/** Convex shapes used for simple collision */
	UPROPERTY()
	TArray<FKConvexElem> JetCollisionConvexElems;

	/** Queue for async body setups that are being cooked */
	UPROPERTY(transient)
	TArray<TObjectPtr<UBodySetup>> JetAsyncBodySetupQueue;

	/** Local space bounds of mesh */
	UPROPERTY()
	FBoxSphereBounds JetLocalBounds;

	/** Ensure ProcMeshBodySetup is allocated and configured */
	void JetCreateProcMeshBodySetup();

	/** Mark collision data as dirty, and re-create on instance if necessary */
	void JetUpdateCollision();

	/** Update LocalBounds member from the local box of each section */
	void JetUpdateLocalBounds();

	/** Helper to create new body setup objects */
	UBodySetup* JetCreateBodySetupHelper();
};
