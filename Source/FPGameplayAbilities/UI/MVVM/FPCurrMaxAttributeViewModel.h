// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "FPMVVMTypes.h"
#include "GameplayEffectTypes.h"
#include "MVVMViewModelBase.h"

#include "FPCurrMaxAttributeViewModel.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPCurrMaxAttributeResolver : public UFPAbilitySystemResolverBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Curr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Max;

	virtual UObject* CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const override;
};

UCLASS(meta = (DisplayName = "Testing"))
class FPGAMEPLAYABILITIES_API UFPCurrMaxAttributeViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	static UFPCurrMaxAttributeViewModel* CreateInstance(UAbilitySystemComponent* InASC, FGameplayAttribute Curr, FGameplayAttribute Max);

	void OnCurrChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void Init(UAbilitySystemComponent* InASC, FGameplayAttribute InCurr, FGameplayAttribute InMax);

	UPROPERTY(BlueprintReadWrite, FieldNotify)
	float Percent;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	float CurrValue;
	void SetCurrValue(float NewValue);

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	float MaxValue;
	void SetMaxValue(float NewValue);

	void UpdatePercent();

protected:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemPtr;
};
