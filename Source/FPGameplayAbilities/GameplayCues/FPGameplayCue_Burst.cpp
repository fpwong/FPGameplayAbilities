// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGameplayCue_Burst.h"

#include "AbilitySystemGlobals.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

bool UFPGameplayCue_Burst::HandlesEvent(EGameplayCueEvent::Type EventType) const
{
	return (EventType == EGameplayCueEvent::Executed);
}

void UFPGameplayCue_Burst::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(MyTarget, Sound, MyTarget->GetActorLocation());
	}

	SpawnParticle(MyTarget, Parameters);
}

void UFPGameplayCue_Burst::SpawnParticle(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (bAttachEmitter && MyTarget)
	{
		if (CascadeSystem)
		{
			UParticleSystemComponent* NewSystem = UGameplayStatics::SpawnEmitterAttached(CascadeSystem, MyTarget->GetRootComponent(), AttachSocketName);
			OnParticleSpawned(nullptr, NewSystem);
		}

		if (NiagaraSystem)
		{
			UNiagaraComponent* NewSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NiagaraSystem, MyTarget->GetRootComponent(), AttachSocketName,
				FVector::ZeroVector, FRotator::ZeroRotator, ParticleSystemScale,
				EAttachLocation::Type::SnapToTarget, true, ENCPoolMethod::None);
			OnParticleSpawned(NewSystem, nullptr);
		}
	}
	else
	{
		FVector Location;
		FRotator Rotation = FRotator::ZeroRotator;
		if (const FHitResult* HitResult = Parameters.EffectContext.GetHitResult())
		{
			Location = HitResult->ImpactPoint;
			Rotation = HitResult->ImpactNormal.Rotation();
		}
		else
		{
			if (MyTarget && !bUseLocationParam)
			{
				Location = MyTarget->GetActorLocation();
				Rotation = Parameters.Normal.Rotation();
			}
			else
			{
				Location = Parameters.Location;
				Rotation = Parameters.Normal.Rotation();
			}
		}

		// DrawDebugSphere(MyTarget->GetWorld(), Location, 100.0f, 8, FColor::Red, true, 1.0f);

		if (CascadeSystem)
		{
			UParticleSystemComponent* NewSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CascadeSystem, Location, Rotation, ParticleSystemScale, true);
			OnParticleSpawned(nullptr, NewSystem);
		}

		if (NiagaraSystem)
		{
			UNiagaraComponent* NewSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, Location, Rotation, ParticleSystemScale, true);
			OnParticleSpawned(NewSystem, nullptr);
		}
	}
}

void UFPGameplayCue_Burst::OnParticleSpawned_Implementation(UNiagaraComponent* Niagara, UParticleSystemComponent* Cascade)
{
}

FGameplayCueParameters UFPGameplayCue_Burst::MakeLocationGameplayCueParams(FVector Location, FRotator Rotation)
{
	// this is so dumb...
	FGameplayCueParameters Params;
	FHitResult DummyHitResult;
	DummyHitResult.Location = Location;
	DummyHitResult.Normal = Rotation.Vector();
	DummyHitResult.ImpactPoint = DummyHitResult.Location;
	DummyHitResult.ImpactNormal = DummyHitResult.Normal;
	Params.EffectContext = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
	Params.EffectContext.AddHitResult(DummyHitResult);
	return Params;
}
