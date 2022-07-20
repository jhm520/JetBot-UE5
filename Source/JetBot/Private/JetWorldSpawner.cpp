// Fill out your copyright notice in the Description page of Project Settings.


#include "JetWorldSpawner.h"
#include "JetGameState.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"
#include "JetProceduralMeshComponent.h"

PRAGMA_DISABLE_OPTIMIZATION
// Sets default values
AJetWorldSpawner::AJetWorldSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	JetLandscapeProcMesh = CreateDefaultSubobject<UJetProceduralMeshComponent>("JetProcMesh");
	RootComponent = JetLandscapeProcMesh;
}

AJetLandscapeMesh* AJetWorldSpawner::GetCurrentLandscapeMesh(UObject* WorldContextObject)
{
	AJetGameState* GameState = Cast<AJetGameState>(WorldContextObject->GetWorld()->GetGameState());

	if (!GameState || !GameState->WorldSpawner)
	{
		return nullptr;
	}

	return GameState->WorldSpawner->CurrentLandscape;
}

void AJetWorldSpawner::OnLandscapeDataCreated(const FOnLandscapeDataCreatedResult& DEPInLandscapeData)
{
	//bCreatingLandscapeData = false;

	OnLandscapeDataCreatedMutex = false;

	const int32 VertexNum = WorldLandscapeData.LandscapeVerticesMap.Num();

	const int32 NewLandscapeNum = NewWorldLandscapeData.LandscapeArray.Num();

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	if (NewLandscapeNum == 0)
	{
		OnLandscapesFinishedSpawning();
	}

	/*if (InLandscapeData.LandscapeArray.Num() == 0)
	{
		OnLandscapesFinishedSpawning();
	}*/

	//const FProcMeshData& ProcMesh = InLandscapeData.LandscapeArray[0];

	/*AJetLandscapeMesh* NewLandscape = AJetLandscapeMesh::SpawnLandscapeWithData(this, ProcMesh, this->LandscapeProperties, this);

	if (CurrentLandscape)
	{
		CurrentLandscape->SmoothDestroy();
	}

	CurrentLandscape = NewLandscape;*/

	//OnLandscapesFinishedSpawning();



	if (NewLandscapeNum > 0)
	{
		//GameState->LandscapeDataMap.Append(InLandscapeData.LandscapeDataMap);

		//GameState->AppendLandscapeSpawnQueue(NewWorldLandscapeData.LandscapeArray);

		//WorldLandscapeVertexMap.Append(InLandscapeData.LandscapeVerticesMap);
		//WorldLandscapeNormalMap.Append(InLandscapeData.LandscapeNormalMap);
	}
	

	//// Spawn all of the new neighbors
	//for (const FProcMeshData& Landscape : InLandscapeData.LandscapeArray)
	//{
	//	/*Landscape.bIsActive = true;
	//	OnLandscapeDataCreated(WorldContextObject, Landscape);*/
	//	AJetLandscapeMesh::SpawnLandscapeWithData(this, Landscape, LandscapeProperties, this);
	//}

	/*if (PlayerEnteredLandscapeQueue.Num() == 0)
	{
		return;
	}*/

	if (PlayerEnteredLandscapeIndexQueue.Num() == 0)
	{
		return;
	}

	int32 EnteredLandscapeIndex = PlayerEnteredLandscapeIndexQueue[0];

	if (EnteredLandscapeIndex < 0)
	{
		return;
	}

	/*JetLandscapeMesh* EnteredLandscape = PlayerEnteredLandscapeQueue[0];

	if (!EnteredLandscape)
	{
		return;
	}*/


	OnCharacterEnteredNewLandscapeSection(UGameplayStatics::GetPlayerCharacter(this, 0), EnteredLandscapeIndex);

	PlayerEnteredLandscapeIndexQueue.RemoveAt(0);

	/*WorldSpawner_OnPlayerEnteredLandscape(EnteredLandscape, nullptr, EnteredLandscape->GetActorLocation());

	PlayerEnteredLandscapeQueue.RemoveAt(0);*/

}

