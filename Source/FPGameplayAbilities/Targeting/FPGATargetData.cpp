#include "FPGATargetData.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "FPGAHelperLibrary.h"
#include "FPGATargetType.h"
#include "FPGATargetTypes.h"
#include "FPTargetFilterLibrary.h"
#include "GameplayTagAssetInterface.h"
#include "FPGameplayAbilities/FPGASettings.h"
#include "FPGameplayAbilities/Utils/FPGAMiscUtils.h"
#include "GameFramework/Actor.h"

FFPGATargetData::FFPGATargetData()
	: Actor(nullptr)
	, Location(FVector2D::ZeroVector)
	, bUseLocation(false)
	, TargetTags(FGameplayTagContainer::EmptyContainer) {}

FFPGATargetData::FFPGATargetData(AActor* InActor,
								const FGameplayTagContainer& InTargetTags)
	: Actor(InActor)
	, Location(FVector2D::ZeroVector)
	, bUseLocation(false)
	, TargetTags(InTargetTags) {}

FFPGATargetData::FFPGATargetData(const FVector2D& InLocation)
	: Actor(nullptr)
	, Location(InLocation)
	, bUseLocation(true)
	, TargetTags(FGameplayTagContainer::EmptyContainer) {}

FFPGATargetData::FFPGATargetData(AActor* InActor, const FVector2D& InLocation, const FGameplayTagContainer& InTargetTags)
	: Actor(InActor)
	, Location(InLocation)
	, TargetTags(InTargetTags)
{
	bUseLocation = InActor == nullptr;
}

FString FFPGATargetData::ToString() const
{
	FString s;
	s += TEXT("(");
	s += TEXT("Actor: ");
	if (Actor != nullptr)
	{
		s += Actor->GetName();
	}

	else
	{
		s += FName().ToString();
	}

	if (bUseLocation)
	{
		s += TEXT(", Location: ");
		s += Location.ToString();
	}

	s += TEXT(", Tags:( ");
	s += TargetTags.ToString();
	s += TEXT(")");
	s += TEXT(")");

	return s;
}

FVector FFPGATargetData::GetTargetLocation() const
{
	if (HasActor())
	{
		return Actor->GetActorLocation();
	}
	if (HasLocation())
	{
		return FVector(Location, 0);
	}

	return FVector::ZeroVector;
}

bool FFPGATargetData::operator==(const FFPGATargetData& Other) const
{
	if (bUseLocation != Other.bUseLocation)
	{
		return false;
	}

	if (bUseLocation)
	{
		return Location.Equals(Other.Location);
	}

	return Actor == Other.Actor;
}

bool FFPGATargetData::operator!=(const FFPGATargetData& Other) const
{
	return !(*this == Other);
}

FFPGATargetingStage::FFPGATargetingStage()
	: TargetTypeFlags(0)
// , OrderPreviewData()
{
}

bool FFPGATargetingStage::IsValidLocation(AActor* OrderedActor, const FVector& Location) const
{
	const FVector2D Location2D = FVector2D(Location);
	if (MaxRange > 0)
	{
		const FVector2D ToTarget = Location2D - FVector2D(GetSourceLocation(OrderedActor));
		return ToTarget.SizeSquared() < MaxRange * MaxRange;
	}

	return true;
}

bool FFPGATargetingStage::IsValidTargetData(const TArray<FFPGATargetData>& OrderData, AActor* OrderedActor, const FFPGATargetData* TargetData) const
{
	if (!TargetData)
	{
		return false;
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::NONE | EFPGATargetTypeFlags::PASSIVE))
	{
		return true;
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
	{
		if (TargetData->Actor)
		{
			if (IsValidActor(OrderedActor, TargetData->Actor))
			{
				return true;
			}
		}
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::LOCATION | EFPGATargetTypeFlags::DIRECTION))
	{
		if (TargetData->HasLocation())
		{
			if (MaxRange > 0)
			{
				FVector2D ToTarget = TargetData->Location - FVector2D(GetSourceLocation(OrderedActor));
				return ToTarget.SizeSquared() < MaxRange * MaxRange;
			}

			return true;
		}
	}

	return false;
}

bool FFPGATargetingStage::IsValidTargetData(AActor* OrderedActor, const FGameplayAbilityTargetData* TargetData, OUT FGameplayTagContainer* OutFailureTags) const
{
	if (!TargetData)
	{
		return false;
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::NONE | EFPGATargetTypeFlags::PASSIVE))
	{
		return true;
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
	{
		const TArray<TWeakObjectPtr<AActor>>& Actors = TargetData->GetActors();
		if (Actors.Num())
		{
			if (AActor* Actor = Actors[0].Get())
			{
				if (IsValidActor(OrderedActor, Actor, OutFailureTags))
				{
					return true;
				}

				return false;
			}
		}
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::LOCATION | EFPGATargetTypeFlags::DIRECTION))
	{
		if (TargetData->HasEndPoint())
		{
			if (IsValidLocation(OrderedActor, TargetData->GetEndPoint()))// && SourceFilter.DoesFilterPass(nullptr, OrderedActor))
			{
				return true;
			}
		}
	}

	return false;
}

