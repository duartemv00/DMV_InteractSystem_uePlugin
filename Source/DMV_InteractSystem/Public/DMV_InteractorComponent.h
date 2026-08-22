// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

private:
	TWeakObjectPtr<AActor> CurrentInteractable;
};
