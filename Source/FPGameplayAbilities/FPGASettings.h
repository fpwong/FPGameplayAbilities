#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "Targeting/FPGATargetFilter.h"
#include "FPGASettings.generated.h"

/**
* Settings are saved in Config/DefaultGame.ini
*/
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "FP Gameplay Abilities Settings"))
class FPGAMEPLAYABILITIES_API UFPGASettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Targeting")
	FFPGATargetFilter DefaultStageTargetFilter;

	UPROPERTY(Config, EditAnywhere, Category = "Targeting")
	FFPGATargetFilter DefaultStageSourceFilter;

	UPROPERTY(config, EditAnywhere, Category = "Targeting")
	TSoftObjectPtr<UDataTable> StaticTargetFiltersTable;

	UPROPERTY(config, EditAnywhere, Category = "Targeting")
	TSoftObjectPtr<UDataAsset> TargetingSettingsData;
};