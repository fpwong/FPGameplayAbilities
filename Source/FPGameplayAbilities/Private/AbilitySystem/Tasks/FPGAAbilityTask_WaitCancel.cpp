// Copyright

#include "AbilitySystem/Tasks/FPGAAbilityTask_WaitCancel.h"

#include "AbilitySystemComponent.h"

UFPGAAbilityTask_WaitCancel::UFPGAAbilityTask_WaitCancel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RegisteredCallbacks = false;
}

void UFPGAAbilityTask_WaitCancel::OnCancelCallback()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancel.Broadcast();
		}
		EndTask();
	}
}

void UFPGAAbilityTask_WaitCancel::OnLocalCancelCallback()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

	if (AbilitySystemComponent && IsPredictingClient())
	{
		AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(),
		                                                 AbilitySystemComponent->ScopedPredictionKey);
	}
	OnCancelCallback();
}

UFPGAAbilityTask_WaitCancel* UFPGAAbilityTask_WaitCancel::FPGAWaitCancel(UGameplayAbility* OwningAbility, bool bEndOnCancelInput)
{
	UFPGAAbilityTask_WaitCancel* Task = NewAbilityTask<UFPGAAbilityTask_WaitCancel>(OwningAbility);
	Task->bEndOnCancelInput = bEndOnCancelInput;
	return Task;
}

void UFPGAAbilityTask_WaitCancel::Activate()
{
	if (AbilitySystemComponent && bEndOnCancelInput)
	{
		const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();

		if (Info->IsLocallyControlled())
		{
			// We have to wait for the callback from the AbilitySystemComponent.
			AbilitySystemComponent->GenericLocalCancelCallbacks.AddDynamic(this, &UFPGAAbilityTask_WaitCancel::OnLocalCancelCallback); // Tell me if the cancel input is pressed

			RegisteredCallbacks = true;
		}
		else
		{
			if (CallOrAddReplicatedDelegate(EAbilityGenericReplicatedEvent::GenericCancel, FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &UFPGAAbilityTask_WaitCancel::OnCancelCallback)))
			{
				// GenericCancel was already received from the client and we just called OnCancelCallback. The task is done.
				return;
			}
		}
	}
}

void UFPGAAbilityTask_WaitCancel::OnDestroy(bool AbilityEnding)
{
	if (RegisteredCallbacks && AbilitySystemComponent)
	{
		AbilitySystemComponent->GenericLocalCancelCallbacks.RemoveDynamic(this, &UFPGAAbilityTask_WaitCancel::OnLocalCancelCallback);
	}

	Super::OnDestroy(AbilityEnding);
}
