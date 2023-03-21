// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGATargetingSubsystem.h"

#include "FPGAPlayerFocusComponent.h"
#include "FPGameplayAbilities/FPGASettings.h"
#include "Kismet/GameplayStatics.h"

UFPGATargetingSubsystem* UFPGATargetingSubsystem::Get(const UWorld* World)
{
	return World ? Get(World->GetFirstLocalPlayerFromController()) : nullptr;
}

UFPGATargetingSubsystem* UFPGATargetingSubsystem::Get(const ULocalPlayer* LocalPlayer)
{
	return LocalPlayer ? LocalPlayer->GetSubsystem<UFPGATargetingSubsystem>() : nullptr;
}

void UFPGATargetingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SettingsData = Cast<UFPGATargetingSystemSettingsData>(GetDefault<UFPGASettings>()->TargetingSettingsData.LoadSynchronous());

	HoveredCursorState = EMouseCursor::Default;
}

void UFPGATargetingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UFPGATargetingSubsystem::StartTargetingRequest(TSharedPtr<FFPGATargetingRequest> Request)
{
	CurrentRequest = Request;
	GetLocalPlayer()->PlayerController->CurrentMouseCursor = EMouseCursor::Crosshairs;
}

bool UFPGATargetingSubsystem::ConfirmTargeting(const TArray<FHitResult>& HitResults)
{
	if (CurrentRequest.IsValid())
	{
		if (CurrentRequest.Pin()->TryApplyingHitResults(HitResults))
		{
			// OnTargetingEnd.Broadcast(CurrentRequest.Pin()->TargetData);

			CancelTargetingRequest();
			return true;
		}
	}

	return false;
}

void UFPGATargetingSubsystem::CancelTargetingRequest()
{
	CurrentRequest.Reset();
	SetMouseCursor(HoveredCursorState);
}

void UFPGATargetingSubsystem::Tick(float DeltaTime)
{
	// if (CurrentRequest)
	// {
	// 	// DrawDebugSphere();
	// }
}

bool UFPGATargetingSubsystem::IsTickable() const
{
	return !HasAnyFlags(RF_BeginDestroyed) && IsValidChecked(this);
}

bool UFPGATargetingSubsystem::GetHitResultsUnderCursor(TArray<FHitResult>& OutHitResults, float HitResultTraceDistance)
{
	check(SettingsData != nullptr);

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	// Get local player viewport
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer || !LocalPlayer->ViewportClient)
	{
		return false;
	}

	auto PlayerController = LocalPlayer->PlayerController;
	if (!PlayerController)
	{
		return false;
	}


	// Get mouse position
	FVector2D MousePosition;
	if (!LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
	{
		return false;
	}

	// Get ray
	FVector WorldOrigin;
	FVector WorldDirection;
	if (!UGameplayStatics::DeprojectScreenToWorld(PlayerController, MousePosition, WorldOrigin, WorldDirection))
	{
		return false;
	}

	World->LineTraceMultiByChannel(
		OutHitResults,
		WorldOrigin,
		WorldOrigin + WorldDirection * HitResultTraceDistance,
		SettingsData->ClickTraceChannel
	);

	// finally check for the ground
	FHitResult GroundHitResult;
	if (PlayerController->GetHitResultUnderCursorByChannel(SettingsData->GroundTraceChannel, false, GroundHitResult))
	{
		OutHitResults.Sort([&GroundHitResult](const FHitResult& A, const FHitResult& B)
		{
			return FVector::DistSquaredXY(A.Location, GroundHitResult.Location) < FVector::DistSquaredXY(B.Location, GroundHitResult.Location);
		});

		GroundHitResult.HitObjectHandle = FActorInstanceHandle();
		OutHitResults.Add(GroundHitResult);
	}

	return true;
}

