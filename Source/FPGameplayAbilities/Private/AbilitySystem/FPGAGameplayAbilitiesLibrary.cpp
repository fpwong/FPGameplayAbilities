// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

#include "GameplayEffectAggregator.h"
#include "GameplayEffect.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "GameplayAbilitySet.h"
#include "GameplayAbilitiesModule.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/FPGAGlobalTags.h"
#include "Engine/Engine.h"
#include "FPGameplayAbilities/Targeting/FPGATargetTypes.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilterTaskSet.h"

void UFPGAGameplayAbilitiesLibrary::InitGlobalData()
{
	UAbilitySystemGlobals::Get().InitGlobalData();
}

void UFPGAGameplayAbilitiesLibrary::AddAttributeSet(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UAttributeSet> Attributes, const UDataTable* DataTable, FName GroupName)
{
	if (!AbilitySystem || !Attributes)
		return;

	// Spawn attributes and skip initialization
	AbilitySystem->InitStats(Attributes, nullptr);

	// Get your attribute set
	UAttributeSet* AttributeSet = nullptr;
	for (UAttributeSet* Att : AbilitySystem->GetSpawnedAttributes())
	{
		if (Att->GetClass() == Attributes)
		{
			AttributeSet = Att;
			break;
		}
	}

	// This should never happen
	if (!AttributeSet)
		return;

	if (!DataTable || GroupName.IsNone())
		return;

	// Load data table
	static const FString Context = FString(TEXT("AddAttributeSet"));

	for (TFieldIterator<FProperty> It(AttributeSet->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		FProperty* Property = *It;
		FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property);
		if (NumericProperty)
		{
			FString RowNameStr = FString::Printf(TEXT("%s.%s.%s"), *GroupName.ToString(), *Property->GetOwnerVariant().GetName(), *Property->GetName());

			FAttributeMetaData* MetaData = DataTable->FindRow<FAttributeMetaData>(FName(*RowNameStr), Context, true);
			if (MetaData)
			{
				void* Data = NumericProperty->ContainerPtrToValuePtr<void>(AttributeSet);
				NumericProperty->SetFloatingPointPropertyValue(Data, MetaData->BaseValue);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s: %f"), *RowNameStr, MetaData->BaseValue));

				FGameplayAttribute AttributeToModify(NumericProperty);
				AttributeSet->GetOwningAbilitySystemComponent()->SetNumericAttributeBase(AttributeToModify, MetaData->BaseValue);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s not found"), *RowNameStr));
			}
		}
	}
}

FGameplayAbilitySpecHandle UFPGAGameplayAbilitiesLibrary::GiveAbility(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability)
{
	if (AbilitySystem && AbilitySystem->GetOwner()->HasAuthority() && Ability)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, -1);
		FGameplayAbilitySpecHandle Handle = AbilitySystem->GiveAbility(AbilitySpec);
		return Handle;
	}

	return FGameplayAbilitySpecHandle();
}

FGameplayAbilitySpecHandle UFPGAGameplayAbilitiesLibrary::GiveAbilityAndActivateOnce(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability, const FGameplayEventData& EventData)
{
	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, -1);
	return AbilitySystem->GiveAbilityAndActivateOnce(Spec, &EventData);
	// if (FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(Ability))
	// {
	// 	return AbilitySystem->GiveAbilityAndActivateOnce(*Spec, &EventData);
	// }
	//
	// return FGameplayAbilitySpecHandle();


	// FGameplayAbilitySpec* FoundSpec = AbilitySystem->FindAbilitySpecFromHandle(Ability);
	// if (AbilitySystem && AbilitySystem->GetOwner()->HasAuthority() && Ability)
	// {
	// 	FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, -1);
	//
	// 	Spec.bActivateOnce = true;
	//
	// 	FGameplayAbilitySpecHandle AddedAbilityHandle = AbilitySystem->GiveAbility(Spec);
	//
	// 	AbilitySystem->GiveAbilityAndActivateOnce();
	//
	// 	if (FoundSpec)
	// 	{
	// 		FoundSpec->RemoveAfterActivation = true;
	//
	// 		if (!AbilitySystem->InternalTryActivateAbility(AddedAbilityHandle, FPredictionKey(), nullptr, nullptr, &EventData))
	// 		{
	// 			// We failed to activate it, so remove it now
	// 			AbilitySystem->ClearAbility(AddedAbilityHandle);
	//
	// 			return FGameplayAbilitySpecHandle();
	// 		}
	// 	}
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("Activated ability?"));
	//
	// 	return AddedAbilityHandle;
	// }
	//
	// return FGameplayAbilitySpecHandle();
}

