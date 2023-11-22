// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAAbilitySystemGlobals.h"

#include "FPGAAttributeSetInitter.h"
#include "AbilitySystem/FPGAGlobalTags.h"

UFPGAAbilitySystemGlobals* UFPGAAbilitySystemGlobals::GetFPPtr()
{
	return Cast<UFPGAAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
}

void UFPGAAbilitySystemGlobals::InitAttributeDefaults()
{
	Super::InitAttributeDefaults();

#if WITH_EDITOR
	if (!bBoundToCurveTables)
	{
		for (UCurveTable* Table : GlobalAttributeDefaultsTables)
		{
			Table->OnCurveTableChanged().AddUObject(this, &UFPGAAbilitySystemGlobals::OnCurveTableChanged);
		}

		bBoundToCurveTables = true;
	}
#endif
}

void UFPGAAbilitySystemGlobals::AllocAttributeSetInitter()
{
	GlobalAttributeSetInitter = MakeShared<FFPGAAttributeSetInitter>();
}

void UFPGAAbilitySystemGlobals::InitGlobalTags()
{
	Super::InitGlobalTags();

	ActivateFailIsDeadTag = FFPGAGlobalTags::Get().Activate_Fail_IsDead;
	ActivateFailCooldownTag = FFPGAGlobalTags::Get().Activate_Fail_Cooldown;
	ActivateFailCostTag = FFPGAGlobalTags::Get().Activate_Fail_Cost;
	ActivateFailTagsBlockedTag = FFPGAGlobalTags::Get().Activate_Fail_TagsBlocked;
	ActivateFailTagsMissingTag = FFPGAGlobalTags::Get().Activate_Fail_TagsMissing;
	ActivateFailNetworkingTag = FFPGAGlobalTags::Get().Activate_Fail_Networking;
}

void UFPGAAbilitySystemGlobals::OnCurveTableChanged()
{
	UE_LOG(LogTemp, Warning, TEXT("Curve table changed: Reloading attribute defaults"));
	ReloadAttributeDefaults();
}
