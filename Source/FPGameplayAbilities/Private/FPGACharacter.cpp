// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGACharacter.h"
#include "AbilitySystem/FPGAAbilitySystemComponent.h"
#include "AbilitySystem/FPGAAttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "FPGACharacterMovement.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "FPGAGameState.h"

FVector AFPGACharacter::GetMovementLocation()
{
	return TargetActor != nullptr ? TargetActor->GetActorLocation() : TargetLocation;
}

float AFPGACharacter::GetMovementDistanceToTarget()
{
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	return (TargetCharacter != nullptr) ? TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() + MovementDistance : MovementDistance;
}

// Sets default values
AFPGACharacter::AFPGACharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFPGACharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystem = CreateDefaultSubobject<UFPGAAbilitySystemComponent>(TEXT("AbilitySystem"));
	check(AbilitySystem != nullptr);

	AttributeSetClass = UFPGAAttributeSet::StaticClass();

	bUseControllerRotationYaw = false;

	MovementDistance = DefaultMovementDistance = 50;

	AbilitySystem->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &AFPGACharacter::OnGameplayEffectRemoved);

	AbilitySystem->AbilityCommittedCallbacks.AddUObject(this, &AFPGACharacter::OnAbilityCommitted);

	// use this delegate for any duration based effects
	//AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &AFPGACharacter::OnGEAppliedToSelf);

	//AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCRPGAttributeSet::GetTurnSpeedAttribute()).AddUObject(this, &AFPGACharacter::OnTurnSpeedChanged);
	
	AbilitySystem->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &AFPGACharacter::OnGEAppliedToSelf);

	TargetActor = nullptr;
	TargetLocation = FVector::ZeroVector;

	TeamId = 0;
}

// Called when the game starts or when spawned
void AFPGACharacter::BeginPlay()
{
	Super::BeginPlay();

	TargetLocation = GetActorLocation();

	check(AbilitySystem != nullptr);
	
	// Initialize actor info
	AbilitySystem->InitAbilityActorInfo(this, this);

	// Initialize the attribute set
	if (AttributeSetClass != nullptr)
	{
		AttributeSet = AbilitySystem->InitStats(AttributeSetClass, AttributeSetDataTable);

		if (HasAuthority())
		{
			for (TSubclassOf<UGameplayEffect> Effect : InitialEffects)
			{
				AbilitySystem->BP_ApplyGameplayEffectToSelf(Effect, 0.f, FGameplayEffectContextHandle());
			}

			for (int i = 0; i < InitialAbilities.Num(); i++)
			{
				TSubclassOf<UGameplayAbility> Ability = InitialAbilities[i];
				FGameplayAbilitySpecHandle AbilityHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, i));
				if (!AbilityHandle.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to give ability %s"), *Ability->GetName());
				}
			}
		}
	}
}

void AFPGACharacter::Restart()
{
	Super::Restart();

	check(AbilitySystem != nullptr);

	AbilitySystem->RefreshAbilityActorInfo();
}

UAbilitySystemComponent* AFPGACharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

FGenericTeamId AFPGACharacter::GetGenericTeamId() const
{
	return FGenericTeamId(TeamId);
}

void AFPGACharacter::OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveEffect)
{
	//FGameplayEffectSpec SpecApplied = ActiveEffect.Spec;

	//FGameplayTagContainer GrantedTags;
	//SpecApplied.GetAllGrantedTags(GrantedTags);
	//bool bIsStatusEffect = GrantedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Combat.Status"), true));

	//if (bIsStatusEffect && HasAuthority())
	//{
	//	if (AFPGAGameState* GameState = Cast<AFPGAGameState>(UGameplayStatics::GetGameState(this)))
	//	{
	//		FString LogOutput = FString::Printf(
	//			TEXT("%s) Effect removed %s by %s"),
	//			*GetName(),
	//			*SpecApplied.ToSimpleString(), // effect name
	//			*SpecApplied.GetContext().ToString() // causer
	//		);

	//		GameState->UpdateCombatLog(LogOutput);
	//	}
	//}
}

void AFPGACharacter::OnAbilityCommitted(UGameplayAbility* Ability)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability committed %s"), *Ability->GetName());
}

void AFPGACharacter::TestPrintGE()
{
	AbilitySystem->PrintAllGameplayEffects();
}

void AFPGACharacter::OnGEAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	//FGameplayTagContainer GrantedTags;
	//SpecApplied.GetAllGrantedTags(GrantedTags);
	//bool bIsStatusEffect = GrantedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Combat.Status"), true));

	//FString LogOutput = FString::Printf(
	//			TEXT("%s) Effect applied %s by %s "),
	//			*GetName(),
	//			*SpecApplied.ToSimpleString(), // effect name,
	//			*SpecApplied.GetContext().ToString() // causer
	//		);

	//UKismetSystemLibrary::PrintString(this, LogOutput, false);

	////UE_LOG(LogTemp, Warning, TEXT("Testing GE %s"), *LogOutput);

	//if (bIsStatusEffect && HasAuthority())
	//{
	//	if (AFPGAGameState* GameState = Cast<AFPGAGameState>(UGameplayStatics::GetGameState(this)))
	//	{
	//		//FString LogOutput = FString::Printf(
	//		//	TEXT("%s) Effect applied %s by %s "),
	//		//	*GetName(),
	//		//	*SpecApplied.ToSimpleString(), // effect name,
	//		//	*SpecApplied.GetContext().ToString() // causer
	//		//);

	//		GameState->UpdateCombatLog(LogOutput);
	//	}
	//}
}

//void AFPGACharacter::OnTurnSpeedChanged(const FOnAttributeChangeData& ChangeData)
//{
//	GetCharacterMovement()->RotationRate.Yaw = ChangeData.NewValue * 360;
//}

void AFPGACharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME_CONDITION_NOTIFY(AFPGACharacter, TeamId, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME(AFPGACharacter, TeamId);
//	DOREPLIFETIME(AFPGACharacter, TargetLocation);
}