UGameplayAbility* UFPGAGameplayAbilitiesLibrary::FindAbilityFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle Handle)
{
	if (!AbilitySystem)
		return nullptr;

	FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromHandle(Handle);

	if (Spec)
		return Spec->GetPrimaryInstance();

	return nullptr;
}

UGameplayAbility* UFPGAGameplayAbilitiesLibrary::FindInstancedAbilityFromClass(
	UAbilitySystemComponent* AbilitySystem,
	TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(AbilityClass))
	{
		UGameplayAbility* InstancedAbility = Spec->GetPrimaryInstance();
		UGameplayAbility* Ability = InstancedAbility ? InstancedAbility : Spec->Ability;
		return Ability;
	}

	return nullptr;
}

FGameplayAbilitySpec UFPGAGameplayAbilitiesLibrary::FindAbilitySpecFromClass(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (!AbilitySystem)
		return nullptr;

	FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(InAbilityClass);

	return (Spec != nullptr) ? *Spec : nullptr;
}

FGameplayAbilitySpec UFPGAGameplayAbilitiesLibrary::FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle Handle)
{
	if (!AbilitySystem)
		return nullptr;

	FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromHandle(Handle);
	if (Spec)
	{
		return *Spec;
	}

	return nullptr;
}

void UFPGAGameplayAbilitiesLibrary::GetActivatableAbilities(UAbilitySystemComponent* AbilitySystem, TArray<FGameplayAbilitySpec>& ActivatableAbilities)
{
	if (!AbilitySystem)
		return;

	ActivatableAbilities = AbilitySystem->GetActivatableAbilities();
}

UGameplayAbility* UFPGAGameplayAbilitiesLibrary::GetAbilityFromSpec(FGameplayAbilitySpec Spec)
{
	return Spec.Ability;
}

FGameplayAbilitySpecHandle UFPGAGameplayAbilitiesLibrary::GetHandleFromSpec(FGameplayAbilitySpec Spec)
{
	return Spec.Handle;
}

void UFPGAGameplayAbilitiesLibrary::ExecuteGameplayCueForAbilitySystem(UAbilitySystemComponent* AbilitySystem, const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	if (!AbilitySystem)
		return;
	AbilitySystem->ExecuteGameplayCue(GameplayCueTag, GameplayCueParameters);
}

FActiveGameplayEffectHandle UFPGAGameplayAbilitiesLibrary::ApplyGameplayEffect(
	UGameplayEffect* Effect, 
	UAbilitySystemComponent* Source, 
	UAbilitySystemComponent* Target,
	float Level,
	FGameplayEffectContextHandle Context)
{
	if (Source && Target && Effect)
	{
		return Source->ApplyGameplayEffectToTarget(Effect, Target, Level, Context);
	}

	return FActiveGameplayEffectHandle();
}

int32 UFPGAGameplayAbilitiesLibrary::RemoveGameplayEffect(UGameplayEffect* Effect, UAbilitySystemComponent* AbilitySystem, int32 StacksToRemove)
{
	if (!Effect || !AbilitySystem)
	{
		return 0;
	}

	FGameplayEffectQuery Query;
	Query.CustomMatchDelegate.BindLambda([EffectDef = Effect](const FActiveGameplayEffect& CurEffect)
	{
		return CurEffect.Spec.Def && EffectDef == CurEffect.Spec.Def;
	});

	return AbilitySystem->RemoveActiveEffects(Query, StacksToRemove);
}

bool UFPGAGameplayAbilitiesLibrary::TryActivateAbility(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle AbilityToActivate, bool bAllowRemoteActivation)
{
	if (!AbilitySystem)
		return false;
	return AbilitySystem->TryActivateAbility(AbilityToActivate, bAllowRemoteActivation);
}

