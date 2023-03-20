// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterTask_FilterPreset.h"

#include "AbilitySystemComponent.h"
#include "FPTargetFilterPreset.h"
#include "FPTargetFilterTaskSetObserver.h"

bool UFPTargetFilterTask_FilterPreset::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
{
	return Preset->DoesFilterPass(SourceActor, TargetActor);
}

FFPTargetFilterObserver* UFPTargetFilterTask_FilterPreset::MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	FFPTargetFilterObserver_FilterPreset* Binding = new FFPTargetFilterObserver_FilterPreset();
	Binding->InitFilterPreset(Cast<UFPTargetFilterTask_FilterPreset>(FilterTask), SourceActor, TargetActor);
	return Binding;
}

void FFPTargetFilterObserver_FilterPreset::InitFilterPreset(UFPTargetFilterTask_FilterPreset* Task, AActor* SourceActor, AActor* TargetActor)
{
	Init(Task, SourceActor, TargetActor);

	TaskSetObserver = MakeShared<FFPTargetFilterTaskSetObserver>();
	TaskSetObserver->Bind(Task->Preset->TaskSet, SourceActor, TargetActor);
	TaskSetObserver->OnResultChanged.AddRaw(this, &FFPTargetFilterObserver_FilterPreset::HandleResultChanged);
}

void FFPTargetFilterObserver_FilterPreset::HandleResultChanged(AActor*, bool)
{
	CheckResultChanged();
}
