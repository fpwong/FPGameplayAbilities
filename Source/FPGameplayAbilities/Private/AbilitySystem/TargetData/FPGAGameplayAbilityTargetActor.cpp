// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/TargetData/FPGAGameplayAbilityTargetActor.h"
#include "GameplayAbilitySpec.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

// --------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	AFPGAGameplayAbilityTargetActor
//
// --------------------------------------------------------------------------------------------------------------------------------------------------------

AFPGAGameplayAbilityTargetActor::AFPGAGameplayAbilityTargetActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ShouldProduceTargetDataOnServer = false;
	bDebug = false;
	bDestroyOnConfirmation = true;
}

void AFPGAGameplayAbilityTargetActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GenericDelegateBoundASC)
	{
		// We must remove ourselves from GenericLocalConfirmCallbacks/GenericLocalCancelCallbacks, since while these are bound they will inhibit any *other* abilities
		// that are bound to the same key.

		UAbilitySystemComponent* UnboundASC = nullptr;
		const FGameplayAbilityActorInfo* Info = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
		if (Info && Info->IsLocallyControlled())
		{
			UAbilitySystemComponent* ASC = Info->AbilitySystemComponent.Get();
			if (ASC)
			{
				ASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &AFPGAGameplayAbilityTargetActor::ConfirmTargeting);
				ASC->GenericLocalCancelCallbacks.RemoveDynamic(this, &AFPGAGameplayAbilityTargetActor::CancelTargeting);

				UnboundASC = ASC;
			}
		}

		ensure(GenericDelegateBoundASC == UnboundASC); // Error checking that we have removed delegates from the same ASC we bound them to
	}

	Super::EndPlay(EndPlayReason);
}

void AFPGAGameplayAbilityTargetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPGAGameplayAbilityTargetActor, StartLocation);
	DOREPLIFETIME(AFPGAGameplayAbilityTargetActor, SourceActor);
	DOREPLIFETIME(AFPGAGameplayAbilityTargetActor, bDebug);
	DOREPLIFETIME(AFPGAGameplayAbilityTargetActor, bDestroyOnConfirmation);
}

void AFPGAGameplayAbilityTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	OwningAbility = Ability;
	SourceActor = Ability->GetCurrentActorInfo()->OwnerActor.Get();
	Filter.InitializeFilterContext(SourceActor);

	if (APlayerController* PC = Cast<APlayerController>(MasterPC))
	{
		PC->CurrentMouseCursor = EMouseCursor::Crosshairs;
	}
}

bool AFPGAGameplayAbilityTargetActor::IsConfirmTargetingAllowed()
{
	return true;
}

void AFPGAGameplayAbilityTargetActor::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	}
}

void AFPGAGameplayAbilityTargetActor::ConfirmTargeting()
{
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericConfirm,
		                                    OwningAbility->GetCurrentAbilitySpecHandle(),
		                                    OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericConfirmHandle);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("AFPGAGameplayAbilityTargetActor::ConfirmTargeting called with null Ability/ASC! Actor %s"), *GetName());
	}

	if (IsConfirmTargetingAllowed())
	{
		ConfirmTargetingAndContinue();
		if (bDestroyOnConfirmation)
		{
			Destroy();
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(MasterPC))
	{
		PC->CurrentMouseCursor = EMouseCursor::Default;
	}
}

/** Outside code is saying 'stop everything and just forget about it' */
void AFPGAGameplayAbilityTargetActor::CancelTargeting()
{
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, OwningAbility->GetCurrentAbilitySpecHandle(),
		                                    OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericCancelHandle);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("AFPGAGameplayAbilityTargetActor::CancelTargeting called with null ASC! Actor %s"), *GetName());
	}

	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	Destroy();
}

bool AFPGAGameplayAbilityTargetActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	//The player who created the ability doesn't need to be updated about it - there should be local prediction in place.
	if (RealViewer == MasterPC)
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : NULL);
	AActor* Avatar = (ActorInfo ? ActorInfo->AvatarActor.Get() : NULL);

	if (Avatar)
	{
		return Avatar->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}

	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

bool AFPGAGameplayAbilityTargetActor::OnReplicatedTargetDataReceived(FGameplayAbilityTargetDataHandle& Data) const
{
	return true;
}

bool AFPGAGameplayAbilityTargetActor::ShouldProduceTargetData() const
{
	// return true if we are locally owned, or (we are the server and this is a gameplaytarget ability that can produce target data server side)
	return (MasterPC && MasterPC->IsLocalController()) || ShouldProduceTargetDataOnServer;
}

void AFPGAGameplayAbilityTargetActor::BindToConfirmCancelInputs()
{
	check(OwningAbility);

	const FGameplayAbilityActorInfo* const Info = OwningAbility->GetCurrentActorInfo();
	UAbilitySystemComponent* const ASC = Info->AbilitySystemComponent.Get();
	if (ASC)
	{
		if (Info->IsLocallyControlled())
		{
			// We have to wait for the callback from the AbilitySystemComponent. Which will always be instigated locally
			ASC->GenericLocalConfirmCallbacks.AddDynamic(this, &AFPGAGameplayAbilityTargetActor::ConfirmTargeting); // Tell me if the confirm input is pressed
			ASC->GenericLocalCancelCallbacks.AddDynamic(this, &AFPGAGameplayAbilityTargetActor::CancelTargeting); // Tell me if the cancel input is pressed

			// Save off which ASC we bound so that we can error check that we're removing them later
			GenericDelegateBoundASC = ASC;
		}
		else
		{
			FGameplayAbilitySpecHandle Handle = OwningAbility->GetCurrentAbilitySpecHandle();
			FPredictionKey PredKey = OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey();

			GenericConfirmHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericConfirm, Handle, PredKey).AddUObject(
				this, &AFPGAGameplayAbilityTargetActor::ConfirmTargeting);
			GenericCancelHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, Handle, PredKey).AddUObject(
				this, &AFPGAGameplayAbilityTargetActor::CancelTargeting);

			if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GenericConfirm, Handle, PredKey))
			{
				return;
			}

			if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GenericCancel, Handle, PredKey))
			{
				return;
			}
		}
	}
}
