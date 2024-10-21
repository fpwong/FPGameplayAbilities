// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "View/MVVMViewModelContextResolver.h"
#include "FPMVVMTypes.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPAbilitySystemResolverBase : public UMVVMViewModelContextResolver
{
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystem(const UUserWidget* UserWidget) const;
};