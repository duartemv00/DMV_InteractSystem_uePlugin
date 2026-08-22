// Fill out your copyright notice in the Description page of Project Settings.


#include "DMV_InteractComponent.h"


UDMV_InteractComponent::UDMV_InteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}



void UDMV_InteractComponent::BeginPlay()
{
	Super::BeginPlay();

}

bool UDMV_InteractComponent::TriggerFunctionalities(AActor* Interactor)
{
	bool bAnyExecuted = false;
	for (UDMV_InteractFunctionality_Base* Functionality : Functionalities)
	{
		if (IsValid(Functionality) && Functionality->CanExecute(Interactor))
		{
			Functionality->ExecuteFunctionality(Interactor);
			bAnyExecuted = true;
		}
	}
	return bAnyExecuted;
}