FGameplayAbilitySpecHandle UFPGAGameplayAbilitiesLibrary::BP_TryActivateAbilityWithEvent(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData EventData)
{
	if (UGameplayAbility* Ability = ActivateAbilityWithEvent(AbilitySystem, AbilityClass, EventData))
	{
		return Ability->GetCurrentAbilitySpecHandle();
	}

	return FGameplayAbilitySpecHandle();
}

UGameplayAbility* UFPGAGameplayAbilitiesLibrary::ActivateAbilityWithEvent(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData EventData, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate)
{
	if (!AbilitySystem)
	{
		return nullptr;
	}

	// if (AActor* TargetActor = GetFirstActorFromTargetData(EventData.TargetData))
	// {
	// 	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	// 	{
	// 		TargetAbilitySystem->GetOwnedGameplayTags(EventData.TargetTags);
	// 		FillRelationshipTags(EventData.TargetTags, AbilitySystem->GetOwnerActor(), TargetActor);
	// 	}
	// }

	if (FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(AbilityClass))
	{
		return ActivateAbilityWithEvent(AbilitySystem, Spec, EventData.TargetData, OnGameplayAbilityEndedDelegate);
		// FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		// const FGameplayTag& EventTag = EventData.EventTag.IsValid() ? EventData.EventTag : FFPGAGlobalTags::Get().Misc_Dummy;
		// AbilitySystem->TriggerAbilityFromGameplayEvent(Spec->Handle, AbilitySystem->AbilityActorInfo.Get(), EventTag, &EventData, *AbilitySystem);
		//
		// UGameplayAbility* InstancedAbility = Spec->GetPrimaryInstance();
		// UGameplayAbility* Ability = InstancedAbility ? InstancedAbility : Spec->Ability;
		// if (Ability)
		// {
		// 	if (OnGameplayAbilityEndedDelegate)
		// 	{
		// 		Ability->OnGameplayAbilityEnded.Add(*OnGameplayAbilityEndedDelegate);
		// 		return Ability;
		// 	}
		// }
	}

	return nullptr;
}

UGameplayAbility* UFPGAGameplayAbilitiesLibrary::ActivateAbilityWithEvent(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpec* Spec, FGameplayAbilityTargetDataHandle TargetData, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate)
{
	FGameplayEventData EventData;
	EventData.TargetData = TargetData;
	return ActivateAbilityWithEvent(AbilitySystem, Spec, EventData, OnGameplayAbilityEndedDelegate);
}

UGameplayAbility* UFPGAGameplayAbilitiesLibrary::ActivateAbilityWithEvent(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpec* Spec, FGameplayEventData EventData, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate)
{
	if (!AbilitySystem || !Spec)
	{
		return nullptr;
	}

	if (AActor* TargetActor = GetFirstActorFromTargetData(EventData.TargetData))
	{
		if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			TargetAbilitySystem->GetOwnedGameplayTags(EventData.TargetTags);
			FillRelationshipTags(EventData.TargetTags, AbilitySystem->GetOwnerActor(), TargetActor);
		}
	}

	FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
	const FGameplayTag& EventTag = EventData.EventTag.IsValid() ? EventData.EventTag : FFPGAGlobalTags::Get().Misc_Dummy;
	if (AbilitySystem->TriggerAbilityFromGameplayEvent(Spec->Handle, AbilitySystem->AbilityActorInfo.Get(), EventTag, &EventData, *AbilitySystem))
	{
		UGameplayAbility* InstancedAbility = Spec->GetPrimaryInstance();
		UGameplayAbility* Ability = InstancedAbility ? InstancedAbility : Spec->Ability;
		if (Ability)
		{
			if (OnGameplayAbilityEndedDelegate)
			{
				Ability->OnGameplayAbilityEnded.Add(*OnGameplayAbilityEndedDelegate);
				return Ability;
			}
		}
	}

	return nullptr;
}

FGameplayAbilitySpecHandle UFPGAGameplayAbilitiesLibrary::GetAbilitySpecHandle(UGameplayAbility* Ability)
{
	if (!Ability)
	{
		return FGameplayAbilitySpecHandle();
	}
	
	return Ability->GetCurrentAbilitySpecHandle();
}

