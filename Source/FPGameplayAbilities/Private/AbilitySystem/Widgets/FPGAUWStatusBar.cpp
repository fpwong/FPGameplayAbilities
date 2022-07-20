// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWStatusBar.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Widgets/FPGAUWStatusBarItem.h"
#include "Blueprint/WidgetTree.h"

void UFPGAUWStatusBar::SetActor(AActor* Actor)
{
	UAbilitySystemComponent* NewAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	// unbind from previous ability system
	if (BoundAbilitySystem && NewAbilitySystem != BoundAbilitySystem)
	{
		BoundAbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	}

	BoundAbilitySystem = NewAbilitySystem;
	if (!BoundAbilitySystem)
	{
		StatusBarPanel->ClearChildren();
		return;
	}

	BoundAbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &ThisClass::HandleGameplayEffectAdded);

	TArray<FGameplayEffectSpec> ActiveGameplayEffectSpecs;
	BoundAbilitySystem->GetAllActiveGameplayEffectSpecs(ActiveGameplayEffectSpecs);

	if (StatusBarItemClass)
	{
		for (FActiveGameplayEffectHandle Handle : BoundAbilitySystem->GetActiveEffects(EffectQuery))
		{
			if (auto Effect = BoundAbilitySystem->GetActiveGameplayEffect(Handle))
			{
				CreateStatusBarItem(Effect);
			}
		}
	}
}

void UFPGAUWStatusBar::CreateStatusBarItem(const FActiveGameplayEffect* ActiveGameplayEffect)
{
	if (!ActiveGameplayEffect)
	{
		return;
	}

	// // check effect tags
	// if (!EffectTagRequirements.IsEmpty())
	// {
	// 	FGameplayTagContainer GETags;
	// 	ActiveGameplayEffect->Spec.GetAllAssetTags(GETags);
	//
	// 	if (!EffectTagRequirements.Matches(GETags))
	// 	{
	// 		return;
	// 	}
	// }

	if (UFPGAUWStatusBarItem* StatusBarItem = WidgetTree->ConstructWidget<UFPGAUWStatusBarItem>(StatusBarItemClass))
	{
		StatusBarItem->SetGameplayEffect(ActiveGameplayEffect->Handle);
		StatusBarPanel->AddChild(StatusBarItem);
	}
}

void UFPGAUWStatusBar::HandleGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	CreateStatusBarItem(AbilitySystemComponent->GetActiveGameplayEffect(ActiveGameplayEffectHandle));
}
