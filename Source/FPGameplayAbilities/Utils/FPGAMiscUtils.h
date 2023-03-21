#pragma once

#include "CoreMinimal.h"
#include "FPGAMiscUtils.generated.h"

struct FGameplayTagContainer;
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAMiscUtils final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FGameplayTagContainer GetMissingTags(const FGameplayTagContainer& ContainerA, const FGameplayTagContainer& ContainerB);
};
