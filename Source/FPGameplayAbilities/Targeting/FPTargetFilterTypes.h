#pragma once

#include "CoreMinimal.h"

struct FGameplayTag;

DECLARE_MULTICAST_DELEGATE_TwoParams(FFPTargetFilterResultChanged, AActor*, bool);

typedef TMap<FGameplayTag, UObject*> FFPTargetFilterContextMapping;

UENUM(BlueprintType)
enum class EFPTargetFilterTaskType : uint8
{
	/* Causes the task to fail */
	FailureTask,

	/* Causes the task to pass */
	PassingTask,

	// /* Doesn't affect pass fail state, but decides whether to check child tasks */
	// ConditionalTask,
};

UENUM(BlueprintType)
enum class EFPTargetFilterTaskContext : uint8
{
	Source,
	Target
};

// USTRUCT(BlueprintType)
// struct FPGAMEPLAYABILITIES_API FFPTargetFilterTaskContext
// {
// 	GENERATED_BODY()
//
// 	TWeakObjectPtr<AActor> SourceActor;
//
// 	TWeakObjectPtr<AActor> TargetActor;
// };
