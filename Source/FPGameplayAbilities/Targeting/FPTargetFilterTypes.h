#pragma once

DECLARE_MULTICAST_DELEGATE_TwoParams(FFPTargetFilterResultChanged, AActor*, bool);

UENUM(BlueprintType)
enum class EFPTargetFilterTaskType : uint8
{
	/* Causes the task to fail */
	FailureTask,

	/* Causes the task to pass */
	PassingTask,
};
