// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DMV_InteractFunctionality_Base.generated.h"

/**
 * The actual behavior of an interactable, kept separate from the actor that carries it - see
 * UDMV_InteractComponent::Functionalities. An interactable actor is meant to be little more than a
 * visual wrapper; what actually happens on interact (make the item glow, let the player carry it,
 * open a door...) lives here, one UDMV_InteractFunctionality_Base subclass per behavior, authored
 * in Blueprint. Deliberately not called "Ability" to avoid implying any relationship to the
 * Gameplay Ability System - this plugin has no dependency on it (see the plugin README) - even
 * though the concept (a per-instance-configurable, EditInlineNew logic hook) is the same idea.
 *
 * This is the simple, first-pass version: a component can hold several of these and every one
 * that currently CanExecute gets executed independently when the component is triggered - no
 * sequencing or inter-functionality conditions yet. A future pass may add ordered/conditional
 * sequences on top of this for more complex interactables.
 */
UCLASS(BlueprintType, EditInlineNew, Blueprintable)
class DMV_INTERACTSYSTEM_API UDMV_InteractFunctionality_Base : public UObject
{
	GENERATED_BODY()

public:
	/** Identifies this functionality so an interactor can block it by tag regardless of this
	 *  functionality's own ActivationRequiredTags/ActivationBlockedTags - see
	 *  UDMV_InteractorComponent::BlockFunctionalityTag. Optional - leave unset if nothing needs to
	 *  block this functionality by category. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Functionality")
	FGameplayTag FunctionalityTag;

	/** The interactor must currently own every one of these tags for this functionality to
	 *  execute - mirrors a GameplayAbility's ActivationRequiredTags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Functionality|Tags")
	FGameplayTagContainer ActivationRequiredTags;

	/** The interactor must currently own none of these tags for this functionality to execute -
	 *  mirrors a GameplayAbility's ActivationBlockedTags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Functionality|Tags")
	FGameplayTagContainer ActivationBlockedTags;

	/** Checks ActivationRequiredTags/ActivationBlockedTags against the interactor's owned tags, and
	 *  FunctionalityTag against the interactor's blocked-functionality tags (see
	 *  UDMV_InteractorComponent) - an interactor with no UDMV_InteractorComponent is treated as
	 *  owning no tags and blocking nothing. */
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	bool CanExecute(AActor* Interactor) const;

	/** The actual per-functionality behavior - override in a Blueprint (or native) subclass. Called
	 *  once CanExecute has already been confirmed true; does not re-check it itself. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Functionality")
	void ExecuteFunctionality(AActor* Interactor);
	virtual void ExecuteFunctionality_Implementation(AActor* Interactor);
};