void AJetWorldSpawner::CreateLandscapeMeshSectionWithData(UObject* WorldContextObject, const FProcMeshData& InProcMeshData, const FLandscapeProperties& InLandscapeProperties, AJetWorldSpawner* InWorldSpawner)
{
	if (!JetLandscapeProcMesh)
	{
		return;
	}


	JetLandscapeProcMesh->CreateMeshSection(CurrentMeshSectionIndex, InProcMeshData.WorldVertices, InProcMeshData.Triangles, InProcMeshData.Normals, InProcMeshData.UVs, TArray<FColor>(), TArray<FJetProcMeshTangent>(), true);
	
	FJetProcMeshSection* NewProcMeshSection = JetLandscapeProcMesh->GetProcMeshSection(CurrentMeshSectionIndex);

	if (NewProcMeshSection)
	{
		UBodySetup* NewBodySetup = nullptr;

		if (JetLandscapeProcMesh->AsyncBodySetupQueue.Num() > 0)
		{
			NewBodySetup = JetLandscapeProcMesh->AsyncBodySetupQueue.Last();
		}

		if (NewBodySetup)
		{
			JetLandscapeProcMesh->OnJetProcMeshAsyncPhysicsCookFinishedDelegate = FOnJetProcMeshAsyncPhysicsCookFinished::CreateUObject(this, &AJetWorldSpawner::FinishPhysicsAsyncCook, NewBodySetup);
			bIsPhysicsAsyncCook = true;
		}

	}

	FProcMeshData* DataPtr = WorldLandscapeData.LandscapeDataMap.Find(InProcMeshData.SpawnTransform.GetLocation() * FVector(1, 1, 0));

	if (DataPtr)
	{
		DataPtr->bIsActive = true;
	}

	//FJetProcMeshSection* Section = JetLandscapeProcMesh->GetProcMeshSection(CurrentMeshSectionIndex);

	//FOnAsyncPhysicsCookFinished::CreateUObject(this, &AJetWorldSpawner::WorldSpawner_FinishPhysicsAsyncCook, LandscapeProcMesh->AsyncBodySetupQueue.Last() != nullptr);

	//TODO: USe this to get the section
	//const FJetProcMeshVertex& VertexZero = Section->ProcVertexBuffer[0];

	if (WorldLandscapeMaterial)
	{
		JetLandscapeProcMesh->SetMaterial(CurrentMeshSectionIndex, WorldLandscapeMaterial);

		LandscapeProcMeshSectionIndexArray.AddUnique(CurrentMeshSectionIndex);
		CurrentMeshSectionIndex++;
		return;
	}

	int32 i = 0;

	for (UMaterialInterface* Mat : InProcMeshData.Materials)
	{
		JetLandscapeProcMesh->SetMaterial(i, Mat);

		i++;
	}

	LandscapeProcMeshSectionIndexArray.AddUnique(CurrentMeshSectionIndex);
	CurrentMeshSectionIndex++;
	return;
}

int32 AJetWorldSpawner::GetActorCurrentLandscapeSectionIndex(AActor* InActor)
{
	//TODO: Implement

	if (!InActor)
	{
		return -1;
	}

	if (!JetLandscapeProcMesh)
	{
		return -1;
	}

	const FVector ActorLoc = InActor->GetActorLocation();

	for (int32 i = 0; i < CurrentMeshSectionIndex; i++)
	{
		FJetProcMeshSection* Section = JetLandscapeProcMesh->GetProcMeshSection(i);

		if (!Section)
		{
			continue;
		}

		if (Section->ProcVertexBuffer.Num() == 0)
		{
			continue;
		}

		const FVector& StartLoc = Section->ProcVertexBuffer[0].Position;

		const FVector& EndLoc = Section->ProcVertexBuffer[Section->ProcVertexBuffer.Num() - 1].Position;

		bool bXStart = StartLoc.X < EndLoc.X;

		float xStart = bXStart ? StartLoc.X : EndLoc.X;

		float xEnd = bXStart ? EndLoc.X : StartLoc.X;

		bool bYStart = StartLoc.Y < EndLoc.Y;

		float yStart = bYStart ? StartLoc.Y : EndLoc.Y;

		float yEnd = bYStart ? EndLoc.Y : StartLoc.Y;

		bool bWithinX = FMath::IsWithin<float>(ActorLoc.X, xStart, xEnd);

		bool bWithinY = FMath::IsWithin<float>(ActorLoc.Y, yStart, yEnd);

		if (bWithinX && bWithinY)
		{
			return i;
		}
	}

	return -1;
}

