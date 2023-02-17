#include "FPGAUWPartyFrameItem.h"

#include "CommonTextBlock.h"
#include "GameFramework/PlayerState.h"

void UFPGAUWPartyFrameItem::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UFPGAUWPartyFrameItem::SetPlayerState(TObjectPtr<APlayerState> InPlayerState)
{
	PlayerState = InPlayerState;

	// if (* LobbyPlayerState = Cast<AFPLobbyPlayerState>(InPlayerState))
	// {
	// 	LobbyPlayerState->OnPlayerNameChanged.AddUObject(this, &UFPUWPartyFrameItem::HandlePlayerNameChanged);
	// }

	if (PlayerNameLabel && InPlayerState)
	{
		FString PlayerName = InPlayerState->GetPlayerName();

		PlayerNameLabel->SetText(FText::FromString(PlayerName.IsEmpty() ? "Unnamed" : PlayerName));
	}
}

void UFPGAUWPartyFrameItem::HandlePlayerNameChanged(const FString& PlayerName)
{
	if (PlayerNameLabel)
	{
		PlayerNameLabel->SetText(FText::FromString(PlayerName));
	}
}
