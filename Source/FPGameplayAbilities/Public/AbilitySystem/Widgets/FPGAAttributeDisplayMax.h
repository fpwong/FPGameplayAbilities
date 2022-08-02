// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CommonTextBlock.h"
#include "GameplayEffectTypes.h"
#include "FPGAAttributeDisplayMax.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAAttributeDisplayMax : public UCommonTextBlock
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute CurrentAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute MaxAttribute;

	UFUNCTION(BlueprintCallable)
	void BindToAbilitySystem(UAbilitySystemComponent* AbilitySystem);

	UFUNCTION(BlueprintCallable)
	void BindAndSetAttributes(UAbilitySystemComponent* AbilitySystem, const FGameplayAttribute& Current, const FGameplayAttribute& Max);

protected:
	void UpdateText();

	void OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	TWeakObjectPtr<UAbilitySystemComponent> BoundAbilitySystem;

	void UnbindAbilitySystem();

	virtual void BeginDestroy() override;
};
