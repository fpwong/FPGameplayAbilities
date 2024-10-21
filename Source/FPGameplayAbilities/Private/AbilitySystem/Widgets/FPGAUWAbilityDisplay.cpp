// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWAbilityDisplay.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "Abilities/GameplayAbility.h"
#include "Components/ProgressBar.h"

// TODO this doesn't work for non-instanced abilities?

void UFPGAUWAbilityDisplay::HandleAbilityEnded(UGameplayAbility* GameplayAbility)
{
	// UE_LOG(LogTemp, Warning, TEXT("Ability ended too"));
	GameplayAbility->OnGameplayAbilityEnded.RemoveAll(this);

	if (ActiveAbility == GameplayAbility)
	{
		ActiveAbility = nullptr;
		OnAbilityEnded.Broadcast(this);
	}
}

void UFPGAUWAbilityDisplay::HandleAbilityActivate(UGameplayAbility* GameplayAbility)
{
	if (!Ability || !GameplayAbility)
	{
		return;
	}

	if (GameplayAbility->GetClass() == Ability->GetClass())
	{
		ActiveAbility = GameplayAbility;
		// UE_LOG(LogTemp, Warning, TEXT("This is correct?"));
		GameplayAbility->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::HandleAbilityEnded);
		OnAbilityActivated.Broadcast(this);
	}
}

void UFPGAUWAbilityDisplay::HandleAbilityCommitted(UGameplayAbility* GameplayAbility)
{
	if (ActiveAbility == GameplayAbility)
	{
		OnAbilityCommited.Broadcast(this);
	}
}

void UFPGAUWAbilityDisplay::InitAbility(UAbilitySystemComponent* InAbilitySystem, UGameplayAbility* InAbility)
{
	Ability = InAbility;
	AbilitySystem = InAbilitySystem;

	AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::HandleAbilityActivate);
	AbilitySystem->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::HandleAbilityCommitted);

	if (Ability && AbilityNameLabel)
	{
		AbilityNameLabel->SetText(FText::FromString(Ability->GetName()));
	}

	UpdateAbilityProgress();
}

void UFPGAUWAbilityDisplay::ClearAbility()
{
	if (AbilitySystem)
	{
		AbilitySystem->AbilityActivatedCallbacks.RemoveAll(this);
		AbilitySystem->AbilityCommittedCallbacks.RemoveAll(this);
	}

	Ability = nullptr;
	AbilitySystem = nullptr;
	AbilityProgressBar->SetPercent(0);
}

void UFPGAUWAbilityDisplay::UpdateAbilityProgress()
{
	if (!Ability)
	{
		AbilityProgressBar->SetPercent(0);
		return;
	}

	float TimeRemaining = 0;
	float CooldownDuration = 0;

	if (Ability->GetCooldownGameplayEffect())
	{
		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), TimeRemaining, CooldownDuration);
	}

	if (AbilityProgressBar)
	{
		float Progress = TimeRemaining > 0 ? TimeRemaining / CooldownDuration : 0;

		if (bFullProgressWhenOnCooldown && Progress > 0)
		{
			Progress = 1.0;
		}

		if (bInvertProgress)
		{
			Progress = 1.0f - Progress;
		}

		AbilityProgressBar->SetPercent(Progress);
	}

	if (CooldownLabel)
	{
		CooldownLabel->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), TimeRemaining)));
		CooldownLabel->SetVisibility(TimeRemaining > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UFPGAUWAbilityDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateAbilityProgress();
}
