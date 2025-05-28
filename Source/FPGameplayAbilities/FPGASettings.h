#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "AttributeSet.h"
#include "FPGASettings.generated.h"

/**
* Settings are saved in Config/DefaultGame.ini
*/
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "FP Gameplay Abilities Settings"))
class FPGAMEPLAYABILITIES_API UFPGASettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "Targeting")
	TSoftObjectPtr<UDataAsset> TargetingSettingsData;

	UPROPERTY(config, EditAnywhere)
	FGameplayAttribute SkillLevelAttribute;

	UPROPERTY(config, EditAnywhere)
	TSoftObjectPtr<UDataTable> GameValues;

	static const UFPGASettings& Get()
	{
		return *GetDefault<UFPGASettings>();
	}

	static UFPGASettings& GetMutable()
	{
		return *GetMutableDefault<UFPGASettings>();
	}

	UFUNCTION(BlueprintCallable)
	UDataTable* GetGameValuesTable();
};
