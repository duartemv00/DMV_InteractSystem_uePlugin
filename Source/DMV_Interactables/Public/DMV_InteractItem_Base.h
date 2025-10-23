// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMVTargetComponent.h"
#include "DMV_ItemData_Base.h"
#include "InteractInterface.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/Actor.h"
#include "DMV_InteractItem_Base.generated.h"

UCLASS()
class DMV_INTERACTABLES_API ADMV_InteractItem_Base : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	UBillboardComponent* InnerInteractIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	UBillboardComponent* OuterInteractIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UDMVTargetComponent* TargetComponent;

	ADMV_InteractItem_Base();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDMV_ItemData_Base* Item_Data;
};
