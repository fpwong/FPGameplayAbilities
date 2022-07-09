#pragma once

#include "CoreMinimal.h"
#include "FPGATargetFilter.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "FPGATargetData.generated.h"

enum class EFPGATargetTypeFlags : unsigned char;
class AActor;

/** Targeting data for an order. */
USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGATargetData
{
	GENERATED_BODY()

	FFPGATargetData();
	FFPGATargetData(AActor* InActor, const FGameplayTagContainer& InTargetTags = FGameplayTagContainer::EmptyContainer);
	FFPGATargetData(const FVector2D& InLocation);
	FFPGATargetData(AActor* InActor, const FVector2D& InLocation, const FGameplayTagContainer& InTargetTags = FGameplayTagContainer::EmptyContainer);

	/** The target actor. */
	UPROPERTY(Category = FPGA, EditDefaultsOnly, BlueprintReadWrite)
	AActor* Actor;

	/** The target location. */
	UPROPERTY(Category = FPGA, EditDefaultsOnly, BlueprintReadWrite)
	FVector2D Location;

	// switch to FVector_NetQuantize?

	/** Indicates that 'Location' is valid and should be used by this order. */
	UPROPERTY(Category = FPGA, EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle = true))
	uint8 bUseLocation : 1;

	/** The target tags. */
	UPROPERTY(Category = FPGA, EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTagContainer TargetTags;

	/**
	* Get a textual representation of this order target data.
	* @return A string describing the order target data.
	*/
	FString ToString() const;

	bool IsValid() const { return HasLocation() || HasActor(); }
	bool HasLocation() const { return bUseLocation; }
	bool HasActor() const { return Actor != nullptr; }

	FVector GetTargetLocation() const;

	bool operator==(const FFPGATargetData& Other) const;
	bool operator!=(const FFPGATargetData& Other) const;
};

UENUM()
enum class EFPGATargetRangeSource : uint8
{
	UNIT,
	LAST_TARGET,
	CUSTOM
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGATargetingStage
{
	GENERATED_BODY()

	FFPGATargetingStage();

	UPROPERTY(Category = "FPGA Targeting", EditDefaultsOnly)
	FFPGATargetFilter TargetFilter;

	/**
	* The target types of this stage.
	*/
	UPROPERTY(Category = "FPGA Targeting", EditDefaultsOnly, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = EFPGATargetTypeFlags, AllowPrivateAccess = true))
	int32 TargetTypeFlags;

	// /** Details about the preview for this stage while choosing a target. */
	// UPROPERTY(Category = "FPGA Targeting", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	// FFPGAOrderPreviewData OrderPreviewData;

	UPROPERTY(Category = "FPGA Targeting", EditDefaultsOnly)
	float MaxRange = -1;

	UPROPERTY(Category = "FPGA Targeting", EditDefaultsOnly)
	EFPGATargetRangeSource TargetRangeSource = EFPGATargetRangeSource::UNIT;

	bool IsValidTargetData(const TArray<FFPGATargetData>& OrderData, AActor* OrderedActor, const FFPGATargetData* TargetData) const;
	bool IsValidTargetData(AActor* OrderedActor, const FGameplayAbilityTargetData* TargetData) const;

	bool IsValidHitResult(AActor* OrderedActor, const FHitResult& HitResult) const;

	bool IsValidLocation(AActor* OrderedActor, const FVector& Location) const;
	bool IsValidActor(AActor* OrderedActor, AActor* TargetActor) const;

	bool IsTargetTypeFlagChecked(int32 InFlag) const;
	bool IsTargetTypeFlagChecked(EFPGATargetTypeFlags InFlag) const;

	FGameplayAbilityTargetData* MakeTargetDataFromHitResult(AActor* OrderedActor, const FHitResult& HitResult) const;

	// FVector GetSourceLocation(const TArray<FFPGATargetData>& OrderData, AActor* OrderedActor) const;
	FVector GetSourceLocation(AActor* OrderedActor) const;
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGATargetingRequirements
{
	GENERATED_BODY()

	UPROPERTY(Category = "FPGA Targeting", EditDefaultsOnly)
	TArray<FFPGATargetingStage> Stages;

	bool IsValidTargetData(AActor* OrderedActor, const FGameplayAbilityTargetDataHandle& TargetData) const;
};
