// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "JetProceduralMeshComponent.generated.h"

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

	/** Once async physics cook is done, create needed state */
	void JetFinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup);

	/** Convex shapes used for simple collision */
	UPROPERTY()
	TArray<FKConvexElem> JetCollisionConvexElems;

	/** Queue for async body setups that are being cooked */
	UPROPERTY(transient)
	TArray<TObjectPtr<UBodySetup>> JetAsyncBodySetupQueue;

	/** Ensure ProcMeshBodySetup is allocated and configured */
	void JetCreateProcMeshBodySetup();

	/** Mark collision data as dirty, and re-create on instance if necessary */
	void JetUpdateCollision();

	/** Helper to create new body setup objects */
	UBodySetup* JetCreateBodySetupHelper();
};
