// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWPartyFrameItem.generated.h"

class UCommonTextBlock;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWPartyFrameItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerNameLabel;

	virtual void NativeOnInitialized() override;

	virtual void SetPlayerState(TObjectPtr<APlayerState> InPlayerState);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="SetPlayerState"))
	void BP_SetPlayerState(APlayerState* InPlayerState);

	UFUNCTION()
	void ReadPlayerName();
};