void UFPGATargetingSubsystem::UpdatePlayerFocus(const TArray<FHitResult>& HitResults)
{
	check(SettingsData != nullptr); 

	for (const auto& HitResult : HitResults)
	{
		if (AActor* NewHovered = HitResult.GetActor())
		{
			if (SettingsData->HoveredFilter.DoesFilterPass(nullptr, NewHovered))
			{
				AActor* OldHovered = HoveredActor.Get();
				if (OldHovered == NewHovered)
				{
					return;
				}

				if (OldHovered && OldHovered->Implements<UFPGAPlayerFocusInterface>())
				{
					IFPGAPlayerFocusInterface::Execute_OnEndHovered(OldHovered);
				}

				if (NewHovered && NewHovered->Implements<UFPGAPlayerFocusInterface>())
				{
					IFPGAPlayerFocusInterface::Execute_OnBeginHovered(NewHovered);
				}

				HoveredActor = NewHovered;
				OnHoveredActorChanged.Broadcast(OldHovered, NewHovered);
				HandleHoveredActorChanged(OldHovered, NewHovered);
				return;
			}
		}
	}

	if (HoveredActor.IsValid())
	{
		AActor* OldHovered = HoveredActor.Get();
		if (OldHovered && OldHovered->Implements<UFPGAPlayerFocusInterface>())
		{
			IFPGAPlayerFocusInterface::Execute_OnEndHovered(OldHovered);
		}

		OnHoveredActorChanged.Broadcast(HoveredActor.Get(), nullptr);
		HoveredActor = nullptr;
		HandleHoveredActorChanged(HoveredActor.Get(), nullptr);
	}
}

void UFPGATargetingSubsystem::SetFocus(AActor* NewFocus)
{
	AActor* OldFocused = FocusedActor.Get();

	if (OldFocused == NewFocus)
	{
		return;
	}

	if (NewFocus && !SettingsData->HoveredFilter.DoesFilterPass(nullptr, NewFocus))
	{
		return;
	}

	if (OldFocused && OldFocused->Implements<UFPGAPlayerFocusInterface>())
	{
		IFPGAPlayerFocusInterface::Execute_OnEndFocus(OldFocused);
	}

	if (NewFocus && NewFocus->Implements<UFPGAPlayerFocusInterface>())
	{
		IFPGAPlayerFocusInterface::Execute_OnBeginFocus(NewFocus);
	}

	FocusedActor = NewFocus;
	OnFocusedActorChanged.Broadcast(OldFocused, NewFocus);
}

void UFPGATargetingSubsystem::FocusHoveredActor()
{
	SetFocus(GetHoveredActor());
}

void UFPGATargetingSubsystem::UpdateMouseCursor()
{
	check(SettingsData != nullptr);
	if (HoveredActor.IsValid())
	{
		for (const FFPGAHoveredCursor& HoveredCursor : SettingsData->HoveredCursors)
		{
			if (HoveredCursor.FilterTaskSet.DoesFilterPass(SourcePawn.Get(), HoveredActor.Get()))
			{
				HoveredCursorState = HoveredCursor.CursorType;
				break;
			}
		}
	}
	else
	{
		HoveredCursorState = EMouseCursor::Default;
	}

	// don't actually update the mouse cursor if we have a targeting request ongoing
	if (!CurrentRequest.IsValid())
	{
		SetMouseCursor(HoveredCursorState);
	}
}

APlayerController* UFPGATargetingSubsystem::GetPlayerController()
{
	return GetLocalPlayer()->PlayerController;
}

void UFPGATargetingSubsystem::HandleHoveredActorChanged(AActor* OldActor, AActor* NewActor)
{
	UpdateMouseCursor();
}

void UFPGATargetingSubsystem::SetMouseCursor(const EMouseCursor::Type Cursor)
{
	if (APlayerController* PC = GetPlayerController())
	{
		if (PC->CurrentMouseCursor != Cursor)
		{
			PC->CurrentMouseCursor = Cursor;
		}
	}
}
