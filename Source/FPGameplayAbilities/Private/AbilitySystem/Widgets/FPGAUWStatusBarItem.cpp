// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWStatusBarItem.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "GameplayTagsManager.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "Components/ProgressBar.h"

void UFPGAUWStatusBarItem::SetGameplayEffect(FActiveGameplayEffectHandle InActiveGameplayEffectHandle)
{
	ActiveGameplayEffectHandle = InActiveGameplayEffectHandle;
	check(ActiveGameplayEffectHandle.IsValid());
	AbilitySystem = ActiveGameplayEffectHandle.GetOwningAbilitySystemComponent();

	{
		if (FActiveGameplayEffectEvents* ActiveEventSet = AbilitySystem->GetActiveEffectEventSet(ActiveGameplayEffectHandle))
		{
			ActiveEventSet->OnEffectRemoved.AddUObject(this, &ThisClass::OnEffectRemoved);
		}
	}

	// TODO: stack change / time change
	// AbilitySystem->OnGameplayEffectStackChangeDelegate(ActiveGameplayEffectHandle);
	// AbilitySystem->OnGameplayEffectTimeChangeDelegate(ActiveGameplayEffectHandle);

	if (const FActiveGameplayEffect* ActiveGameplayEffect = AbilitySystem->GetActiveGameplayEffect(ActiveGameplayEffectHandle))
	{
		ActiveGameplayEffect->EventSet.OnEffectRemoved;;
		StartWorldTime = ActiveGameplayEffect->StartWorldTime;
		Duration = ActiveGameplayEffect->GetDuration();

		// infinite duration, set the progress to full
		if (Duration < 0.0f && DurationBar)
		{
			DurationBar->SetPercent(1.0f);
		}

		FString EffectName = ActiveGameplayEffect->Spec.ToSimpleString();
		FGameplayTagContainer EffectTags;
		ActiveGameplayEffect->Spec.GetAllAssetTags(EffectTags);
		if (EffectTags.Num() > 0)
		{
			EffectName = UFPGAGameplayAbilitiesLibrary::GetSimpleTagName(EffectTags.GetByIndex(0)).ToString();
		}
		else
		{
			FGameplayTagContainer StatusTags;
			ActiveGameplayEffect->Spec.GetAllGrantedTags(StatusTags);
			if (StatusTags.Num() > 0)
			{
				EffectName = UFPGAGameplayAbilitiesLibrary::GetSimpleTagName(StatusTags.GetByIndex(0)).ToString();
			}
		}

		if (NameLabel)
		{
			NameLabel->SetText(FText::FromString(EffectName));
		}

		// TODO: Setup a tooltip widget
		if (UWidget* Root = GetRootWidget())
		{
			Root->SetToolTipText(FText::FromString(EffectName));
		}
	}

	if (DurationLabel)
	{
		DurationLabel->SetText(FText::GetEmpty());
	}
}

void UFPGAUWStatusBarItem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Duration > 0)
	{
		const float TimeRemaining = Duration - (GetWorld()->GetTimeSeconds() - StartWorldTime);
		if (TimeRemaining > 0)
		{
			if (DurationLabel)
			{
				FText DurationText;
				if (TimeRemaining > 0)
				{
					DurationText = FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(TimeRemaining)));
				}

				DurationLabel->SetText(DurationText);
			}

			if (DurationBar)
			{
				DurationBar->SetPercent(TimeRemaining / Duration);
			}
		}
		else
		{
			RemoveFromParent();
		}
	}
}

void UFPGAUWStatusBarItem::OnEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo)
{
	RemoveFromParent();
}
