// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE()
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class DMV_INTERACTABLES_API IInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interaction")
	void InRange();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interaction")
	void Hover(bool bGood);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interaction")
	void UnHover();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interaction")
	void RemoteActivation(bool Activate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interaction")
	bool Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Interaction")
	void Inspect(AActor* Interactor, UStaticMesh* Item, FName ItemName, FName ItemDescription);
};
