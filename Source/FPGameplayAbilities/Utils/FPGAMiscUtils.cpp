#include "FPGAMiscUtils.h"

#include "GameplayTagContainer.h"

FGameplayTagContainer UFPGAMiscUtils::GetMissingTags(const FGameplayTagContainer& ContainerA, const FGameplayTagContainer& ContainerB)
{
	FGameplayTagContainer Difference;

	for (auto ItA = ContainerA.CreateConstIterator(); ItA; ++ItA)
	{
		if (!ContainerB.HasTag(*ItA))
		{
			Difference.AddTag(*ItA);
		}
	}
	return Difference;
}
