#include "FPGAUWPartyFrame.h"

#include "FPGAUWPartyFrameItem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void UFPGAUWPartyFrame::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshPlayers();
}

void UFPGAUWPartyFrame::RefreshPlayers()
{
	if (!PartyFramePanel)
	{
		return;
	}

	if (GetWorld()->bIsTearingDown)
	{
		return;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Refresh players!"));

	PartyFramePanel->ClearChildren();

	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld()))
	{
		for (TObjectPtr<APlayerState> PlayerState : GameState->PlayerArray)
		{
			if (UFPGAUWPartyFrameItem* PartyItem = WidgetTree->ConstructWidget<UFPGAUWPartyFrameItem>(LobbyItemType))
			{
				PartyItem->SetPlayerState(PlayerState);
				PartyFramePanel->AddChild(PartyItem);
				// UE_LOG(LogTemp, Warning, TEXT("Added party frame item!"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("LobbyItemTemplate invalid"));
			}
		}
	}
}

void UFPGAUWPartyFrame::HandleAddOrRemovePlayerState(APlayerState* PlayerState)
{
	RefreshPlayers();
}
