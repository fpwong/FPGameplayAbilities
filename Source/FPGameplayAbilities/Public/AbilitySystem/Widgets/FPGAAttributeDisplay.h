// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CommonTextBlock.h"
#include "GameplayTagContainer.h"
#include "Misc/FPGameplayValueRow.h"
#include "GameplayEffectTypes.h"
#include "FPGAAttributeDisplay.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFPGAAttributeDisplayChanged, float, OldValue, float, NewValue);

struct FOnAttributeChangeData;
class UAbilitySystemComponent;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAAttributeDisplay : public UCommonTextBlock
{
	GENERATED_BODY()

public:
	FDelegateHandle DelegateHandle;

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemPtr = nullptr;

	bool bNeedToUnbind = false;

	/** The attribute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer AttributeTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UFPValueDisplay> ValueDisplayMethod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinNumDecimals = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxNumDecimals = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowPlusMinus = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowAttributeName = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowBaseFinal = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameplayModEvaluationChannel BaseChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Unit;

	UPROPERTY(BlueprintAssignable)
	FOnFPGAAttributeDisplayChanged OnAttributeDisplayChanged;

	UPROPERTY()
	float OldValue = 0.0f;

	FNumberFormattingOptions NumberFormat;

	UFPGAAttributeDisplay();

	UFUNCTION(BlueprintCallable)
	void BindAttribute(UAbilitySystemComponent* AbilitySystem);

	UFUNCTION(BlueprintCallable)
	void BindAndSetAttribute(UAbilitySystemComponent* AbilitySystem, FGameplayAttribute NewAttribute);

	UFUNCTION(BlueprintCallable)
	void BindAndSetAttributeWithTag(UAbilitySystemComponent* AbilitySystem, FGameplayAttribute NewAttribute, FGameplayTagContainer NewAttributeTags);

	void UnbindAttribute();

	void OnAttributeChanged(const FOnAttributeChangeData& ChangeData);

	void UpdateAttributeValue(bool bBroadcastChange);

	virtual void BeginDestroy() override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
};
