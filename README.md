# DMV_InteractSystem

A plugin for actors the player can interact with: pick up, open, talk to, inspect up close, or
trigger through proximity or by damaging them. Unlike `DMV_Traps` (Blueprint-first, thin C++),
this plugin's data model and actor scaffolding are real C++ - `ADMV_InteractItem_Base`,
`IDMV_InteractInterface`, `UDMV_InteractorComponent`, and a large `UDataAsset` hierarchy describing
multi-step interaction sequences.

This plugin has **no dependency on any targeting/detection system, including `DMV_TargetSystem`**
- a deliberate choice, not an oversight (see [Responsibilities](#responsibilities-what-belongs-in-this-plugin)).
How a project decides "what should the player interact with" (a targeting plugin, a line trace,
an overlap) is entirely outside this plugin's concern; it only ever reacts to being told the
answer.

**Status:** early. The C++ scaffolding (actor construction, component wiring, interface
declarations) is in place, but almost every function body is an empty stub -
`UDMV_InteractComponent::BeginPlay` does nothing beyond `Super::BeginPlay()`,
`UDMV_InteractAbility_Base::ExecuteCustomLogic_Implementation` is empty, and
`ADMV_InteractItem_Base` implements `IDMV_InteractInterface` but overrides none of its functions in
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
data assets rather than new classes. This plugin has no opinion on "is the player looking at/in
range of this" - it starts from the moment some external system already has an `AActor*` reference
to a potential interactable in hand, however that reference was obtained, and handles everything
from there: what happens when the player actually interacts, and what other things (proximity,
damage) can trigger the same interaction machinery besides direct input.

The player side is `UDMV_InteractorComponent`: a passive holder of "the current interactable"
(`SetCurrentInteractable`/`GetCurrentInteractable`, fed by whatever targeting mechanism the project
uses) plus one trigger-agnostic entry point, `TryInteract()`, that calls `IDMV_InteractInterface::Interact`
on the current target. Anything can call `TryInteract` - a raw input handler, a `UGameplayAbility`'s
`ActivateAbility` - the component doesn't know or care which. That's the whole contract: a project
wires up its own way of finding a target and its own way of triggering an attempt, and this plugin
does the rest.

An interaction plays out as a sequence of **steps** (`FInteractionStep`), each with its own
duration, cost/requisites (by `FGameplayTag`), rewards, feedback (VFX/anim/sound), and two logic
hooks (`StepLogic_Start`/`StepLogic_End`, arrays of `UDMV_InteractAbility_Base` instances) a
designer can use to run custom per-step behavior without touching the base actor's code - the same
per-instance-configurable-instanced-object pattern `DMV_TargetSystem`'s Filters use.

## Core concepts

| Concept | Class | Role |
|---|---|---|
| Interact interface | `IDMV_InteractInterface` (C++) | Declares the full interaction contract: `InRange`, `Hover(bool bGood)`, `UnHover`, `RemoteActivation(bool)`, `Interact(AActor*) -> bool`, `Inspect(AActor*, UStaticMesh*, FName, FName)`. All `BlueprintNativeEvent`s - default bodies are empty, subclasses override the ones they need. |
| Interactable actor | `ADMV_InteractItem_Base` | The base actor: owns `Mesh`, inner/outer interact-prompt billboards, an `InteractComponent`, and a `UDMV_ItemData_Base` reference. Tracks `InteractStep` (which step of the sequence it's currently on) and `ListensTo`/`Activates` actor arrays (for interactables that chain/gate each other - not yet implemented beyond the data fields). Owns no targeting/detection component of any kind - see [Design intent](#design-intent). |
| Interact component (item side) | `UDMV_InteractComponent` | Currently an empty scaffold (`BeginPlay` does nothing beyond `Super`) - presumably meant to hold future shared interact-side logic. Distinct from `UDMV_InteractorComponent` below, which is the *player*-side piece. |
| Interactor component (player side) | `UDMV_InteractorComponent` | Goes on whatever should be able to perform interactions (a player pawn/controller). Holds a passively-set `CurrentInteractable`, and exposes `TryInteract()` as the one trigger-agnostic entry point - see [Design intent](#design-intent). |
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
- **No dependency on any targeting/detection system, `DMV_TargetSystem` included.** Deliberately
  the opposite of an earlier draft of this document, which called `DMV_TargetSystem` a hard
  dependency - reversed because inter-plugin coupling should only exist where 100% necessary, and
  it isn't here. `UDMV_InteractorComponent` only ever receives a target via `SetCurrentInteractable`;
  it never asks any targeting system for one, and this plugin has zero build dependency on
  `DMV_TargetSystem` (removed from `DMV_InteractSystem.Build.cs`, and `ADMV_InteractItem_Base` no
  longer owns a `UDMVTargetComponent`). A project is free to feed `SetCurrentInteractable` from
  `DMV_TargetSystem`, a line trace, or anything else - this plugin doesn't know or care which.
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

- **Most C++ behavior is still an empty stub.** `UDMV_InteractComponent::BeginPlay`,
  `UDMV_InteractAbility_Base::ExecuteCustomLogic_Implementation`, and `ADMV_InteractItem_Base`'s
  (non-)override of every `IDMV_InteractInterface` function are all either empty or absent. The step
  sequence, reward granting, and the proximity/damage trigger types described above exist only as
  data structures today, not as running logic. `UDMV_InteractorComponent`'s activation path
  (`SetCurrentInteractable`/`TryInteract`) is the one piece of this plugin that's actually
  implemented end to end - see Recent history.
- **`UDMV_InteractorComponent::TryInteract` doesn't manage the `Hover`/`UnHover`/`InRange` part of
  `IDMV_InteractInterface`'s contract** - only `Interact` itself. Wiring those up (e.g. calling
  `UnHover`/`InRange` on the old/new target when `SetCurrentInteractable` changes) was deliberately
  left out of the first pass to keep it to exactly what was asked for; a natural next step once
  there's a concrete need for hover-state UI feedback.
- **Melee fields are baked directly into `FInteractionStep`**, which conflicts with the "combat
  resolution is out of scope" decision above - `MeleeEffectRange`/`NumberOfEnemiesAffectedByMelee`/
  `Strength`/`bCanStun`/`StunTime` should likely move behind the same `StepLogic_Start`/
  `StepLogic_End` hook mechanism (a project-specific `UDMV_InteractAbility_Base` subclass) instead
  of being dedicated struct fields the plugin itself would have to interpret.
- **`BPI_Interact` (a Blueprint Interface in Content) vs. `IDMV_InteractInterface` (the C++
  interface)** - unclear from static inspection whether these are the same contract duplicated, or
  two genuinely different interfaces for different purposes (e.g. one for the item, one for
  whatever the player/controller side implements). Needs clarifying before more content picks one.
- **Leftover `XM_` naming from the pre-rename plugin** (`DMV_Interactables` -> `DMV_InteractSystem`,
  per the `CoreRedirects` in `Config/DefaultDMV_InteractSystem.ini`): `UXM_InputInteraction_Data`,
  `UXM_DamageInteraction_Data`, and `UXM_ProximityInteraction_Data` never got renamed to match
  `UDMV_ItemData_Base` and the rest of the `DMV_`-prefixed classes.
- **`BP_NPC` and `BP_InteractFeatures_Carry` reference none of `IDMV_InteractInterface`'s functions** -
  likely placeholders, same status as some `DMV_Traps` trap types.
- **A minigame is referenced only in a comment** (`FInteractionStep`: `// MINIGAME`, no fields
  under it) - a step type mentioned as planned but with zero data model or logic behind it yet.

## Recent history

- Renamed every source file and class in this module to consistently start with `DMV_`:
  `InteractComponent.h/.cpp` -> `DMV_InteractComponent.h/.cpp` (`UInteractComponent` ->
  `UDMV_InteractComponent`), `InteractInterface.h/.cpp` -> `DMV_InteractInterface.h/.cpp`
  (`UInteractInterface`/`IInteractInterface` -> `UDMV_InteractInterface`/`IDMV_InteractInterface`),
  and the newly-added interactor component to `DMV_InteractorComponent.h/.cpp`
  (`UDMV_InteractorComponent`). These two were the only files/classes in the plugin without the
  prefix; everything else (`DMV_InteractItem_Base`, `DMV_ItemData_Base`, `DMV_InteractAbility_Base`,
  `DMV_InspectItem`) already followed it. Added `ClassRedirects` for the two renamed classes to
  `Config/DefaultDMV_InteractSystem.ini` so existing Blueprint content referencing them (`BP_Door`,
  `BP_2Door`, `BP_InteractItem_Base`, `BP_NPC`) keeps resolving correctly.
- Added `UDMV_InteractorComponent`, the player-side half of "an actor is referenced and the
  interaction input is pressed" - see [Design intent](#design-intent). It's deliberately minimal:
  a passive `CurrentInteractable` set from outside, and one `TryInteract()` entry point that any
  trigger (input, a `UGameplayAbility`) can call.
- Removed this plugin's dependency on `DMV_TargetSystem` entirely: `ADMV_InteractItem_Base` no
  longer has a `TargetComponent`, and `DMV_TargetSystem` is gone from
  `DMV_InteractSystem.Build.cs`'s `PublicDependencyModuleNames`. This reverses what an earlier
  version of this document called a hard dependency - see the note under
  [Responsibilities](#responsibilities-what-belongs-in-this-plugin) for why. Existing Blueprints
  that inherited the now-removed `TargetComponent` (`BP_Door`, `BP_2Door`, `BP_InteractItem_Base`,
  `BP_NPC`, `BP_InspectExample`) will simply lose that component on next compile - it had no
  callable behavior of its own for any graph to have depended on.
- Declared a plugin-level dependency on Niagara in `DMV_InteractSystem.uplugin` (the module already
  depended on the Niagara module; UBT was warning about the missing plugin-level declaration) - the
  same fix `DMV_TargetSystem` needed for the same reason.
