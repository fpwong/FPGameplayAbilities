// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAPlayerController.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemInterface.h"
#include "Widgets/Text/ISlateEditableTextWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/HUD.h"

AFPGAPlayerController::AFPGAPlayerController()
{
	bShowMouseCursor = true;
	bEnableTouchEvents = false;
}

void AFPGAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);
}

bool AFPGAPlayerController::GetHitResultsUnderCursor(const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, TArray<FHitResult>& OutHits)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
		{
			FVector WorldOrigin;
			FVector WorldDirection;
			if (UGameplayStatics::DeprojectScreenToWorld(this, MousePosition, WorldOrigin, WorldDirection) == true)
			{
				// Early out if we clicked on a HUD hitbox
				if (GetHUD() != nullptr && GetHUD()->GetHitBoxAtCoordinates(MousePosition, true))
				{
					return false;
				}

				FCollisionObjectQueryParams const ObjParam(ObjectTypes);
				return GetWorld()->LineTraceMultiByObjectType(OutHits, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjParam,
				                                              FCollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), false));
			}
		}
	}

	return false;
}
