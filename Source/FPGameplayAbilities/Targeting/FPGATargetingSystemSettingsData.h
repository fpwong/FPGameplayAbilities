// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilter.h"
#include "FPGATargetingSystemSettingsData.generated.h"

USTRUCT()
struct FFPGAHoveredCursor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FFPTargetFilter TargetFilter;

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

	UPROPERTY(EditDefaultsOnly)
	FFPTargetFilter HoveredTargetFilter;

	// change the cursor when the hovered actor matches the filter
	UPROPERTY(EditDefaultsOnly)
	TArray<FFPGAHoveredCursor> HoveredCursors;
};
