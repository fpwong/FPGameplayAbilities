// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "FPMVVMTypes.h"
#include "GameplayEffectTypes.h"
#include "MVVMViewModelBase.h"

#include "FPAttributeViewModel.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPAttributeResolver : public UFPAbilitySystemResolverBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Attribute;

	virtual UObject* CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const override;
};

UCLASS()
class FPGAMEPLAYABILITIES_API UFPAttributeViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// UFUNCTION(BlueprintCallable, Category="View Model")
	static UFPAttributeViewModel* CreateInstance(UAbilitySystemComponent* InASC, FGameplayAttribute InAttribute);

	void OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void Init(UAbilitySystemComponent* InASC, FGameplayAttribute InAttribute);

	UPROPERTY(BlueprintReadWrite)
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter)
	float AttributeValue;

	void SetAttributeValue(float NewValue);

protected:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemPtr;
	FDelegateHandle DelegateHandle;

};
