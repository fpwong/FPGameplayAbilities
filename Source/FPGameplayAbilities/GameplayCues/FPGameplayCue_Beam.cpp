// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGameplayCue_Beam.h"

#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

bool UFPGameplayCue_Beam::HandlesEvent(EGameplayCueEvent::Type EventType) const
{
	return (EventType == EGameplayCueEvent::Executed);
}

void UFPGameplayCue_Beam::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(MyTarget, Sound, MyTarget->GetActorLocation());
	}

	SpawnParticle(MyTarget, Parameters);
}

void UFPGameplayCue_Beam::SpawnParticle(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	FRotator Rotation = FRotator::ZeroRotator;

	FVector StartLocation = Parameters.Location;

	FVector EndLocation = FVector::ZeroVector;
	if (const FHitResult* HitResult = Parameters.EffectContext.GetHitResult())
	{
		EndLocation = HitResult->Location;
	}

	if (BeamNiagaraSystem)
	{
		UNiagaraComponent* NewSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BeamNiagaraSystem, StartLocation, Rotation, ParticleSystemScale, true);
		NewSystem->SetVectorParameter("StartLocation", StartLocation);
		NewSystem->SetVectorParameter("EndLocation", EndLocation);
	}

	if (HitNiagaraSystem)
	{
		if (bRotateHitTowardsDir)
		{
			FVector Dir = EndLocation - StartLocation;
			Rotation = Dir.Rotation();
		}

		UNiagaraComponent* NewSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagaraSystem, EndLocation, Rotation, ParticleSystemScale, true);
	}
}

void UFPGameplayCue_Beam::OnParticleSpawned_Implementation(UNiagaraComponent* Niagara, UParticleSystemComponent* Cascade)
{
}

FGameplayCueParameters UFPGameplayCue_Beam::MakeBeamCueParams(FVector Start, FVector End)
{
	FGameplayCueParameters Params;
	Params.Location = Start;

	FHitResult EndHitResult;
	EndHitResult.Location = End;
	Params.EffectContext = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
	Params.EffectContext.AddHitResult(EndHitResult);

	return Params;
}
