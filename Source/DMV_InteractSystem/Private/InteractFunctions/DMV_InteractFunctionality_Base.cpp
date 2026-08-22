// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractFunctions/DMV_InteractFunctionality_Base.h"
#include "Components/DMV_InteractorComponent.h"
#include "GameFramework/Actor.h"

bool UDMV_InteractFunctionality_Base::CanExecute(AActor* Interactor) const
{
	if (!IsValid(Interactor))
	{
		return false;
	}

	const UDMV_InteractorComponent* InteractorComponent = Interactor->FindComponentByClass<UDMV_InteractorComponent>();
	if (!InteractorComponent)
	{
		return ActivationRequiredTags.IsEmpty();
	}

	if (!InteractorComponent->GetOwnedTags().HasAll(ActivationRequiredTags))
	{
		return false;
	}

	if (InteractorComponent->GetOwnedTags().HasAny(ActivationBlockedTags))
	{
		return false;
	}

	if (FunctionalityTag.IsValid() && InteractorComponent->IsFunctionalityTagBlocked(FunctionalityTag))
	{
		return false;
	}

	return true;
}

void UDMV_InteractFunctionality_Base::ExecuteFunctionality_Implementation(AActor* Interactor)
{
}
