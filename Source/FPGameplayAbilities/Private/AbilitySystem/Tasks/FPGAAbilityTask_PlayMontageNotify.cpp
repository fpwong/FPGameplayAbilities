// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Tasks/FPGAAbilityTask_PlayMontageNotify.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UFPGAAbilityTask_PlayMontageNotify::UFPGAAbilityTask_PlayMontageNotify(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

void UFPGAAbilityTask_PlayMontageNotify::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// Reset AnimRootMotionTranslationScale
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
				(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(NAME_None);
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(NAME_None);
		}
	}
}

void UFPGAAbilityTask_PlayMontageNotify::OnMontageInterrupted()
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(NAME_None);
		}
	}
}

void UFPGAAbilityTask_PlayMontageNotify::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindDelegates();
	
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(NAME_None);
		}
	}

	EndTask();
}

UFPGAAbilityTask_PlayMontageNotify* UFPGAAbilityTask_PlayMontageNotify::CreatePlayMontageAndWaitProxy(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	UAnimMontage* MontageToPlay,
	float Rate,
	FName StartSection,
	bool bStopWhenAbilityEnds,
	bool bEndOnCancelInput,
	float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UFPGAAbilityTask_PlayMontageNotify* MyObj = NewAbilityTask<UFPGAAbilityTask_PlayMontageNotify>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	// MyObj->bEndOnCancelInput = bEndOnCancelInput;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return MyObj;
}

void UFPGAAbilityTask_PlayMontageNotify::Activate()
{
	// Super::Activate();

	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	if (AbilitySystemComponent && MontageToPlay)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			const float NewSequenceTime = MontageToPlay->SequenceLength / Rate;

			if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, NewSequenceTime, StartSection) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				AnimInstancePtr = AnimInstance;

				InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UFPGAAbilityTask_PlayMontageNotify::OnMontageInterrupted);

				BlendingOutDelegate.BindUObject(this, &UFPGAAbilityTask_PlayMontageNotify::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UFPGAAbilityTask_PlayMontageNotify::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UFPGAAbilityTask_PlayMontageNotify::OnNotifyBeginReceived);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UFPGAAbilityTask_PlayMontageNotify::OnNotifyEndReceived);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UFPGAAbilityTask_PlayMontageNotify call to PlayMontage failed!"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UFPGAAbilityTask_PlayMontageNotify called on invalid AbilitySystemComponent or Montage"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UFPGAAbilityTask_PlayMontageNotify called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),
		            *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(NAME_None);
		}
	}

	SetWaitingOnAvatar();
}

void UFPGAAbilityTask_PlayMontageNotify::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	OnNotifyBegin.Broadcast(NotifyName);
}

void UFPGAAbilityTask_PlayMontageNotify::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	OnNotifyEnd.Broadcast(NotifyName);
}

void UFPGAAbilityTask_PlayMontageNotify::ExternalCancel()
{
	check(AbilitySystemComponent);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(NAME_None);
	}
	Super::ExternalCancel();
}

void UFPGAAbilityTask_PlayMontageNotify::OnDestroy(bool AbilityEnded)
{	
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability invalid"));
	}

	Super::OnDestroy(AbilityEnded);
}

bool UFPGAAbilityTask_PlayMontageNotify::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor info invalid"));
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Anim instance invalid"));
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to unbind?"));
			}

			// UE_LOG(LogTemp, Warning, TEXT("Stopped playing montage! %s | %s"), *ActorInfo->OwnerActor->GetName(), *AbilitySystemComponent->GetOwner()->GetName());
			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

FString UFPGAAbilityTask_PlayMontageNotify::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWait. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

void UFPGAAbilityTask_PlayMontageNotify::UnbindDelegates()
{
	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UFPGAAbilityTask_PlayMontageNotify::OnNotifyBeginReceived);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UFPGAAbilityTask_PlayMontageNotify::OnNotifyEndReceived);
	}
}

void UFPGAAbilityTask_PlayMontageNotify::BeginDestroy()
{
	UnbindDelegates();
	Super::BeginDestroy();
}
