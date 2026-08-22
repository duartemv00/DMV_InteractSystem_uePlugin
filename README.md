# DMV_InteractSystem

A plugin for actors the player can interact with: pick up, open, talk to, inspect up close, or
trigger through proximity or by damaging them. Unlike `DMV_Traps` (Blueprint-first, thin C++),
this plugin's data model and actor scaffolding are real C++ - `ADMV_InteractItem_Base`,
`IInteractInterface`, and a large `UDataAsset` hierarchy describing multi-step interaction
sequences. It depends on `DMV_TargetSystem` for detection: `ADMV_InteractItem_Base` owns a
`UDMVTargetComponent`, matching the worked example in `DMV_TargetSystem`'s own README
(`ID.TargetGroup.CanInteract`/`Interact`).

**Status:** early. The C++ scaffolding (actor construction, component wiring, interface
declarations) is in place, but almost every function body is an empty stub -
`UInteractComponent::BeginPlay` does nothing beyond `Super::BeginPlay()`,
`UDMV_InteractAbility_Base::ExecuteCustomLogic_Implementation` is empty, and
`ADMV_InteractItem_Base` implements `IInteractInterface` but overrides none of its functions in
C++ (that's presumably left to Blueprint subclasses - `BP_Door`/`BP_2Door`/`BP_InteractItem_Base`
do reference `Interact`/`Hover`/`InRange`/`Inspect`; `BP_NPC` and `BP_InteractFeatures_Carry`
reference none of them, so are likely placeholders same as some `DMV_Traps` types). The plugin was
recently renamed from `DMV_Interactables` (see the `CoreRedirects` in
`Config/DefaultDMV_InteractSystem.ini`) - some classes still carry the old `XM_` prefix from before
that rename (see [Known gaps](#known-gaps--open-design-questions)).

**Methodology note:** as with `DMV_Traps`, specific claims about Blueprint content
(`BP_Door`, `BP_NPC`, etc.) come from static string-scanning `.uasset` files, not reading full
graphs - a strong signal, not a certainty.

## Design intent

An interactable is an `ADMV_InteractItem_Base` (or a Blueprint subclass of it) driven by a
`UDMV_ItemData_Base` data asset that describes *everything about how it behaves* - separately from
the actor's own C++/Blueprint code, so a designer can build new interactables mostly by authoring
data assets rather than new classes. `DMV_TargetSystem` handles "is the player looking at /
in range of this" (via the `TargetComponent` every interactable owns); this plugin handles
everything downstream of that: what happens when the player actually interacts, and what other
things (proximity, damage) can trigger the same interaction machinery besides direct input.

An interaction plays out as a sequence of **steps** (`FInteractionStep`), each with its own
duration, cost/requisites (by `FGameplayTag`), rewards, feedback (VFX/anim/sound), and two logic
hooks (`StepLogic_Start`/`StepLogic_End`, arrays of `UDMV_InteractAbility_Base` instances) a
designer can use to run custom per-step behavior without touching the base actor's code - the same
per-instance-configurable-instanced-object pattern `DMV_TargetSystem`'s Filters use.

## Core concepts

| Concept | Class | Role |
|---|---|---|
| Interact interface | `IInteractInterface` (C++) | Declares the full interaction contract: `InRange`, `Hover(bool bGood)`, `UnHover`, `RemoteActivation(bool)`, `Interact(AActor*) -> bool`, `Inspect(AActor*, UStaticMesh*, FName, FName)`. All `BlueprintNativeEvent`s - default bodies are empty, subclasses override the ones they need. |
| Interactable actor | `ADMV_InteractItem_Base` | The base actor: owns `Mesh`, inner/outer interact-prompt billboards, a `TargetComponent` (`DMV_TargetSystem`), an `InteractComponent`, and a `UDMV_ItemData_Base` reference. Tracks `InteractStep` (which step of the sequence it's currently on) and `ListensTo`/`Activates` actor arrays (for interactables that chain/gate each other - not yet implemented beyond the data fields). |
| Interact component | `UInteractComponent` | Currently an empty scaffold (`BeginPlay` does nothing beyond `Super`) - presumably meant to hold future shared interact-side logic, distinct from the `TargetComponent`'s job of detection. |
| Per-step logic hook | `UDMV_InteractAbility_Base` | `EditInlineNew`/`Blueprintable` `UObject` - not a `UGameplayAbility`, and deliberately so (see [Responsibilities](#responsibilities-what-belongs-in-this-plugin)) - with one function, `ExecuteCustomLogic(ADMV_InteractItem_Base*)`. A per-instance-configurable instanced object, same pattern as `DMV_TargetSystem`'s filters. |
| Item data | `UDMV_ItemData_Base` (+ `UXM_InputInteraction_Data`, `UXM_DamageInteraction_Data`, `UXM_ProximityInteraction_Data`) | `UDataAsset`s holding the data described in [Design intent](#design-intent) - see [Known gaps](#known-gaps--open-design-questions) for the `XM_` naming leftover. |
| Inspect | `ADMV_InspectItem` | A separate interactable that implements only `Inspect` - swaps its mesh to the inspected item and presumably feeds a `SceneCaptureComponent2D` to `WBP_Inspect`/`RT_Inspect` for an up-close rotate-and-view UI. |

## Responsibilities (what belongs in this plugin)

Established explicitly so scope doesn't drift as the plugin grows:

- **Owns:** the interactable actor/data model itself, the multi-step sequence machinery, all of
  input/proximity/damage as *trigger types* for that machinery, reward *granting* (applying the
  attribute/tag/class rewards a step defines), and the Inspect feature.
- **Does not own combat resolution.** An item step can *want* to cause a melee-style effect
  (knockback, stun, damage to nearby enemies), but actually resolving that effect is the main
  game's combat system's job (`ALBP_EnemyMelee`, weapon code), not this plugin's. A step's melee
  intent should reach the game through the same `StepLogic_Start`/`StepLogic_End` hook mechanism
  every other per-step behavior uses - see [Known gaps](#known-gaps--open-design-questions), since
  `FInteractionStep` currently bakes melee parameters (`MeleeEffectRange`,
  `NumberOfEnemiesAffectedByMelee`, `Strength`, `bCanStun`, `StunTime`) directly into its own
  fields instead.
- **Damage/health-triggered steps are this plugin's own concept, deliberately separate from
  `DMV_Traps`.** `DMV_Traps`' `IDMVTrapDamageInterface` is about a trap *dealing* damage to
  something else, authoritatively; `FInteractableData_DamageInteract`/`FInteractableData_Health`
  here are about an interactable *reacting to receiving* damage as one of several possible
  triggers for its own step sequence (alongside input and proximity) - a breakable object, an
  NPC/mini-boss-like interactable with HP thresholds that advance its steps. There's no shared
  code between the two today, and none is expected - they solve different problems that happen to
  both involve the word "damage."
- **Reward granting is this plugin's job**, not just a hook: `FAttributeReward`/`FTagReward`/
  `FClassReward` are meant to be actually applied by this plugin when a step completes, not merely
  reported for the game to act on. This does mean the plugin needs *some* concrete way to apply an
  "attribute" identified by a `FGameplayTag` - see the GAS note immediately below for why that
  can't be a `UAttributeSet`/`UGameplayEffect`; resolving the actual mechanism is unresolved, see
  Known gaps.
- **`DMV_TargetSystem` is a hard dependency** for detection/targeting - this plugin doesn't
  reimplement "what is the player currently looking at."
- **No direct dependency on the Gameplay Ability System.** No `UGameplayAbility`,
  `UAbilitySystemComponent`, `UGameplayEffect`, or `UAttributeSet` anywhere in this plugin, and
  `DMV_InteractSystem.Build.cs` never depends on the `GameplayAbilities` module - only
  `GameplayTags` (tag identifiers, not the ability/effect runtime - a separate module, fine to keep
  using). `UDMV_InteractAbility_Base` is a plain `UObject` despite the name, not a
  `UGameplayAbility` - a deliberate choice, not an oversight. If a consuming project happens to use
  GAS, that integration is the project's job at the boundary (e.g. a `UDMV_InteractAbility_Base`
  subclass the project authors that talks to its own `UAbilitySystemComponent`) - this plugin
  itself stays usable in non-GAS projects too. This also means reward granting (above) can't
  resolve "apply an attribute" through GAS - it needs its own, GAS-independent mechanism.

## Known gaps / open design questions

- **Nearly all C++ behavior is an empty stub.** `UInteractComponent::BeginPlay`,
  `UDMV_InteractAbility_Base::ExecuteCustomLogic_Implementation`, and `ADMV_InteractItem_Base`'s
  (non-)override of every `IInteractInterface` function are all either empty or absent. The step
  sequence, reward granting, and every trigger type described above exist only as data structures
  today, not as running logic.
- **Melee fields are baked directly into `FInteractionStep`**, which conflicts with the "combat
  resolution is out of scope" decision above - `MeleeEffectRange`/`NumberOfEnemiesAffectedByMelee`/
  `Strength`/`bCanStun`/`StunTime` should likely move behind the same `StepLogic_Start`/
  `StepLogic_End` hook mechanism (a project-specific `UDMV_InteractAbility_Base` subclass) instead
  of being dedicated struct fields the plugin itself would have to interpret.
- **`BPI_Interact` (a Blueprint Interface in Content) vs. `IInteractInterface` (the C++
  interface)** - unclear from static inspection whether these are the same contract duplicated, or
  two genuinely different interfaces for different purposes (e.g. one for the item, one for
  whatever the player/controller side implements). Needs clarifying before more content picks one.
- **Leftover `XM_` naming from the pre-rename plugin** (`DMV_Interactables` -> `DMV_InteractSystem`,
  per the `CoreRedirects` in `Config/DefaultDMV_InteractSystem.ini`): `UXM_InputInteraction_Data`,
  `UXM_DamageInteraction_Data`, and `UXM_ProximityInteraction_Data` never got renamed to match
  `UDMV_ItemData_Base` and the rest of the `DMV_`-prefixed classes.
- **`BP_NPC` and `BP_InteractFeatures_Carry` reference none of `IInteractInterface`'s functions** -
  likely placeholders, same status as some `DMV_Traps` trap types.
- **A minigame is referenced only in a comment** (`FInteractionStep`: `// MINIGAME`, no fields
  under it) - a step type mentioned as planned but with zero data model or logic behind it yet.
