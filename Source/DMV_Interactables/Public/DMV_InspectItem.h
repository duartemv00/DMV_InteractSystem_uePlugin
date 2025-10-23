// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "DMV_InspectItem.generated.h"

UCLASS()
class DMV_INTERACTABLES_API ADMV_InspectItem : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* CaptureComponent2D;
	
	ADMV_InspectItem();

	virtual void Inspect_Implementation(AActor* Interactor, UStaticMesh* Item, FName ItemName, FName ItemDescription) override;
	
};
