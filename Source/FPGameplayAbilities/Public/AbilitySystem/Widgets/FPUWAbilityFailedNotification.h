#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPUWAbilityFailedNotification.generated.h"

class UCommonTextBlock;
UCLASS()
class FPGAMEPLAYABILITIES_API UFPUWAbilityFailedNotification : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* Label;

	UFUNCTION(BlueprintCallable)
	void BindToAbilitySystem(UAbilitySystemComponent* AbilitySystem);

protected:
	TWeakObjectPtr<UAbilitySystemComponent> OldBoundAbilitySystem;

	UFUNCTION()
	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="HandleAbilityFailed"))
	void BP_HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& Tags);
};
