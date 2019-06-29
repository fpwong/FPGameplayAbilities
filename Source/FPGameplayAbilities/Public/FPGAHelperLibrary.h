// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Perception/AISense.h"
#include "FPGAHelperLibrary.generated.h"

//USTRUCT(BlueprintType)
//struct FSteamFriends
//{
//	GENERATED_USTRUCT_BODY()
//
//public:
//	UPROPERTY(BlueprintReadOnly, Category = "Steam | Utils")
//	FString DisplayName;
//
//	UPROPERTY(BlueprintReadOnly, Category = "Steam | Utils")
//	UTexture2D* PlayerAvatar;
//};

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*UFUNCTION(BlueprintCallable, Category = "Steam")
	static void GetSteamFriends(TArray<FSteamFriends>& OutFriends, int32& OutFriendCount);*/
	
	UFUNCTION(BlueprintCallable, Category = "AIPerception")
	static bool CheckAffiliation(FAISenseAffiliationFilter Affiliation, AActor* ActorA, AActor* ActorB);
};
