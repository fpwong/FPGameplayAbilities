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
#include "Engine/Engine.h"

void UFPGAGameplayAbilitiesLibrary::AddAttributeSet(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UAttributeSet> Attributes, const UDataTable* DataTable, FName GroupName)
{
	if (!AbilitySystem || !Attributes)
		return;

	// Spawn attributes and skip initialization
	AbilitySystem->InitStats(Attributes, nullptr);

	// Get your attribute set
	UAttributeSet* AttributeSet = nullptr;
	for (UAttributeSet* Att : AbilitySystem->SpawnedAttributes)
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

	for (TFieldIterator<UProperty> It(AttributeSet->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		UProperty* Property = *It;
		UNumericProperty* NumericProperty = Cast<UNumericProperty>(Property);
		if (NumericProperty)
		{
			FString RowNameStr = FString::Printf(TEXT("%s.%s.%s"), *GroupName.ToString(), *Property->GetOuter()->GetName(), *Property->GetName());

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
	if (!AbilitySystem)
		return FGameplayAbilitySpecHandle();

	if (AbilitySystem->GetOwner()->HasAuthority() && Ability)
	{
		FGameplayAbilitySpecHandle Handle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, -1));
		return Handle;
	}

	return FGameplayAbilitySpecHandle();
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

bool UFPGAGameplayAbilitiesLibrary::TryActivateAbility(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle AbilityToActivate, bool bAllowRemoteActivation)
{
	if (!AbilitySystem)
		return false;
	return AbilitySystem->TryActivateAbility(AbilityToActivate, bAllowRemoteActivation);
}

FGameplayAbilitySpecHandle UFPGAGameplayAbilitiesLibrary::GetAbilitySpecHandle(UGameplayAbility* Ability)
{
	if (!Ability)
		return FGameplayAbilitySpecHandle();
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
	for (UAttributeSet* Set : AbilitySystem->SpawnedAttributes)
	{
		if (Set && Set->IsA(AttributeClass))
		{
			return Set;
		}
	}
	return nullptr;
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

FVector UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index)
{
	if (TargetData.Data.IsValidIndex(Index))
	{
		FGameplayAbilityTargetData* Data = TargetData.Data[Index].Get();
		if (Data)
		{
			TArray<TWeakObjectPtr<AActor>> WeakArray = Data->GetActors();
			for (TWeakObjectPtr<AActor>& WeakPtr : WeakArray)
			{
				return WeakPtr.Get()->GetActorLocation();
			}

			const FHitResult* HitResultPtr = Data->GetHitResult();
			if (HitResultPtr)
			{
				return HitResultPtr->Location;
			}

			return Data->GetEndPoint();
		}
	}

	return FVector::ZeroVector;
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

int32 UFPGAGameplayAbilitiesLibrary::GetTagCount(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag)
{
	return (AbilitySystem == nullptr) ? -1 : AbilitySystem->GetTagCount(Tag);
}

//FGameplayTargetDataFilterHandle UFPGAGameplayAbilitiesLibrary::MakeFPGAFilterHandle(FFPGAGameplayTargetDataFilter Filter, AActor* FilterActor)
//{
//	FGameplayTargetDataFilterHandle FilterHandle;
//	FFPGAGameplayTargetDataFilter* NewFilter = new FFPGAGameplayTargetDataFilter(Filter);
//	NewFilter->InitializeFilterContext(FilterActor);
//	FilterHandle.Filter = TSharedPtr<FGameplayTargetDataFilter>(NewFilter);
//	return FilterHandle;
//}
