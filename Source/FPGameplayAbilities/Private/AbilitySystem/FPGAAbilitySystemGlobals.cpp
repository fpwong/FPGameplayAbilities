// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAAbilitySystemGlobals.h"

#include "FPGAAttributeSetInitter.h"

void UFPGAAbilitySystemGlobals::AllocAttributeSetInitter()
{
	GlobalAttributeSetInitter = TSharedPtr<FAttributeSetInitter>(new FFPGAAttributeSetInitter());;
}