int32 UFPGAGameplayAbilitiesLibrary::HandleGameplayEvent(UAbilitySystemComponent* AbilitySystem, FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (AbilitySystem)
	{
		FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		return AbilitySystem->HandleGameplayEvent(EventTag, &Payload);
	}
	return int32();
}

bool UFPGAGameplayAbilitiesLibrary::IsAbilityActive(UGameplayAbility* Ability)
{
	if (Ability == nullptr)
		return false;

	return Ability->IsActive();
}

void UFPGAGameplayAbilitiesLibrary::GetCooldownTimeRemainingAndDuration(UGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle, float& TimeRemaining, float& CooldownDuration)
{
	if (!Ability)
		return;
	const FGameplayAbilityActorInfo Info = Ability->GetActorInfo();
	Ability->GetCooldownTimeRemainingAndDuration(Handle, &Info, TimeRemaining, CooldownDuration);
}

void UFPGAGameplayAbilitiesLibrary::GetActiveAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	AbilitySystem->GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			if (ActiveAbility->IsActive())
			{
				ActiveAbilities.Add(Cast<UGameplayAbility>(ActiveAbility));
			}
		}
	}
}

void UFPGAGameplayAbilitiesLibrary::AddLooseGameplayTag(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag, int32 Count)
{
	if (AbilitySystem == nullptr)
		return;

	AbilitySystem->AddLooseGameplayTag(Tag, Count);
}

void UFPGAGameplayAbilitiesLibrary::RemoveLooseGameplayTag(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag, int32 Count)
{
	if (AbilitySystem == nullptr)
		return;

	AbilitySystem->RemoveLooseGameplayTag(Tag, Count);
}

void UFPGAGameplayAbilitiesLibrary::BlockAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& Tags)
{
	if (AbilitySystem)
	{
		AbilitySystem->BlockAbilitiesWithTags(Tags);
	}
}

void UFPGAGameplayAbilitiesLibrary::UnBlockAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& Tags)
{
	if (AbilitySystem)
	{
		AbilitySystem->UnBlockAbilitiesWithTags(Tags);
	}
}

void UFPGAGameplayAbilitiesLibrary::CancelAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, UGameplayAbility* RequestingAbility, const FGameplayTagContainer& Tags)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAbilities(&Tags, nullptr, RequestingAbility);
	}
}

UAttributeSet* UFPGAGameplayAbilitiesLibrary::FindAttributeSetOfClass(UAbilitySystemComponent* AbilitySystem, const TSubclassOf<UAttributeSet> AttributeClass)
{
	for (UAttributeSet* Set : AbilitySystem->GetSpawnedAttributes())
	{
		if (Set && Set->IsA(AttributeClass))
		{
			return Set;
		}
	}
	return nullptr;
}

FGameplayEffectContextHandle UFPGAGameplayAbilitiesLibrary::GetContextFromEffectSpec(const FGameplayEffectSpec& Spec)
{
	return Spec.GetContext();
}

TArray<AActor*> UFPGAGameplayAbilitiesLibrary::EffectContectGetActors(FGameplayEffectContextHandle EffectContext)
{
	TArray<AActor*> Actors;

	for (TWeakObjectPtr<AActor> ActorPtr : EffectContext.GetActors())
	{
		if (ActorPtr.IsValid())
		{
			Actors.Add(ActorPtr.Get());
		}
	}

	return Actors;
}

// CALCULATIONS ----------------------------------------------------

UAbilitySystemComponent* UFPGAGameplayAbilitiesLibrary::GetTargetAbilitySystemComponent(FGameplayEffectCustomExecutionParameters ExecutionParams)
{
	return ExecutionParams.GetTargetAbilitySystemComponent();
}

UAbilitySystemComponent* UFPGAGameplayAbilitiesLibrary::GetSourceAbilitySystemComponent(FGameplayEffectCustomExecutionParameters ExecutionParams)
{
	return ExecutionParams.GetSourceAbilitySystemComponent();
}

const FGameplayEffectSpec UFPGAGameplayAbilitiesLibrary::GetOwningSpec(FGameplayEffectCustomExecutionParameters ExecutionParams)
{
	return ExecutionParams.GetOwningSpec();
}

