// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Effects/FPGameplayEffect_Infinite.h"

#include "GameplayEffect.h"

UFPGameplayEffect_Infinite::UFPGameplayEffect_Infinite()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite; 
}
