// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGATargetData.h"
#include "UObject/Interface.h"
#include "FPGATargetRequirementsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPGATargetRequirementsInterface : public UInterface
{
	GENERATED_BODY()
};

class FPGAMEPLAYABILITIES_API IFPGATargetRequirementsInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="FPGATargetRequirements")
	FFPGATargetingRequirements GetTargetingRequirements();
};
