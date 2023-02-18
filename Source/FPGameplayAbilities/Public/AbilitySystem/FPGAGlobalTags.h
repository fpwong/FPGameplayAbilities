// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

struct FPGAMEPLAYABILITIES_API FFPGAGlobalTags : public FGameplayTagNativeAdder
{
	FORCEINLINE static const FFPGAGlobalTags& Get() { return Tags; }

	FGameplayTag Misc_Dummy;
	FGameplayTag Relationship_Self;
	FGameplayTag Relationship_Friendly;
	FGameplayTag Relationship_Hostile;

	FGameplayTag Activate_Fail_IsDead;
	FGameplayTag Activate_Fail_Cooldown;
	FGameplayTag Activate_Fail_Cost;
	FGameplayTag Activate_Fail_TagsBlocked;
	FGameplayTag Activate_Fail_TagsMissing;
	FGameplayTag Activate_Fail_Networking;

protected:
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		Misc_Dummy = Manager.AddNativeGameplayTag(TEXT("Misc.Dummy"));
		Relationship_Self = Manager.AddNativeGameplayTag(TEXT("Relationship.Self"));
		Relationship_Friendly = Manager.AddNativeGameplayTag(TEXT("Relationship.Friendly"));
		Relationship_Hostile = Manager.AddNativeGameplayTag(TEXT("Relationship.Hostile"));

		Activate_Fail_IsDead = Manager.AddNativeGameplayTag(TEXT("Activate.Fail.IsDead"));
		Activate_Fail_Cooldown = Manager.AddNativeGameplayTag(TEXT("Activate.Fail.Cooldown"));
		Activate_Fail_Cost = Manager.AddNativeGameplayTag(TEXT("Activate.Fail.Cost"));
		Activate_Fail_TagsBlocked = Manager.AddNativeGameplayTag(TEXT("Activate.Fail.TagsBlocked"));
		Activate_Fail_TagsMissing = Manager.AddNativeGameplayTag(TEXT("Activate.Fail.TagsMissing"));
		Activate_Fail_Networking = Manager.AddNativeGameplayTag(TEXT("Activate.Fail.Networking"));
	}

private:
	static FFPGAGlobalTags Tags;
};