void AJetWorldSpawner::OnCharacterEnteredNewLandscapeSection(ACharacter* InCharacter, int32 InLandscapeSectionIndex)
{
	if (!InCharacter)
	{
		return;
	}

	if (!JetLandscapeProcMesh)
	{
		return;
	}

	if (InLandscapeSectionIndex < 0)
	{
		return;
	}

	if (bCreatingLandscapeData)
	{
		PlayerEnteredLandscapeIndexQueue.Add(InLandscapeSectionIndex);
		return;
	}


	FJetProcMeshSection* Section = JetLandscapeProcMesh->GetProcMeshSection(InLandscapeSectionIndex);

	if (!Section)
	{
		return;
	}


	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	if (Section->ProcVertexBuffer.Num() == 0)
	{
		return;
	}

	TWeakObjectPtr<AJetWorldSpawner> WeakPtr = this;
	bCreatingLandscapeData = true;
	AsyncCreateLandscapeData(LandscapeCreatedDelegate, Section->ProcVertexBuffer[0].Position*FVector(1,1,0), LandscapeProperties, WeakPtr, GameState->LandscapeDataMap, WorldLandscapeVertexMap, WorldLandscapeNormalMap, &WorldLandscapeData, &NewWorldLandscapeData);

}

void AJetWorldSpawner::OnCharacterExitedLandscapeSection(ACharacter* InCharacter, int32 InExitedLandscapeSectionIndex, int32 InNewLandscapeSectionIndex)
{
	if (!InCharacter || !InCharacter->IsLocallyControlled())
	{
		return;
	}

	if (!(InNewLandscapeSectionIndex > -1) || !(InExitedLandscapeSectionIndex > -1))
	{
		return;
	}

	TArray<int32> DestroySectionIndices;

	FVector CharLoc = InCharacter->GetActorLocation();

	int32 MaxDist = LandscapeProperties.NeighborSpawnRadius * LandscapeProperties.GetVectorScale();
	for (int32 LandscapeMeshSectionIndex : LandscapeProcMeshSectionIndexArray)
	{
		FJetProcMeshSection* NewLandscapeSection = JetLandscapeProcMesh->GetProcMeshSection(InNewLandscapeSectionIndex);
		FJetProcMeshSection* LandscapeSection = JetLandscapeProcMesh->GetProcMeshSection(LandscapeMeshSectionIndex);

		if (!NewLandscapeSection || !LandscapeSection)
		{
			continue;
		}

		FVector NewLoc = NewLandscapeSection->ProcVertexBuffer[0].Position;
		FVector LandscapeLoc = LandscapeSection->ProcVertexBuffer[0].Position;

		FVector Dist = NewLoc - LandscapeLoc;

		if (FMath::Abs(Dist.X) > MaxDist)
		{
			DestroySectionIndices.Add(LandscapeMeshSectionIndex);
		}
		else if (FMath::Abs(Dist.Y) > MaxDist)
		{
			DestroySectionIndices.Add(LandscapeMeshSectionIndex);
		}
	}

	LandscapeProcMeshSectionIndexDestroyQueue.Append(DestroySectionIndices);
}

