#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFPGATargetTypeFlags : uint8
{
    NO_FLAGS = 0 UMETA(Hidden),
	
    /** No specific target (AOE, Aura or automatically applied to self). */
    NONE = 1 << 0,

    /** Order needs an actor as target. */
    ACTOR = 1 << 1,

    /** Order needs vector location as target. */
    LOCATION = 1 << 2,

    /** Order needs vector location that is used together with the unit location as a direction. */
    DIRECTION = 1 << 3,

    /** Can't be activated. */
    PASSIVE = 1 << 4
};
ENUM_CLASS_FLAGS(EFPGATargetTypeFlags)
