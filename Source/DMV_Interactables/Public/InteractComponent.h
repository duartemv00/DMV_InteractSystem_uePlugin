// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DMV_INTERACTABLES_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractComponent();

protected:
	virtual void BeginPlay() override;
};
