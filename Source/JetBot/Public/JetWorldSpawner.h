// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../JetBot.h"
#include "JetWorldSpawner.generated.h"

UCLASS()
class JETBOT_API AJetWorldSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJetWorldSpawner();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	int32 WorldRadius = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
	FLandscapeProperties LandscapeProperties;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	bool bSpawnWorldAtBeginPlay = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
