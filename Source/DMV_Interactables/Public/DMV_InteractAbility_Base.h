// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DMV_InteractAbility_Base.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, EditInlineNew, Blueprintable)
class DMV_INTERACTABLES_API UDMV_InteractAbility_Base : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExecuteCustomLogic(class ADMV_InteractItem_Base* Executor);
};