void UFPGAGameplayAbilitiesLibrary::InitAbilityActorInfo(UAbilitySystemComponent* AbilitySystemComponent, AActor* InOwnerActor, AActor* InAvatarActor)
{
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

//bool UFPGAGameplayAbilitiesLibrary::AttemptCalculateCapturedAttributeMagnitude(FGameplayEffectCustomExecutionParameters ExecutionParams,
//	const FGameplayEffectAttributeCaptureDefinition InCaptureDef,
//	float & OutMagnitude)
//{
//	if (!ExecutionParams.GetTargetAbilitySystemComponent() || InCaptureDef == FGameplayEffectAttributeCaptureDefinition()) return false;
//
//	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
//
//	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
//	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
//	/* Some more helper variables: Spec is the spec this execution originated from, and the Source/TargetTags
//	are pointers to the tags granted to source/target actor, respectively. */
//
//	/* We use these tags to set up an FAggregatorEvaluateParameters struct, which we will need
//	to get the values of our captured attributes later in this function. */
//	FAggregatorEvaluateParameters EvaluationParameters;
//	EvaluationParameters.SourceTags = SourceTags;
//	EvaluationParameters.TargetTags = TargetTags;
//
//	float Out = 0.f;
//	bool Success = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InCaptureDef, EvaluationParameters, Out);
//	OutMagnitude = Out;
//	return Success;
//}

//FGameplayEffectAttributeCaptureDefinition UFPGAGameplayAbilitiesLibrary::GetCaptureFromAttribute(TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture, FGameplayAttribute Attribute)
//{
//	for (FGameplayEffectAttributeCaptureDefinition Capture : RelevantAttributesToCapture)
//		if (Capture.AttributeToCapture == Attribute)
//			return Capture;
//
//	return FGameplayEffectAttributeCaptureDefinition();
//}

//void UFPGAGameplayAbilitiesLibrary::AddOutputModifier(TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture,
//	FGameplayEffectFPGAExecutionOutput& OutExecutionOutput,
//	const FGameplayAttribute InAttribute,
//	TEnumAsByte<EGameplayModOp::Type> InModOp,
//	float InMagnitude)
//{
//	// Find capture def from RelevantAttributesToCapture
//	FGameplayEffectAttributeCaptureDefinition CaptureDef;
//	for (auto CapDef : RelevantAttributesToCapture)
//		if (CapDef.AttributeToCapture == InAttribute)
//			CaptureDef = CapDef;
//
//	if (CaptureDef == FGameplayEffectAttributeCaptureDefinition()) return;
//
//	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(CaptureDef.AttributeToCapture, InModOp, InMagnitude));
//}
//
//bool UFPGAGameplayAbilitiesLibrary::GetCapturedAttributeMagnitude(FGameplayAttribute Attribute,
//	TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture,
//	FGameplayEffectCustomExecutionParameters ExecutionParams,
//	float & OutMagnitude)
//{
//	if (!ExecutionParams.GetTargetAbilitySystemComponent()) return false;
//	FGameplayEffectAttributeCaptureDefinition CaptureDefinition = GetCaptureFromAttribute(RelevantAttributesToCapture, Attribute);
//	return AttemptCalculateCapturedAttributeMagnitude(ExecutionParams, CaptureDefinition, OutMagnitude);
//}

FGameplayTagQuery UFPGAGameplayAbilitiesLibrary::Query_MatchTag(FGameplayTag const& InTag)
{
	return FGameplayTagQuery::MakeQuery_MatchTag(InTag);
}

FGameplayEffectQuery UFPGAGameplayAbilitiesLibrary::Query_MatchAnyOwningTags(const FGameplayTagContainer& InTags)
{
	return FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InTags);
}

UAbilitySystemComponent* UFPGAGameplayAbilitiesLibrary::GetAbilitySystemFromEffectHandle(FActiveGameplayEffectHandle Handle)
{
	return Handle.GetOwningAbilitySystemComponent();
}

class UGameplayEffectUIData* UFPGAGameplayAbilitiesLibrary::GetGameplayEffectUIData(FActiveGameplayEffectHandle Handle)
{
	UAbilitySystemComponent* ASC = Handle.GetOwningAbilitySystemComponent();
	if (ASC != nullptr)
	{
		const UGameplayEffect* Effect = ASC->GetGameplayEffectDefForHandle(Handle);
		if (Effect != nullptr)
		{
			return Effect->UIData;
		}
	}

