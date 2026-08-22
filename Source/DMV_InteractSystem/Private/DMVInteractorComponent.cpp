// Fill out your copyright notice in the Description page of Project Settings.

#include "DMVInteractorComponent.h"
#include "InteractInterface.h"

UDMVInteractorComponent::UDMVInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDMVInteractorComponent::SetCurrentInteractable(AActor* NewInteractable)
{
	CurrentInteractable = NewInteractable;
}

AActor* UDMVInteractorComponent::GetCurrentInteractable() const
{
	return CurrentInteractable.Get();
}

bool UDMVInteractorComponent::TryInteract()
{
	AActor* Target = CurrentInteractable.Get();
	if (!IsValid(Target) || !Target->Implements<UInteractInterface>())
	{
		return false;
	}

	return IInteractInterface::Execute_Interact(Target, GetOwner());
}
