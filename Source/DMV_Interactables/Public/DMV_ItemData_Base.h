// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DMV_InteractAbility_Base.h"
#include "NiagaraSystem.h"
#include "Engine/DataAsset.h"
#include "DMV_ItemData_Base.generated.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class EWeight : uint8
{
	Light, // Equipping it will diminish player movement
	Medium, // Equipping it don't affect the player movements
	Heavy // Equipping it don't affect the player movements
};

USTRUCT(BlueprintType)
struct FItemData_Basics
{
	GENERATED_BODY()

	/** Used for other objects to recognize the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ItemTag;

	/** Visualization in world */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* StaticMesh;
	
	/** Used for UI tooltips */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemName;

	/** Used for UI tooltips */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemDescription;

	/** VFX when the item is just on the world */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* IDLE_VFX;

	/** Animation when the item is just on the world */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationAsset* IDLE_Anim;

	/** If the item is affected by physics */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bPhysicsOn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector IconLocation;

	/** The weight decides the movement speed of the player with the item equiped */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement",
	// 	meta=(EditConditionHides = "bMovable == true"))
	// EWeight Weight;

	// LOGIC
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Basic|Interact|Visuals")
	// TArray<UDMV_InteractAbility_Base*> InitialLogic;
};

UENUM(BlueprintType)
enum class EWhen : uint8
{
	No,
	OnStepStart,
	OnLoop,
	OnStepEnd
};

USTRUCT(BlueprintType)
struct FAttributeReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	EWhen When = EWhen::No;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	TMap<FGameplayTag, float> Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	bool bRandom;
};

USTRUCT(BlueprintType)
struct FTagReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	EWhen When = EWhen::No;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	TArray<FGameplayTag> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	bool bRandom;
};

USTRUCT(BlueprintType)
struct FClassReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	EWhen When = EWhen::No;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards|Actor Classes")
	TArray<TSubclassOf<AActor>> ItemActorsClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	bool bRandom;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT INTERACT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FInteractableData_InputInteraction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputInteraction")
	bool bActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description; 

	// PROMPT VISUALS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Prompt|Visuals")
	UNiagaraSystem* Prompt_VFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Prompt|Visuals")
	UUserWidget* Prompt_Widget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Prompt|Visuals")
	USoundCue* Prompt_Sound;
	
	// INTERACT VISUALS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	UNiagaraSystem* Interact_VFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	UUserWidget* Interact_Widget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	USoundCue* Interact_Sound;
};

USTRUCT(BlueprintType)
struct FInteractionStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDeactivateInputInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDeactivateDamageInteraction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDeactivateProximityInteraction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWhen MarkItemAsCompleted = EWhen::No;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWhen Destroy = EWhen::No;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta=(EditConditionHides = "bDestroyAfterUse = true"))
	float DestroyDelay;

	// PENALTIES
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer AbilitiesToDisable;

	// COST
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, bool> Requisites;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, float> AttributeCost;

	// LOGIC I
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly)
	TArray<UDMV_InteractAbility_Base*> StepLogic_Start;
	
	// TIME
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StepDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool SaveProgress;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool KeepPressing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Time",
		meta=(EditConditionHides = "bKeepPressed == true"))
	UUserWidget* InteractTimeWidget;

	// MINIGAME

	// LOGIC II
	/**
	 * Logic that will be triggered at the end of the step
	 * Will only be triggered if the StepDuration > 0
	 */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly)
	TArray<UDMV_InteractAbility_Base*> StepLogic_End;

	// EQUIP //
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	EWhen Equip = EWhen::No;

	// MELEE //
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Melee",
		meta=(EditConditionHides = "bMovable == true"))
	bool bCanBeUsedAsMelee = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Melee",
		meta=(EditConditionHides = "bCanBeUsedAsMelee == true"))
	float MeleeEffectRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Melee",
		meta=(EditConditionHides = "bCanBeUsedAsMelee == true"))
	int NumberOfEnemiesAffectedByMelee;

	/** How far the item will move the enemies */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Melee",
		meta=(EditConditionHides = "bCanBeUsedAsMelee == true"))
	float Strength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Melee",
		meta=(EditConditionHides = "bCanBeUsedAsMelee == true"))
	bool bCanStun = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Melee",
		meta=(EditConditionHides = "bCanStun == true"))
	float StunTime;

	// REWARDS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Rewards")
	TArray<FAttributeReward> Attribute_Reward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Rewards")
	TArray<FTagReward> Tag_Reward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact Rewards")
	TArray<FClassReward> Class_Reward;

	// FEEDBACK
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* VFX_Start;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* VFX_Loop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* VFX_End;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationAsset* Anim_Start;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationAsset* Anim_Loop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationAsset* Anim_End;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationAsset* Anim_Player_Start;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationAsset* Anim_Player_Loop;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationAsset* Anim_Player_End;
	
};

USTRUCT(BlueprintType)
struct FInteractableData_StepsList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	TArray<FInteractionStep> InteractionCountAndLogic;

	/** If you want to trigger the logic again everytime */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	bool Loop;

	/** If you want to skip any step when you loop */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic|Interact|Visuals")
	int LoopFrom;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DAMAGE INTERACT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class EHealthType : uint8
{
	HealthPoints,
	HitPoints
};

USTRUCT(BlueprintType)
struct FInteractableData_DamageInteract
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputInteraction")
	bool bActive;
};

USTRUCT(BlueprintType)
struct FInteractableData_Health
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	EHealthType HealthType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float HealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	TMap<float, FInteractionStep> LogicByHealthPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	int HitPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	TMap<int, FInteractionStep> LogicByHitPoints;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROXIMITY INTERACT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FNearbyActorClass
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputInteraction")
	TSubclassOf<AActor> Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer ActorState;
};

USTRUCT(BlueprintType)
struct FProximityStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FNearbyActorClass> InteractorActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSave;

	
	
};


USTRUCT(BlueprintType)
struct FInteractableData_ProximityInteract
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FProximityStep> Steps;
	
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Basic data of the Interactable Items, not related with interaction itself but with the visuals
 */
UCLASS()
class DMV_INTERACTABLES_API UDMV_ItemData_Base : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FItemData_Basics ItemBasicData;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Data related with input interaction.
 */
UCLASS()
class DMV_INTERACTABLES_API UXM_InputInteraction_Data : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FInteractableData_InputInteraction InputInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FInteractableData_StepsList StepList;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Data related with Damage Interaction.
 */
UCLASS()
class DMV_INTERACTABLES_API UXM_DamageInteraction_Data : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FInteractableData_DamageInteract DamageInteraction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FInteractableData_Health HealthData;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Data related with Proximity Interaction.
 */
UCLASS()
class DMV_INTERACTABLES_API UXM_ProximityInteraction_Data : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FInteractableData_ProximityInteract ProximityInteraction;
};