// Called when the game starts or when spawned
void AJetWorldSpawner::BeginPlay()
{
	Super::BeginPlay();

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	GameState->WorldSpawner = this;

	LandscapeCreatedDelegate.AddDynamic(this, &AJetWorldSpawner::OnLandscapeDataCreated);
	
	//GameState->CreateWorldLandscapes(GetActorLocation(), WorldRadius, LandscapeProperties);
	
	WorldLandscapeMaterial = UMaterialInstanceDynamic::Create(LandscapeProperties.LandscapeMaterialClass, this);

	if (bSpawnWorldAtBeginPlay)
	{
		/*FProcMeshData FirstLandscape = AJetLandscapeMesh::CreateLandscapeData(FTransform(GetActorLocation()), LandscapeProperties);

		AJetLandscapeMesh::SpawnLandscapeWithData(this, FirstLandscape, LandscapeProperties, this);*/

		//AJetLandscapeMesh::SpawnNeighborLandscapesInRadius(this, GetActorLocation(), LandscapeProperties, this, GameState->LandscapeDataMap);
		bCreatingLandscapeData = true;
		int32 Mod = LandscapeProperties.GetVectorScale() / 2;
		FVector VectorMod = FVector(Mod, Mod, 0);
		AsyncCreateLandscapeData(LandscapeCreatedDelegate, GetActorLocation()-VectorMod, LandscapeProperties, this, GameState->LandscapeDataMap, WorldLandscapeVertexMap, WorldLandscapeNormalMap, &WorldLandscapeData, &NewWorldLandscapeData);
	}
}

// Called every frame
void AJetWorldSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJetWorldSpawner::AsyncCreateLandscapeData(FOnLandscapeDataCreatedDelegate Out, const FVector& InLocation, const FLandscapeProperties& InLandscapeProperties, TWeakObjectPtr<AJetWorldSpawner> InWorldSpawner, const TMap<FVector, FProcMeshData>& InLandscapeDataMap, const TMap<FVector, FLandscapeVertexData>& InLandscapeVerticesMap, const TMap<FVector, FVector>& InLandscapeNormalMap, FOnLandscapeDataCreatedResult* InWorldLandscapeData, FOnLandscapeDataCreatedResult* InOutNewWorldLandscapeData)
{
	OnLandscapeDataCreatedMutex = true;

	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [Out, InLocation, InLandscapeProperties, InWorldSpawner, InLandscapeDataMap, InLandscapeVerticesMap, InLandscapeNormalMap, InWorldLandscapeData, InOutNewWorldLandscapeData]()
	{
		FOnLandscapeDataCreatedResult& WorldDataRef = *InWorldLandscapeData;

		FOnLandscapeDataCreatedResult& NewWorldDataRef = *InOutNewWorldLandscapeData;

		AJetLandscapeMesh::CreateLandscapesInRadius(InLocation, InLandscapeProperties, NewWorldDataRef.LandscapeArray, WorldDataRef.LandscapeDataMap, WorldDataRef.LandscapeVerticesMap, WorldDataRef.LandscapeNormalMap);

		/*FProcMeshData InOutSuperLandscapeData;

		AJetLandscapeMesh::CreateLandscapeDataInRadius(InLocation, InLandscapeProperties, OutLandscapeResult.LandscapeArray, InOutSuperLandscapeData, OutLandscapeResult.LandscapeDataMap, OutLandscapeResult.LandscapeVerticesMap);*/

		/*FOnLandscapeDataCreatedResult& DataRef = *InWorldLandscapeData;
		DataRef.LandscapeVerticesMap = OutLandscapeResult.LandscapeVerticesMap;*/

		AsyncTask(ENamedThreads::GameThread, [Out]()
		{
			FOnLandscapeDataCreatedResult OutLandscapeResult;
			// We execute the delegate along with the param
			Out.Broadcast(OutLandscapeResult);
		});
	});
}

