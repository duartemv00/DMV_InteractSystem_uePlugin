// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/DMV_InteractorComponent.h"
#include "DMV_InteractInterface.h"

UDMV_InteractorComponent::UDMV_InteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDMV_InteractorComponent::SetCurrentInteractable(AActor* NewInteractable)
{
	CurrentInteractable = NewInteractable;
}

AActor* UDMV_InteractorComponent::GetCurrentInteractable() const
{
	return CurrentInteractable.Get();
}

bool UDMV_InteractorComponent::TryInteract()
{
	AActor* Target = CurrentInteractable.Get();
	if (!IsValid(Target) || !Target->Implements<UDMV_InteractInterface>())
	{
		return false;
	}

	return IDMV_InteractInterface::Execute_Interact(Target, GetOwner());
}

void UDMV_InteractorComponent::AddOwnedTag(FGameplayTag Tag)
{
	OwnedTags.AddTag(Tag);
}

void UDMV_InteractorComponent::RemoveOwnedTag(FGameplayTag Tag)
{
	OwnedTags.RemoveTag(Tag);
}

void UDMV_InteractorComponent::BlockFunctionalityTag(FGameplayTag Tag)
{
	BlockedFunctionalityTags.AddTag(Tag);
}

void UDMV_InteractorComponent::UnblockFunctionalityTag(FGameplayTag Tag)
{
	BlockedFunctionalityTags.RemoveTag(Tag);
}

bool UDMV_InteractorComponent::IsFunctionalityTagBlocked(FGameplayTag Tag) const
{
	return BlockedFunctionalityTags.HasTag(Tag);
}