	return nullptr;
}

class UAbilitySystemComponent* UFPGAGameplayAbilitiesLibrary::GetInstigatorAbilitySystemFromEffectSpec(const FGameplayEffectSpec& Spec)
{
	return Spec.GetContext().GetInstigatorAbilitySystemComponent();
}

bool UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index, FVector& Location)
{
	if (!TargetData.Data.IsValidIndex(Index))
	{
		return false;
	}

	FGameplayAbilityTargetData* Data = TargetData.Data[Index].Get();
	if (!Data)
	{
		return false;
	}

	const FHitResult* HitResultPtr = Data->GetHitResult();
	if (HitResultPtr)
	{
		Location = HitResultPtr->Location;
		return true;
	}

	TArray<TWeakObjectPtr<AActor>> WeakArray = Data->GetActors();
	for (TWeakObjectPtr<AActor>& WeakPtr : WeakArray)
	{
		if (WeakPtr.IsValid())
		{
			Location = WeakPtr->GetActorLocation();
			return true;
		}
		else
		{
			return false;
		}
	}

	if (Data->HasEndPoint())
	{
		Location = Data->GetEndPoint();
		return true;
	}

	return false;
}

AActor* UFPGAGameplayAbilitiesLibrary::GetFirstActorFromTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	for (TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		for (TWeakObjectPtr<AActor> Actor : TargetData->GetActors())
		{
			if (Actor.IsValid())
			{
				return Actor.Get();
			}
		}
	}

	return nullptr;
}

FGameplayAbilityTargetDataHandle UFPGAGameplayAbilitiesLibrary::MakeTargetDataFromLocations(FVector Source, FVector Target)
{
	// Construct TargetData
	FGameplayAbilityTargetData_LocationInfo* NewData = new FGameplayAbilityTargetData_LocationInfo();
	 
	FGameplayAbilityTargetingLocationInfo SourceLocation;
	SourceLocation.LiteralTransform.SetLocation(Source);
	SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	NewData->SourceLocation = SourceLocation;


	FGameplayAbilityTargetingLocationInfo TargetLocation;
	TargetLocation.LiteralTransform.SetLocation(Target);
	TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	NewData->TargetLocation = TargetLocation;

	// Give it a handle and return
	FGameplayAbilityTargetDataHandle Handle;
	Handle.Data.Add(MakeShareable(NewData));
	return Handle;
}

FGameplayAbilityTargetDataHandle UFPGAGameplayAbilitiesLibrary::MakeVectorTargetData(FVector Location)
{
	FGameplayAbilityTargetDataHandle Handle;
	Handle.Data.Add(MakeShareable(FFPGATargetData_Vector::MakeVectorTargetData(Location)));
	return Handle;
}

bool UFPGAGameplayAbilitiesLibrary::CanActivateAbilityFromHandle(
	UAbilitySystemComponent* AbilitySystem,
	FGameplayAbilitySpecHandle AbilityHandle)
{
	if (AbilitySystem == nullptr || !AbilityHandle.IsValid())
		return false;

	UGameplayAbility* Ability = FindAbilityFromHandle(AbilitySystem, AbilityHandle);
	if (Ability == nullptr)
		return false;

	TSharedPtr<FGameplayAbilityActorInfo> ActorInfoSharedPtr = AbilitySystem->AbilityActorInfo;
	if (!ActorInfoSharedPtr.IsValid())
		return false;

	return Ability->CanActivateAbility(AbilityHandle, ActorInfoSharedPtr.Get());
}

bool UFPGAGameplayAbilitiesLibrary::CanActivateAbility(UAbilitySystemComponent* AbilitySystem, UGameplayAbility* Ability)
{
	if (AbilitySystem == nullptr || Ability == nullptr)
		return false;

	TSharedPtr<FGameplayAbilityActorInfo> ActorInfoSharedPtr = AbilitySystem->AbilityActorInfo;
	if (!ActorInfoSharedPtr.IsValid())
		return false;

	FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec();
	if (Spec == nullptr)
		return false;

	return Ability->CanActivateAbility(Spec->Handle, ActorInfoSharedPtr.Get());
}

