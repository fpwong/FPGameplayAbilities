// Copyright 2020 Dan Kestranek.

#include "AbilitySystem/Tasks/FPGAAsyncTask_TagChanged.h"
#include "Kismet/KismetSystemLibrary.h"

UFPGAAsyncTask_TagChanged* UFPGAAsyncTask_TagChanged::ListenForTagChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag InTag, EGameplayTagEventType::Type InEventType)
{
	UFPGAAsyncTask_TagChanged* ListenForTagChange = NewObject<UFPGAAsyncTask_TagChanged>();
	ListenForTagChange->ASC = AbilitySystemComponent;
	ListenForTagChange->Tag = InTag;

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(InTag, InEventType).AddUObject(ListenForTagChange, &UFPGAAsyncTask_TagChanged::TagChanged);
	}

	return ListenForTagChange;
}

void UFPGAAsyncTask_TagChanged::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->RegisterGameplayTagEvent(Tag, EventType).RemoveAll(this);		
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UFPGAAsyncTask_TagChanged::TagChanged(const FGameplayTag TagChanged, int32 NewCount)
{
	// if (NewCount == 0)
	{
		OnTagChanged.Broadcast(ASC, TagChanged, NewCount);
	}
}
