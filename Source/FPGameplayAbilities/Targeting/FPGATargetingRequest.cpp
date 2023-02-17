// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGATargetingRequest.h"

FFPGATargetingRequest FFPGATargetingRequest::MakeTargetingRequest(const FFPGATargetingRequirements& TargetingRequirements, AActor* InRequestingActor)
{
	FFPGATargetingRequest Request;
	Request.Requirements = TargetingRequirements;
	Request.RequestingActor = InRequestingActor;
	return Request;
}

bool FFPGATargetingRequest::IsValidEventData() const
{
	return Requirements.IsValidTargetData(RequestingActor.Get(), TargetData);
}

bool FFPGATargetingRequest::DoesRequireTargeting() const
{
	return TargetData.Num() < Requirements.Stages.Num();
}

bool FFPGATargetingRequest::TryApplyingHitResults(const TArray<FHitResult>& HitResults)
{
	if (!DoesRequireTargeting())
	{
		return true;
	}

	for (const FHitResult& HitResult : HitResults)
	{
		if (TryApplyingHitResult(HitResult))
		{
			if (!DoesRequireTargeting())
			{
				OnRequestCompleted.Broadcast();
				return true;
			}
		}
	}

	return false;
}

bool FFPGATargetingRequest::TryApplyingHitResult(const FHitResult& HitResult)
{
	if (!DoesRequireTargeting())
	{
		return true;
	}

	if (!RequestingActor.IsValid())
	{
		return false;
	}

	const FFPGATargetingStage& Stage = GetCurrentStage();
	if (FGameplayAbilityTargetData* Data = Stage.MakeTargetDataFromHitResult(RequestingActor.Get(), HitResult))
	{
		TargetData.Add(Data);

		if (!DoesRequireTargeting())
		{
			OnRequestCompleted.Broadcast();
			return true;
		}
	}

	return false;
}

const FFPGATargetingStage& FFPGATargetingRequest::GetCurrentStage() const
{
	check(DoesRequireTargeting());
	const int StageIndex = TargetData.Num();
	return Requirements.Stages[StageIndex];
}
