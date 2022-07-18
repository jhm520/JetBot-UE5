// Fill out your copyright notice in the Description page of Project Settings.


#include "JetProceduralMeshComponent.h"



void UJetProceduralMeshComponent::JetFinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup)
{
	TArray<UBodySetup*> NewQueue;
	NewQueue.Reserve(JetAsyncBodySetupQueue.Num());

	int32 FoundIdx;
	if (JetAsyncBodySetupQueue.Find(FinishedBodySetup, FoundIdx))
	{
		if (bSuccess)
		{
			//The new body was found in the array meaning it's newer so use it
			ProcMeshBodySetup = FinishedBodySetup;
			RecreatePhysicsState();

			//remove any async body setups that were requested before this one
			for (int32 AsyncIdx = FoundIdx + 1; AsyncIdx < JetAsyncBodySetupQueue.Num(); ++AsyncIdx)
			{
				NewQueue.Add(JetAsyncBodySetupQueue[AsyncIdx]);
			}

			JetAsyncBodySetupQueue = NewQueue;
		}
		else
		{
			JetAsyncBodySetupQueue.RemoveAt(FoundIdx);
		}
	}
}

void UJetProceduralMeshComponent::JetCreateProcMeshBodySetup()
{
	if (ProcMeshBodySetup == nullptr)
	{
		ProcMeshBodySetup = JetCreateBodySetupHelper();
	}
}

//TArray<TObjectPtr<UBodySetup>> UProceduralMeshComponent::GetAsyncBodySetupQueue()
//{
//	return AsyncBodySetupQueue;
//}
void UJetProceduralMeshComponent::JetUpdateCollision()
{
	//SCOPE_CYCLE_COUNTER(STAT_ProcMesh_UpdateCollision);

	UWorld* World = GetWorld();
	const bool bUseAsyncCook = World && World->IsGameWorld() && bUseAsyncCooking;

	if (bUseAsyncCook)
	{
		// Abort all previous ones still standing
		for (UBodySetup* OldBody : JetAsyncBodySetupQueue)
		{
			OldBody->AbortPhysicsMeshAsyncCreation();
		}

		JetAsyncBodySetupQueue.Add(JetCreateBodySetupHelper());
	}
	else
	{
		JetAsyncBodySetupQueue.Empty();	//If for some reason we modified the async at runtime, just clear any pending async body setups
		JetCreateProcMeshBodySetup();
	}

	UBodySetup* UseBodySetup = bUseAsyncCook ? JetAsyncBodySetupQueue.Last() : ProcMeshBodySetup;

	// Fill in simple collision convex elements
	UseBodySetup->AggGeom.ConvexElems = JetCollisionConvexElems;

	// Set trace flag
	UseBodySetup->CollisionTraceFlag = bUseComplexAsSimpleCollision ? CTF_UseComplexAsSimple : CTF_UseDefault;

	if (bUseAsyncCook)
	{
		UseBodySetup->CreatePhysicsMeshesAsync(FOnAsyncPhysicsCookFinished::CreateUObject(this, &UJetProceduralMeshComponent::JetFinishPhysicsAsyncCook, UseBodySetup));
	}
	else
	{
		// New GUID as collision has changed
		UseBodySetup->BodySetupGuid = FGuid::NewGuid();
		// Also we want cooked data for this
		UseBodySetup->bHasCookedCollisionData = true;
		UseBodySetup->InvalidatePhysicsData();
		UseBodySetup->CreatePhysicsMeshes();
		RecreatePhysicsState();
	}
}

UBodySetup* UJetProceduralMeshComponent::JetCreateBodySetupHelper()
{

	// The body setup in a template needs to be public since the property is Tnstanced and thus is the archetype of the instance meaning there is a direct reference
	UBodySetup* NewBodySetup = NewObject<UBodySetup>(this, NAME_None, (IsTemplate() ? RF_Public | RF_ArchetypeObject : RF_NoFlags));
	NewBodySetup->BodySetupGuid = FGuid::NewGuid();

	NewBodySetup->bGenerateMirroredCollision = false;
	NewBodySetup->bDoubleSidedGeometry = true;
	NewBodySetup->CollisionTraceFlag = bUseComplexAsSimpleCollision ? CTF_UseComplexAsSimple : CTF_UseDefault;

	return NewBodySetup;
}
