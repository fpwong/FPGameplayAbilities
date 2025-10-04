// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGameplayCue_Status.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

bool AFPGameplayCue_Status::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	const float TargetHeight = MyTarget->GetSimpleCollisionHalfHeight();
	const FVector Offset(0, 0, TargetHeight * RelativeHeightOffset);

	// for now assume all status effects simply attach to the target
	if (CascadeSystem)
	{
		ActiveCascadeSystem = UGameplayStatics::SpawnEmitterAttached(CascadeSystem, MyTarget->GetRootComponent(), AttachSocketName, Offset);
		OnParticleSpawned(nullptr, ActiveCascadeSystem.Get());
	}

	if (NiagaraSystem)
	{
		ActiveNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraSystem, MyTarget->GetRootComponent(), AttachSocketName,
			Offset, FRotator::ZeroRotator, ParticleSystemScale,
			EAttachLocation::Type::SnapToTarget, true, ENCPoolMethod::None);
		OnParticleSpawned(ActiveNiagaraComponent.Get(), nullptr);
	}

	if (Sound)
	{
		ActiveAudioComponent = UGameplayStatics::SpawnSoundAttached(Sound, MyTarget->GetRootComponent(), AttachSocketName);
	}

	return Super::OnActive_Implementation(MyTarget, Parameters);
}

bool AFPGameplayCue_Status::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (ActiveCascadeSystem.IsValid())
	{
		ActiveCascadeSystem->bAutoDestroy = true; 
		ActiveCascadeSystem->Deactivate();
	}

	if (ActiveNiagaraComponent.IsValid())
	{
		ActiveNiagaraComponent->SetAutoDestroy(true);
		ActiveNiagaraComponent->Deactivate();
	}

	if (ActiveAudioComponent.IsValid())
	{
		ActiveAudioComponent->bAutoDestroy = true;
		ActiveAudioComponent->Deactivate();
	}

	return Super::OnRemove_Implementation(MyTarget, Parameters);
}

void AFPGameplayCue_Status::OnParticleSpawned_Implementation(UNiagaraComponent* Niagara, UParticleSystemComponent* Cascade)
{
}
