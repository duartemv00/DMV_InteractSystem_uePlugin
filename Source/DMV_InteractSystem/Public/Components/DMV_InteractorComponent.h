// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DMV_InteractorComponent.generated.h"

/**
 * Attach to anything that should be able to perform interactions - typically a player pawn or
 * controller. Deliberately has no idea how a target was found: DMV_InteractSystem has no
 * dependency on any targeting/detection system by design (see the plugin README). Whatever
 * mechanism a project uses to decide "what should the player currently interact with" (a
 * targeting plugin, a line trace, an overlap, anything) is expected to call
 * SetCurrentInteractable() whenever that answer changes - this component only ever reacts to
 * that, it never resolves a target on its own.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DMV_INTERACTSYSTEM_API UDMV_InteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDMV_InteractorComponent();

	/** Sets which actor this interactor should try to interact with next. Pass nullptr to clear it
	 *  (e.g. the target left range/is no longer valid). Purely a passive store - this component
	 *  never resolves a target on its own, whatever set it is responsible for clearing it too. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void SetCurrentInteractable(AActor* NewInteractable);

	/** The actor this interactor would currently try to interact with, if any. */
	UFUNCTION(BlueprintPure, Category = "Interact")
	AActor* GetCurrentInteractable() const;

	/** Call this from wherever "the player pressed interact" is decided - a raw input handler, a
	 *  GameplayAbility's ActivateAbility, anything. Trigger-agnostic by design: this component
	 *  doesn't care what caused the attempt, only whether there's currently a valid target that
	 *  implements IDMV_InteractInterface. Returns false if there's no current target, the target
	 *  no longer implements IDMV_InteractInterface, or the target's own Interact() returns false. */
	UFUNCTION(BlueprintCallable, Category = "Interact")
	bool TryInteract();

	// TAGS - GAS-style gating without any dependency on the Gameplay Ability System (see the
	// plugin README's Responsibilities section). OwnedTags is this interactor's own analogue of an
	// AbilitySystemComponent's owned tags, checked by UDMV_InteractFunctionality_Base against its
	// ActivationRequiredTags/ActivationBlockedTags. BlockedFunctionalityTags mirrors
	// UAbilitySystemComponent::BlockAbilitiesWithTag - blocking every functionality whose own
	// FunctionalityTag matches, regardless of that functionality's own tag requirements.

	/** Adds a tag this interactor currently owns (e.g. "State.Stunned", "State.InDialogue"). */
	UFUNCTION(BlueprintCallable, Category = "Interact|Tags")
	void AddOwnedTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "Interact|Tags")
	void RemoveOwnedTag(FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "Interact|Tags")
	const FGameplayTagContainer& GetOwnedTags() const { return OwnedTags; }

	/** Blocks every functionality whose FunctionalityTag matches Tag from executing for this
	 *  interactor, regardless of that functionality's own ActivationRequiredTags/
	 *  ActivationBlockedTags. */
	UFUNCTION(BlueprintCallable, Category = "Interact|Tags")
	void BlockFunctionalityTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "Interact|Tags")
	void UnblockFunctionalityTag(FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "Interact|Tags")
	bool IsFunctionalityTagBlocked(FGameplayTag Tag) const;

private:
	TWeakObjectPtr<AActor> CurrentInteractable;

	UPROPERTY()
	FGameplayTagContainer OwnedTags;

	UPROPERTY()
	FGameplayTagContainer BlockedFunctionalityTags;
};
