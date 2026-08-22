// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMV_InteractFunctionality_Base.h"
#include "DMV_InteractComponent.generated.h"

/**
 * Goes on anything that should be interactable - the item-side counterpart to
 * UDMV_InteractorComponent. Owns this interactable's actual behavior as a list of
 * UDMV_InteractFunctionality_Base instances (see Functionalities) - the owning actor (e.g.
 * ADMV_InteractItem_Base, though using that base class isn't required, only having this component
 * and implementing IDMV_InteractInterface is) is meant to be little more than a visual wrapper
 * around whatever this component and its Functionalities actually do.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DMV_INTERACTSYSTEM_API UDMV_InteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDMV_InteractComponent();

	/** This interactable's functionalities. Each is checked independently
	 *  (UDMV_InteractFunctionality_Base::CanExecute) and executed if it passes - one failing its
	 *  own tag requirements doesn't block the others. An interactable can combine several (e.g.
	 *  both a Glow and a Carry functionality) or just use one. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Interact")
	TArray<UDMV_InteractFunctionality_Base*> Functionalities;

	/** Runs CanExecute/ExecuteFunctionality for every functionality that currently passes its own
	 *  check. Returns true if at least one functionality actually executed. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	bool TriggerFunctionalities(AActor* Interactor);

protected:
	virtual void BeginPlay() override;
};
