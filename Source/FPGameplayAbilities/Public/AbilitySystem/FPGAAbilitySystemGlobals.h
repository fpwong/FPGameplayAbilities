// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "FPGAAbilitySystemGlobals.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	static UFPGAAbilitySystemGlobals* GetFPPtr();

	bool bBoundToCurveTables = false;

	virtual void InitAttributeDefaults() override;

	virtual void AllocAttributeSetInitter() override;

	virtual void InitGlobalTags() override;

	virtual void OnCurveTableChanged();
};
