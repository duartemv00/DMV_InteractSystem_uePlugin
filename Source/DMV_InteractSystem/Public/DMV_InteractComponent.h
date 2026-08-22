// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMV_InteractComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DMV_INTERACTSYSTEM_API UDMV_InteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDMV_InteractComponent();

protected:
	virtual void BeginPlay() override;
};
