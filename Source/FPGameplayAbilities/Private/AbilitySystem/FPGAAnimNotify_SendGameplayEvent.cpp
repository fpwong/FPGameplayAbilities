// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAAnimNotify_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UFPGAAnimNotify_SendGameplayEvent::UFPGAAnimNotify_SendGameplayEvent()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 200, 200, 255);
	bShouldFireInEditor = false;
#endif // WITH_EDITORONLY_DATA

	// use accurate timing for gameplay events!
	bIsNativeBranchingPoint = true;
}

void UFPGAAnimNotify_SendGameplayEvent::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	ContainingAnimNotifyEvent.MontageTickType = EMontageNotifyTickType::BranchingPoint;
}

void UFPGAAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}

void UFPGAAnimNotify_SendGameplayEvent::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);
}
