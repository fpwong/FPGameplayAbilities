// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTask.h"
#include "FPTargetFilterObserver.h"
#include "GameplayEffectTypes.h"
#include "FPTargetFilterTask_GameplayTag.generated.h"

struct FFPTargetFilterTaskSetObserver;

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API UFPTargetFilterTask_GameplayTag final : public UFPTargetFilterTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Default)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, Category = Default)
	FGameplayTagContainer BlockedTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default, meta=(Categories="TagGroup"))
	FGameplayTagContainer TagGroups;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	class UFPTagRelationshipMapping* TagRelationshipMapping;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const override;

	FGameplayTagContainer GetAllRelatedTags();

	virtual FFPTargetFilterObserver* MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;
};

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterObserver_GameplayTag : public FFPTargetFilterObserver
{
	GENERATED_BODY()

	virtual ~FFPTargetFilterObserver_GameplayTag() override;

	virtual void Init(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;

	void OnFilterTagChanged(FGameplayTag Tag, int NewCount);

private:
	TMap<FGameplayTag, FDelegateHandle> TagChangedDelegates;
};