void AJetWorldSpawner::WorldSpawner_OnPlayerEnteredLandscape(AJetLandscapeMesh* InLandscape, ACharacter* InPlayer, const FVector& InLandscapeSegmentVector)
{
	if (!InLandscape)
	{
		return;
	}

	if (bCreatingLandscapeData)
	{
		PlayerEnteredLandscapeQueue.Add(InLandscape);
		return;
	}

	AJetGameState* GameState = Cast<AJetGameState>(GetWorld()->GetGameState());

	if (!GameState)
	{
		return;
	}

	TWeakObjectPtr<AJetWorldSpawner> WeakPtr = this;
	bCreatingLandscapeData = true;
	AsyncCreateLandscapeData(LandscapeCreatedDelegate, InLandscape->GetActorLocation(), LandscapeProperties, WeakPtr, GameState->LandscapeDataMap, WorldLandscapeVertexMap, WorldLandscapeNormalMap, &WorldLandscapeData, &NewWorldLandscapeData);
}

void AJetWorldSpawner::WorldSpawner_TickSpawnLandscape()
{
	if (OnLandscapeDataCreatedMutex)
	{
		return;
	}

	if (NewWorldLandscapeData.LandscapeArray.Num() == 0)
	{
		return;
	}

	//if a previous section's collision is being generated async at the moment, wait until its finished before queuing another one
	if (bIsPhysicsAsyncCook)
	{
		return;
	}


	FProcMeshData& FirstLandscape = NewWorldLandscapeData.LandscapeArray[0];

	CreateLandscapeMeshSectionWithData(this, FirstLandscape, LandscapeProperties, this);

	//AJetLandscapeMesh::SpawnLandscapeWithData(this, FirstLandscape, WorldSpawner->LandscapeProperties, WorldSpawner);

	NewWorldLandscapeData.LandscapeArray.RemoveAt(0);

	if (NewWorldLandscapeData.LandscapeArray.Num() == 0)
	{
		OnLandscapesFinishedSpawning();
	}

}

void AJetWorldSpawner::WorldSpawner_TickDestroyLandscape()
{
	if (LandscapeProcMeshSectionIndexDestroyQueue.Num() == 0)
	{
		return;
	}

	int32 FirstIndex = LandscapeProcMeshSectionIndexDestroyQueue[0];

	FJetProcMeshSection* SectionPtr = JetLandscapeProcMesh->GetProcMeshSection(FirstIndex);

	if (SectionPtr)
	{
		if (SectionPtr->ProcVertexBuffer.Num() > 0)
		{
			FVector MapKey = SectionPtr->ProcVertexBuffer[0].Position;

			MapKey.Z = 0;

			FProcMeshData* DataPtr = WorldLandscapeData.LandscapeDataMap.Find(MapKey);

			if (DataPtr)
			{
				DataPtr->bIsActive = false;
			}
		}
	}

	LandscapeProcMeshSectionIndexDestroyQueue.RemoveAt(0);

	if (JetLandscapeProcMesh)
	{
		JetLandscapeProcMesh->ClearMeshSection(FirstIndex);
	}

	LandscapeProcMeshSectionIndexArray.Remove(FirstIndex);
}

void AJetWorldSpawner::WorldSpawner_FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup)
{

}

void AJetWorldSpawner::FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup)
{
	bIsPhysicsAsyncCook = false;

}

void AJetWorldSpawner::OnLandscapesFinishedSpawning_Implementation()
{
	bCreatingLandscapeData = false;

	OnWorldSpawned();

	if (PlayerEnteredLandscapeQueue.Num() == 0)
	{
		return;
	}

	AJetLandscapeMesh* EnteredLandscape = PlayerEnteredLandscapeQueue[0];

	if (!EnteredLandscape)
	{
		return;
	}

	WorldSpawner_OnPlayerEnteredLandscape(EnteredLandscape, nullptr, EnteredLandscape->GetActorLocation());

	PlayerEnteredLandscapeQueue.RemoveAt(0);
}
PRAGMA_ENABLE_OPTIMIZATION