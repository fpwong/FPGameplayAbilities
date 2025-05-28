// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWPartyFrame.generated.h"

class UCommonButtonBase;
class UFPGAUWPartyFrameItem;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWPartyFrame : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	virtual void RefreshPlayers();

	UFUNCTION()
	void HandlePlayerLoginOrLogout(APlayerState* PlayerState);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGAUWPartyFrameItem> LobbyItemType;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> PartyFramePanel;
};
