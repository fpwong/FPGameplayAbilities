// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAAbilitySystemGlobals.h"

#include "FPGAAttributeSetInitter.h"
#include "AbilitySystem/FPGAGlobalTags.h"

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
