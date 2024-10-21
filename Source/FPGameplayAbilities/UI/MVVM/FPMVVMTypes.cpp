// Fill out your copyright notice in the Description page of Project Settings.


#include "FPMVVMTypes.h"

#include "AbilitySystemGlobals.h"
#include "FPAttributeViewModel.h"
#include "Blueprint/UserWidget.h"

UAbilitySystemComponent* UFPAbilitySystemResolverBase::GetAbilitySystem(const UUserWidget* UserWidget) const
{
	FLocalPlayerContext Context = UserWidget->GetPlayerContext();
	APawn* Pawn = Context.GetPawn();
	return Pawn ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn) : nullptr;
}