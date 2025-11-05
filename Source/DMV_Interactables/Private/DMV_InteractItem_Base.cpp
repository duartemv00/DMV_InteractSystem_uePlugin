// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/DMV_InteractItem_Base.h"

ADMV_InteractItem_Base::ADMV_InteractItem_Base()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	InnerInteractIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("InnerInteractIcon"));
	InnerInteractIcon->SetupAttachment(RootComponent);
	
	OuterInteractIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("OuterInteractIcon"));
	OuterInteractIcon->SetupAttachment(RootComponent);
	
	TargetComponent = CreateDefaultSubobject<UDMVTargetComponent>(TEXT("TargetComponent"));
	//TargetComponent->SetupAttachment(RootComponent);

	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	
	if (Item_Data)
	{
		OuterInteractIcon->SetRelativeLocation(Item_Data->ItemBasicData.IconLocation);
		InnerInteractIcon->SetRelativeLocation(Item_Data->ItemBasicData.IconLocation);
		Mesh->SetStaticMesh(Item_Data->ItemBasicData.StaticMesh);
	}
}

void ADMV_InteractItem_Base::IncreateStep()
{
	InteractStep++;
}

int32 ADMV_InteractItem_Base::GetCurrentStep()
{
	return InteractStep;
}