void UFPGAGameplayAbilitiesLibrary::SetSetByCallerMagnitude(FGameplayEffectSpec& Spec, FGameplayTag DataTag, float Magnitude)
{
	Spec.SetSetByCallerMagnitude(DataTag, Magnitude);
}

FGameplayEffectSpec UFPGAGameplayAbilitiesLibrary::GetEffectSpecFromHandle(FGameplayEffectSpecHandle& Spec)
{
	if (Spec.Data.IsValid())
	{
		return *Spec.Data.Get();
	}

	return FGameplayEffectSpec();
}

int32 UFPGAGameplayAbilitiesLibrary::GetTagCount(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag)
{
	return (AbilitySystem == nullptr) ? -1 : AbilitySystem->GetTagCount(Tag);
}

FName UFPGAGameplayAbilitiesLibrary::GetSimpleTagName(FGameplayTag Tag)
{
	if (TSharedPtr<FGameplayTagNode> TagNode = UGameplayTagsManager::Get().FindTagNode(Tag))
	{
		return TagNode->GetSimpleTagName();
	}

	return FName();
}

void UFPGAGameplayAbilitiesLibrary::GetQueryTags(const FGameplayTagQuery& Query, TArray<FGameplayTag>& OutTags)
{
	FGameplayTagQueryExpression Expr;
	Query.GetQueryExpr(Expr);

	TArray<uint8> Tokens;
	Expr.EmitTokens(Tokens, OutTags);
}

void UFPGAGameplayAbilitiesLibrary::FillRelationshipTags(UPARAM(ref) FGameplayTagContainer& TagContainer, const AActor* Source, const AActor* Target)
{
	if (Source == nullptr || Target == nullptr)
	{
		return;
	}

	if (Source == Target)
	{
		TagContainer.AddTag(FFPGAGlobalTags::Get().Relationship_Self);
		TagContainer.AddTag(FFPGAGlobalTags::Get().Relationship_Friendly);
	}
	else
	{
		if (const IGenericTeamAgentInterface* SourceAgent = Cast<IGenericTeamAgentInterface>(Source))
		{
			ETeamAttitude::Type Attitude = SourceAgent->GetTeamAttitudeTowards(*Target);

			if (Attitude == ETeamAttitude::Friendly)
			{
				TagContainer.AddTag(FFPGAGlobalTags::Get().Relationship_Friendly);
			}
			else if (Attitude == ETeamAttitude::Hostile)
			{
				TagContainer.AddTag(FFPGAGlobalTags::Get().Relationship_Hostile);
			}
		}
	}
}

void UFPGAGameplayAbilitiesLibrary::ApplyGameplayAttributeInitter(FName GroupName, UAbilitySystemComponent* AbilitySystemComponent)
{
	if (FAttributeSetInitter* AttributeSetInit = IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()->GetAttributeSetInitter())
	{
		if (GroupName.IsValid())
		{
			AttributeSetInit->InitAttributeSetDefaults(AbilitySystemComponent, GroupName, 1, true);
		}
	}
}

bool UFPGAGameplayAbilitiesLibrary::EvaluateTargetFilterTaskSet(FFPTargetFilterTaskSet TaskSet, const AActor* SourceActor, const AActor* TargetActor)
{
	return TaskSet.DoesFilterPass(SourceActor, TargetActor);
}

bool UFPGAGameplayAbilitiesLibrary::EvaluateGameplayEffectModifierMagnitude(UAbilitySystemComponent* AbilitySystem, const FGameplayEffectModifierMagnitude& ModifierMagnitude, float& OutValue)
{
	if (!AbilitySystem)
	{
		return false;
	}

	TArray<FGameplayEffectAttributeCaptureDefinition> CaptureDefs;
	ModifierMagnitude.GetAttributeCaptureDefinitions(CaptureDefs);
	FGameplayEffectSpec Spec;

	for (FGameplayEffectAttributeCaptureDefinition CaptureDef : CaptureDefs)
	{
		Spec.CapturedRelevantAttributes.AddCaptureDefinition(CaptureDef);
	}

	Spec.CapturedRelevantAttributes.CaptureAttributes(AbilitySystem, EGameplayEffectAttributeCaptureSource::Source);

	if (ModifierMagnitude.AttemptCalculateMagnitude(Spec, OutValue))
	{
		return true;
	}

	return false;
}
