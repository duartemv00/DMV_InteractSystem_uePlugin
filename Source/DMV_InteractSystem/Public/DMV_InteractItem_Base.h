// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMV_ItemData_Base.h"
#include "Components/DMV_InteractComponent.h"
#include "DMV_InteractInterface.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/Actor.h"
#include "DMV_InteractItem_Base.generated.h"

UCLASS()
class DMV_INTERACTSYSTEM_API ADMV_InteractItem_Base : public AActor, public IDMV_InteractInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	UDMV_InteractComponent* InteractComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	UBillboardComponent* InnerInteractIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	UBillboardComponent* OuterInteractIcon;

	ADMV_InteractItem_Base();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDMV_ItemData_Base* Item_Data;

	UPROPERTY()
	int32 InteractStep;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<AActor*> ListensTo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<AActor*> Activates;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void IncreateStep();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	int32 GetCurrentStep();

	//~ Begin IDMV_InteractInterface interface
	/** Forwards straight to InteractComponent->TriggerFunctionalities - this actor is just a
	 *  visual wrapper, the actual behavior lives in the component's Functionalities. */
	virtual bool Interact_Implementation(AActor* Interactor) override;
	//~ End IDMV_InteractInterface interface
};
