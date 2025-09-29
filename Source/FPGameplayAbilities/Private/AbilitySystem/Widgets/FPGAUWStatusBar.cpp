// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWStatusBar.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Widgets/FPGAUWStatusBarItem.h"
#include "Blueprint/WidgetTree.h"

void UFPGAUWStatusBar::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
}

void UFPGAUWStatusBar::SetActor(AActor* Actor)
{
	UAbilitySystemComponent* NewAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if (NewAbilitySystem == BoundAbilitySystem)
	{
		return;
	}

	StatusBarPanel->ClearChildren();

	// unbind from previous ability system
	if (BoundAbilitySystem)
	{
		BoundAbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	}

	BoundAbilitySystem = NewAbilitySystem;

	if (!BoundAbilitySystem)
	{
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	SetVisibility(ESlateVisibility::Visible);

	BoundAbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &ThisClass::HandleGameplayEffectAdded);

	if (StatusBarItemClass)
	{
		for (FActiveGameplayEffectHandle Handle : BoundAbilitySystem->GetActiveEffects(EffectQuery))
		{
			CreateStatusBarItem(Handle, false);
		}
	}
}

void UFPGAUWStatusBar::CreateStatusBarItem(const FActiveGameplayEffectHandle& ActiveEffectHandle, bool bCheckEffectQuery)
{
	if (!ActiveEffectHandle.IsValid())
	{
		return;
	}

	if (bCheckEffectQuery)
	{
		if (const FActiveGameplayEffect* GameplayEffect = BoundAbilitySystem->GetActiveGameplayEffect(ActiveEffectHandle))
		{
			if (!EffectQuery.Matches(*GameplayEffect))
			{
				return;
			}
		}
	}

	if (UFPGAUWStatusBarItem* StatusBarItem = WidgetTree->ConstructWidget<UFPGAUWStatusBarItem>(StatusBarItemClass))
	{
		StatusBarPanel->AddChild(StatusBarItem);
		StatusBarItem->SetGameplayEffect(ActiveEffectHandle);
	}
}

void UFPGAUWStatusBar::HandleGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	CreateStatusBarItem(ActiveGameplayEffectHandle, true);
}
