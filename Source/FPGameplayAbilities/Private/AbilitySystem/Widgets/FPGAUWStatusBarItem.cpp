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
			ActiveEventSet->OnTimeChanged.AddUObject(this, &ThisClass::OnTimeChanged);
			ActiveEventSet->OnStackChanged.AddUObject(this, &ThisClass::OnStackChanged);
			ActiveEventSet->OnInhibitionChanged.AddUObject(this, &ThisClass::OnInhibitionChanged);
		}

		UpdateStackCount();
	}

	if (const FActiveGameplayEffect* ActiveGameplayEffect = AbilitySystem->GetActiveGameplayEffect(ActiveGameplayEffectHandle))
	{
		ActiveGameplayEffect->EventSet.OnEffectRemoved;;
		StartWorldTime = ActiveGameplayEffect->StartWorldTime;
		Duration = ActiveGameplayEffect->GetDuration();

		// hide duration bar when infinite
		if (Duration == FGameplayEffectConstants::INFINITE_DURATION && DurationBar)
		{
			DurationBar->SetVisibility(ESlateVisibility::Collapsed);
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

		BP_SetGameplayEffect(ActiveGameplayEffectHandle, EffectTags);
	}

	if (DurationLabel)
	{
		DurationLabel->SetText(FText::GetEmpty());
	}

}

UAbilitySystemComponent* UFPGAUWStatusBarItem::GetAbilitySystemComponent() const
{
	return AbilitySystem.Get();
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
					DurationText = FText::FromString(FString::Printf(TEXT("%d"), FMath::CeilToInt(TimeRemaining)));
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

void UFPGAUWStatusBarItem::OnTimeChanged(FActiveGameplayEffectHandle ActiveHandle, float NewStartTime, float NewDuration)
{
	Duration = NewDuration;
	StartWorldTime = NewStartTime;
}

void UFPGAUWStatusBarItem::OnStackChanged(FActiveGameplayEffectHandle ActiveHandle, int NewStackCount, int PreviousStackCount) const
{
	UpdateStackCount();
}

void UFPGAUWStatusBarItem::OnInhibitionChanged(FActiveGameplayEffectHandle ActiveHandle, bool bIsInhibited)
{
	BP_OnInhibitedChanged(bIsInhibited);
}

void UFPGAUWStatusBarItem::UpdateStackCount() const
{
	if (StackLabel && ActiveGameplayEffectHandle.IsValid())
	{
		int StackCount = AbilitySystem->GetCurrentStackCount(ActiveGameplayEffectHandle);
		if (StackCount > 1)
		{
			StackLabel->SetText(FText::FromString(FString::FromInt(StackCount)));
			StackLabel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			StackLabel->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
