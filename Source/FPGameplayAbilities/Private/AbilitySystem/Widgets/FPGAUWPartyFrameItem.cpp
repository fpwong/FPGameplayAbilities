#include "FPGAUWPartyFrameItem.h"

#include "CommonTextBlock.h"
#include "AbilitySystem/FPGATypes.h"
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

	if (IFPPlayerStateInterface* PartyInterface = Cast<IFPPlayerStateInterface>(InPlayerState))
	{
		PartyInterface->GetPlayerNameChangeDelegate().AddUObject(this, &UFPGAUWPartyFrameItem::ReadPlayerName);
	}

	ReadPlayerName();

	BP_SetPlayerState(InPlayerState);
}

void UFPGAUWPartyFrameItem::ReadPlayerName()
{
	if (PlayerNameLabel && PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();
		PlayerNameLabel->SetText(FText::FromString(PlayerName.IsEmpty() ? "Unnamed" : PlayerName));
	}
}