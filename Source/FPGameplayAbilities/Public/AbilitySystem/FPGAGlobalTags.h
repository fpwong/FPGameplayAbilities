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
 
protected:
	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		Misc_Dummy = Manager.AddNativeGameplayTag(TEXT("Misc.Dummy"));
		Relationship_Self = Manager.AddNativeGameplayTag(TEXT("Relationship.Self"));
		Relationship_Friendly = Manager.AddNativeGameplayTag(TEXT("Relationship.Friendly"));
		Relationship_Hostile = Manager.AddNativeGameplayTag(TEXT("Relationship.Hostile"));
	}

private:
	static FFPGAGlobalTags Tags;
};
