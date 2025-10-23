// Fill out your copyright notice in the Description page of Project Settings.


#include "DMV_InspectItem.h"




ADMV_InspectItem::ADMV_InspectItem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	CaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent2D"));
	CaptureComponent2D->SetupAttachment(RootComponent);
}

void ADMV_InspectItem::Inspect_Implementation(AActor* Interactor, UStaticMesh* Item, FName ItemName,
	FName ItemDescription)
{
	IInteractInterface::Inspect_Implementation(Interactor, Item, ItemName, ItemDescription);

	Mesh->SetStaticMesh(Item);
}

