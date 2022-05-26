// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Brush.h"
#include "JetBrush.generated.h"

/**
 * 
 */
UCLASS()
class JETBOT_API AJetBrush : public ABrush
{
	GENERATED_BODY()
public:
	void ChangeBrush();
	
};