// EFPTargetValidResult FFPGATargetingStage::IsValidTargetDataWithResult(AActor* OrderedActor, const FGameplayAbilityTargetData* TargetData, OUT FGameplayTagContainer* OutFailureTags) const
// {
// 	if (!TargetData)
// 	{
// 		return EFPTargetValidResult::Invalid;
// 	}
//
// 	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::NONE | EFPGATargetTypeFlags::PASSIVE))
// 	{
// 		return EFPTargetValidResult::Valid;
// 	}
//
// 	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
// 	{
// 		const TArray<TWeakObjectPtr<AActor>>& Actors = TargetData->GetActors();
// 		if (Actors.Num())
// 		{
// 			if (AActor* Actor = Actors[0].Get())
// 			{
// 				if (IsValidActorCore(OrderedActor, Actor))
// 				{
// 					if (!IsValidActor(OrderedActor, Actor, OutFailureTags))
// 					{
// 						UE_LOG(LogTemp, Warning, TEXT("Target is blocked"));
// 						return EFPTargetValidResult::Blocked;
// 					}
//
// 					UE_LOG(LogTemp, Warning, TEXT("Target is valid!"));
// 					return EFPTargetValidResult::Valid;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("No actors found"));
// 		}
// 	}
//
// 	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::LOCATION | EFPGATargetTypeFlags::DIRECTION))
// 	{
// 		if (TargetData->HasEndPoint())
// 		{
// 			if (IsValidLocation(OrderedActor, TargetData->GetEndPoint()) && SourceFilter.DoesFilterPass(nullptr, OrderedActor))
// 			{
// 				return EFPTargetValidResult::Valid;
// 			}
// 		}
// 	}
//
// 	return EFPTargetValidResult::Valid;
// }

bool FFPGATargetingStage::IsValidHitResult(AActor* OrderedActor, const FHitResult& HitResult) const
{
	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::NONE | EFPGATargetTypeFlags::PASSIVE))
	{
		return true;
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
	{
		if (AActor* Actor = HitResult.GetActor())
		{
			return IsValidActor(OrderedActor, Actor);
		}
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::LOCATION | EFPGATargetTypeFlags::DIRECTION))
	{
		if (HitResult.IsValidBlockingHit())
		{
			if (IsValidLocation(OrderedActor, HitResult.Location))
			{
				return true;
			}
		}
	}

	return false;
}

bool FFPGATargetingStage::IsValidActor(AActor* SourceActor, AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags) const
{
	// TODO check max range?

	if (TargetActor && UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
	{
		return TargetFilterTaskSet.DoesFilterPass(SourceActor, TargetActor, OutFailureTags);
	}

	return false;
}

bool FFPGATargetingStage::IsTargetTypeFlagChecked(int32 InFlag) const
{
	return UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, InFlag);
}

bool FFPGATargetingStage::IsTargetTypeFlagChecked(EFPGATargetTypeFlags InFlag) const
{
	return UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, InFlag);
}

FGameplayAbilityTargetData* FFPGATargetingStage::MakeTargetDataFromHitResult(AActor* SourceActor, const FHitResult& HitResult, OUT FGameplayTagContainer* OutFailureTags) const
{
	FGameplayTagContainer FailureTags;
	if (IsTargetTypeFlagChecked(EFPGATargetTypeFlags::NONE | EFPGATargetTypeFlags::PASSIVE))
	{
		return nullptr;
	}

	if (IsTargetTypeFlagChecked(EFPGATargetTypeFlags::ACTOR))
	{
		if (AActor* Actor = HitResult.GetActor())
		{
			if (IsValidActor(SourceActor, Actor, &FailureTags))
			{
				return FFPGATargetData_SingleActor::MakeSingleActorTargetData(Actor);
			}
		}
	}

	if (IsTargetTypeFlagChecked(EFPGATargetTypeFlags::LOCATION | EFPGATargetTypeFlags::DIRECTION))
	{
		if (HitResult.IsValidBlockingHit())
		{
			// if (SourceFilter.DoesFilterPass(nullptr, SourceActor))
			// {
			return FFPGATargetData_Vector::MakeVectorTargetData(HitResult.Location);
			// }
		}
	}

	if (OutFailureTags)
	{
		OutFailureTags->AppendTags(FailureTags);
	}

	return nullptr;
}

FVector FFPGATargetingStage::GetSourceLocation(AActor* SourceActor) const
{
	switch (TargetRangeSource)
	{
		case EFPGATargetRangeSource::UNIT:
		{
			return SourceActor->GetActorLocation();
		}
		case EFPGATargetRangeSource::LAST_TARGET:
		{
			// const int32 PreviousIndex = TargetData.Num() - 1;
			// if (TargetData.IsValidIndex(PreviousIndex))
			// {
			// 	const FFPGATargetData& LastData = TargetData[PreviousIndex];
			// 	return LastData.GetTargetLocation();
			// }
			break;
		}
		case EFPGATargetRangeSource::CUSTOM:
			// TODO: Implement this
			//check(0);
			// UE_LOG(LogFPGA, Warning, TEXT("Custom source location not implemented!"));
			return FVector::ZeroVector;
	}

	return FVector::ZeroVector;
}

bool FFPGATargetingRequirements::IsValidTargetData(AActor* OrderedActor, const FGameplayAbilityTargetDataHandle& TargetData) const
{
	for (int i = 0; i < Stages.Num(); ++i)
	{
		const FFPGATargetingStage& Stage = Stages[i];
		if (const FGameplayAbilityTargetData* Data = TargetData.Get(i))
		{
			if (!Stage.IsValidTargetData(OrderedActor, Data))
			{
				// UE_LOG(LogTemp, Warning, TEXT("Target invalid for stage %d"), i);
				return false;
			}
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("Missing data for stage %d"), i);
			return false;
		}
	}

	return true;
}
