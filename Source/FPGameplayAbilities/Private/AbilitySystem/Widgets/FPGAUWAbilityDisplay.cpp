// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWAbilityDisplay.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "Abilities/GameplayAbility.h"
#include "Components/ProgressBar.h"

// TODO this doesn't work for non-instanced abilities?

void UFPGAUWAbilityDisplay::OnAbilityEnded(UGameplayAbility* GameplayAbility)
{
		// UE_LOG(LogTemp, Warning, TEXT("Ability ended too"));
	GameplayAbility->OnGameplayAbilityEnded.RemoveAll(this);

	if (ActiveAbility == GameplayAbility)
	{
		ActiveAbility = nullptr;
	}
}

void UFPGAUWAbilityDisplay::OnAbilityActivate(UGameplayAbility* GameplayAbility)
{
	if (GameplayAbility->GetClass() == Ability->GetClass())
	{
		ActiveAbility = GameplayAbility;
		// UE_LOG(LogTemp, Warning, TEXT("This is correct?"));
		GameplayAbility->OnGameplayAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
	}
}

void UFPGAUWAbilityDisplay::InitAbility(UAbilitySystemComponent* InAbilitySystem, UGameplayAbility* InAbility)
{
	Ability = InAbility;
	AbilitySystem = InAbilitySystem;

	AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnAbilityActivate);

	// auto ASC = Ability->GetAbilitySystemComponentFromActorInfo();
	// Ability->OnGameplayAbilityEnded;

	if (Ability && AbilityNameLabel)
	{
		AbilityNameLabel->SetText(FText::FromString(Ability->GetName()));
	}

	UpdateAbilityProgress();
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
		const float Progress = TimeRemaining > 0 ? TimeRemaining / CooldownDuration : 0;
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
