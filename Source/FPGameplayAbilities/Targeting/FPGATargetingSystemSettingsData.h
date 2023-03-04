// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGATargetFilter.h"
#include "FPGATargetingSystemSettingsData.generated.h"

USTRUCT()
struct FFPGAHoveredCursor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FFPGATargetFilter Filter;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EMouseCursor::Type> CursorType = EMouseCursor::Type::Default;
};

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGATargetingSystemSettingsData final : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> ClickTraceChannel;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ETraceTypeQuery> GroundTraceChannel;

	// only change hover state for these actors
	UPROPERTY(EditDefaultsOnly)
	FFPGATargetFilter HoveredFilter;

	// change the cursor when the hovered actor matches the filter
	UPROPERTY(EditDefaultsOnly)
	TArray<FFPGAHoveredCursor> HoveredCursors;
};
