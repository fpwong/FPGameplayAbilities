// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/TargetData/FPGATargetActor_Cursor.h"

#include "GameFramework/Pawn.h"
#include "WorldCollision.h"
#include "Abilities/GameplayAbility.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "FPGAPlayerController.h"

AFPGATargetActor_Cursor::AFPGATargetActor_Cursor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	ShouldProduceTargetDataOnServer = false;
	bDestroyOnConfirmation = false;
	CurrentlyHovered = nullptr;
	TargetType = FPGA_Target_Actor;
	//Affiliation = ETeamAttitude::Hostile;
}

void AFPGATargetActor_Cursor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (ReticleClass)
	{
		AGameplayAbilityWorldReticle* SpawnedReticleActor = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, GetActorLocation(), GetActorRotation());
		if (SpawnedReticleActor)
		{
			if (APlayerController* PC = Cast<APlayerController>(MasterPC))
			{
				SpawnedReticleActor->InitializeReticle(this, PC, ReticleParams);
				ReticleActor = SpawnedReticleActor;

				// This is to catch cases of playing on a listen server where we are using a replicated reticle actor.
				// (In a client controlled player, this would only run on the client and therefor never replicate. If it runs
				// on a listen server, the reticle actor may replicate. We want consistancy between client/listen server players.
				// Just saying 'make the reticle actor non replicated' isnt a good answer, since we want to mix and match reticle
				// actors and there may be other targeting types that want to replicate the same reticle actor class).
				if (!ShouldProduceTargetDataOnServer)
				{
					SpawnedReticleActor->SetReplicates(false);
				}
			}
		}
	}
}

void AFPGATargetActor_Cursor::ConfirmTargetingAndContinue()
{
	// Run on server and launching client only
	if (!OwningAbility)
		return;

	FHitResult Hit = GetCursorHit();
	if (Hit.IsValidBlockingHit())
	{
		TargetDataReadyDelegate.Broadcast(MakeTargetDataHandleFromHitResult(Hit));
	}
	else
	{
		CancelTargeting();
	}
}

void AFPGATargetActor_Cursor::ConfirmTargeting()
{
	FHitResult Hit = GetCursorHit();
	if (Hit.bBlockingHit)
	{
		if (CurrentlyHovered)
		{
			CurrentlyHovered->GetMesh()->SetRenderCustomDepth(false);
			CurrentlyHovered = nullptr;
		}
	}

	Super::ConfirmTargeting();
}

void AFPGATargetActor_Cursor::Tick(float DeltaSeconds)
{
	FHitResult Hit = GetCursorHit();

	if (bDebug)
		DrawDebugFromHit(Hit);

	ACharacter* HoveredCharacter = Cast<ACharacter>(Hit.Actor);

	// Disable outline on currently hovered
	if (CurrentlyHovered != nullptr)
		CurrentlyHovered->GetMesh()->SetRenderCustomDepth(false);

	// Enable outline on new hovered
	if (HoveredCharacter != nullptr)
		HoveredCharacter->GetMesh()->SetRenderCustomDepth(true);

	// Set currently hovered
	CurrentlyHovered = HoveredCharacter;

	if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	{
		LocalReticleActor->SetIsTargetValid(Hit.bBlockingHit);

		if (TargetType == FPGA_Target_Actor)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				ReticleActor->SetIsTargetAnActor(true);
				LocalReticleActor->SetActorLocation(HitActor->GetActorLocation());
			}
		}
		else if (TargetType == FPGA_Target_Ground)
		{
			ReticleActor->SetIsTargetAnActor(false);
			LocalReticleActor->SetActorLocation(Hit.Location);
		}
	}
}

FHitResult AFPGATargetActor_Cursor::GetCursorHit()
{
	APawn* Pawn = Cast<APawn>(SourceActor);

	check(Pawn);

	AFPGAPlayerController* PlayerController = Cast<AFPGAPlayerController>(Pawn->GetController());

	if (PlayerController == nullptr)
		return FHitResult();

	//check(PlayerController);

	TArray<FHitResult> HitResults;
	PlayerController->GetHitResultsUnderCursor(ObjectTypes, HitResults);

	for (FHitResult HitResult : HitResults)
	{
		if (TargetType == FPGA_Target_Actor && Filter.FilterPassesForActor(HitResult.GetActor()))
		{
			return HitResult;
		}

		if (TargetType == FPGA_Target_Ground)
		{
			return HitResult;
		}
	}

	return FHitResult();

	/*IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(HitResult.GetActor());
	if (AbilitySystemInterface != nullptr)
	{
		UAbilitySystemComponent* AbilitySystem = AbilitySystemInterface->GetAbilitySystemComponent();
		if (AbilitySystem != nullptr)
		{
			const bool bIsTargetDead = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Debuff.Strong.Death"), true));
			if (bIsTargetDead)
			{
				HitResult.Actor = nullptr;
			}
		}
	}*/

	//PlayerController->GetHitResultUnderCursorByChannel(TraceChannel, false, HitResult);
}

FGameplayAbilityTargetDataHandle AFPGATargetActor_Cursor::MakeTargetDataHandleFromHitResult(FHitResult& Hit)
{
	switch (TargetType)
	{
	case FPGA_Target_Actor:
		{
			TArray<TWeakObjectPtr<AActor>> HitActors;
			HitActors.Add(Hit.GetActor());
			return StartLocation.MakeTargetDataHandleFromActors(HitActors);
		}
	case FPGA_Target_Ground:
		{
			FGameplayAbilityTargetingLocationInfo LocationInfo;
			LocationInfo.LiteralTransform = FTransform(Hit.Location);
			return UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(StartLocation, LocationInfo);
		}
	default:
		{
			return FGameplayAbilityTargetDataHandle();
		}
	}
}

void AFPGATargetActor_Cursor::DrawDebugFromHit(FHitResult& Hit)
{
	if (TargetType == FPGA_Target_Actor)
	{
		if (Hit.Actor != nullptr)
		{
			DrawDebugSphere(GetWorld(), Hit.GetActor()->GetActorLocation(), 32, 32, FColor(255, 0, 0));
		}
	}
	else if (TargetType == FPGA_Target_Ground)
	{
		DrawDebugSphere(GetWorld(), Hit.Location, 32, 32, FColor(255, 0, 0));
	}
}

void AFPGATargetActor_Cursor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ReticleActor.IsValid())
	{
		ReticleActor.Get()->Destroy();
	}
}

void AFPGATargetActor_Cursor::CancelTargeting()
{
	if (CurrentlyHovered)
	{
		CurrentlyHovered->GetMesh()->SetRenderCustomDepth(false);
		CurrentlyHovered = nullptr;
	}

	AFPGAGameplayAbilityTargetActor::CancelTargeting();
}
