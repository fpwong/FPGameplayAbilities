#include "FPGATargetData.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "FPGAHelperLibrary.h"
#include "FPGATargetType.h"
#include "FPGATargetTypes.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPGameplayAbilities/FPGADeveloperSettings.h"
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
	// TargetFilter.TargetTagQuery = FGameplayTagQuery::BuildQuery(
	// 	FGameplayTagQueryExpression()
	// 	.AllExprMatch()
	// 	.AddExpr(FGameplayTagQueryExpression().AllTagsMatch()
	// 										  .AddTag(UFPGAGlobalTags::Status_Changing_IsAlive())
	// 										  .AddTag(UFPGAGlobalTags::Relationship_Visible()))
	// 	.AddExpr(FGameplayTagQueryExpression().NoTagsMatch()
	// 										  .AddTag(UFPGAGlobalTags::Status_Changing_Invulnerable()))
	// );

	TargetFilter = GetDefault<UFPGADeveloperSettings>()->DefaultStageTargetFilter;
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

bool FFPGATargetingStage::IsValidTargetData(AActor* OrderedActor, const FGameplayAbilityTargetData* TargetData) const
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
				if (IsValidActor(OrderedActor, Actor))
				{
					return true;
				}
			}
		}
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::LOCATION | EFPGATargetTypeFlags::DIRECTION))
	{
		if (TargetData->HasEndPoint())
		{
			if (IsValidLocation(OrderedActor, TargetData->GetEndPoint()))
			{
				return true;
			}
		}
	}

	return false;
}

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

bool FFPGATargetingStage::IsValidActor(AActor* OrderedActor, AActor* TargetActor) const
{
	if (TargetActor && UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
	{
		return TargetFilter.DoesFilterPass(OrderedActor, TargetActor);
	}

	return false;

	// if (TargetActor && UFPGAOrderHelper::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
	// {
	// 	auto AbilitySystem = TargetActor->FindComponentByClass<UAbilitySystemComponent>();
	// 	if (AbilitySystem)
	// 	{
	// 		if (MaxRange < 0 || (TargetActor->GetActorLocation() - OrderedActor->GetActorLocation()).SizeSquared() < MaxRange * MaxRange)
	// 		{
	// 			FGameplayTagContainer SourceTags;
	// 			FGameplayTagContainer TargetTags;
	// 			UFPGAAbilitySystemHelper::GetSourceAndTargetTags(OrderedActor, TargetActor, SourceTags, TargetTags);
	//
	// 			FGameplayTagContainer MissingTags;
	// 			FGameplayTagContainer BlockingTags;
	//
	// 			// if (UFPGAAbilitySystemHelper::DoesSatisfyTagRequirements(
	// 			if (UFPGAAbilitySystemHelper::DoesSatisfyTagRequirementsWithResult(
	// 				TargetTags, TargetRequiredTags, TargetBlockedTags, MissingTags, BlockingTags))
	// 			{
	// 				return true;
	// 			}
	//
	// 			// else
	// 			// {
	// 			//     if (MissingTags.Num() > 0)
	// 			//     {
	// 			//         UE_LOG(LogTemp, Warning, TEXT("Missing tags %s"), *MissingTags.ToString());
	// 			//     }
	// 			//     if (BlockingTags.Num() > 0)
	// 			//     {
	// 			//         UE_LOG(LogTemp, Warning, TEXT("Blocked tags %s"), *BlockingTags.ToString());
	// 			//     }
	// 			// }
	// 		}
	// 	}
	// }
	//
	// return false;
}

bool FFPGATargetingStage::IsTargetTypeFlagChecked(int32 InFlag) const
{
	return UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, InFlag);
}

bool FFPGATargetingStage::IsTargetTypeFlagChecked(EFPGATargetTypeFlags InFlag) const
{
	return UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, InFlag);
}

FGameplayAbilityTargetData* FFPGATargetingStage::MakeTargetDataFromHitResult(AActor* OrderedActor, const FHitResult& HitResult) const
{
	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::NONE | EFPGATargetTypeFlags::PASSIVE))
	{
		return nullptr;
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::ACTOR))
	{
		if (AActor* Actor = HitResult.GetActor())
		{
			if (IsValidActor(OrderedActor, Actor))
			{
				FGameplayAbilityTargetData_ActorArray* Data = new FGameplayAbilityTargetData_ActorArray();
				Data->TargetActorArray.Add(Actor);
				return Data;
			}
		}
	}

	if (UFPGAHelperLibrary::IsTargetTypeFlagChecked(TargetTypeFlags, EFPGATargetTypeFlags::LOCATION | EFPGATargetTypeFlags::DIRECTION))
	{
		if (HitResult.IsValidBlockingHit())
		{
			return FFPGATargetData_Vector::MakeVectorTargetData(HitResult.Location);
		}
	}

	return nullptr;
}

FVector FFPGATargetingStage::GetSourceLocation(AActor* OrderedActor) const
{
	switch (TargetRangeSource)
	{
		case EFPGATargetRangeSource::UNIT:
		{
			return OrderedActor->GetActorLocation();
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
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}
