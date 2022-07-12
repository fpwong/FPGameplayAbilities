#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "FPGATargetTypes.generated.h"

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGATargetData_Vector : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	static FFPGATargetData_Vector* MakeVectorTargetData(const FVector& Vector);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	FVector_NetQuantize TargetLocation;

	virtual bool HasEndPoint() const override
	{
		return true;
	}

	virtual FVector GetEndPoint() const override
	{
		return TargetLocation;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FFPGATargetData_Vector::StaticStruct();
	}

	virtual FString ToString() const override
	{
		return TEXT("FFPGATargetData_Vector");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FFPGATargetData_Vector> : public TStructOpsTypeTraitsBase2<FFPGATargetData_Vector>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FFPGATargetData_Vector net serialization to work
	};
};