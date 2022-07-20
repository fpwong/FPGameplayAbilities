// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWStatusBarItem.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"

void UFPGAUWStatusBarItem::SetGameplayEffect(const FActiveGameplayEffectHandle& InActiveGameplayEffectHandle)
{
	ActiveGameplayEffectHandle = InActiveGameplayEffectHandle;
	AbilitySystem = ActiveGameplayEffectHandle.GetOwningAbilitySystemComponent();

	if (NameLabel)
	{
		if (const FActiveGameplayEffect* ActiveGameplayEffect = GetActiveGameplayEffect())
		{
			NameLabel->SetText(FText::FromString(ActiveGameplayEffect->Spec.Def->GetName()));
		}
	}
}

const FActiveGameplayEffect* UFPGAUWStatusBarItem::GetActiveGameplayEffect()
{
	return AbilitySystem->GetActiveGameplayEffect(ActiveGameplayEffectHandle);
}

void UFPGAUWStatusBarItem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DurationLabel)
	{
		if (const FActiveGameplayEffect* ActiveGameplayEffect = GetActiveGameplayEffect())
		{
			FText DurationText; 
			if (ActiveGameplayEffect->GetDuration() > 0)
			{
				const float TimeRemaining = ActiveGameplayEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());

				if (TimeRemaining > 0)
				{
					DurationText = FText::FromString(FString::Printf(TEXT("%.1f"), TimeRemaining));
				}
			}

			DurationLabel->SetText(DurationText);
		}
		else
		{
			RemoveFromParent();
		}
	}
}
