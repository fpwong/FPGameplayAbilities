// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile/FPGAProjectile.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

AFPGAProjectile* AFPGAProjectile::SpawnProjectile(
	const UObject* WorldContextObject,
	TSubclassOf<AFPGAProjectile> ProjectileClass,
	AActor* InOwner,
	const FTransform& Transform,
	const FGameplayAbilityTargetDataHandle& InTargetData,
	const FFPGAProjectileEffectData& InProjectileEffectData)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World || !ProjectileClass)
	{
		return nullptr;
	}

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = InOwner;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ActorSpawnParameters.bDeferConstruction = true;

	if (AFPGAProjectile* Projectile = World->SpawnActor<AFPGAProjectile>(ProjectileClass, ActorSpawnParameters))
	{
		Projectile->InitProjectile(InTargetData, InProjectileEffectData);
		Projectile->FinishSpawning(Transform);
		// DrawDebugLine(World, Transform.GetLocation(), Transform.GetLocation() + Transform.Rotator().Vector() * 100.0f, FColor::Red, true);
		return Projectile;
	}

	return nullptr;
}

// Sets default values
AFPGAProjectile::AFPGAProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bRotationRemainsVertical = false;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;

	ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
	ProjectileMovementComponent->Friction = 0.0f;

	ProjectileMovementComponent->MaxSpeed = 500.0f;
	ProjectileMovementComponent->InitialSpeed = 500.0f;
	ProjectileMovementComponent->HomingAccelerationMagnitude = 250.0f;

	ProjectileMovementComponent->bInterpMovement = true;
	ProjectileMovementComponent->bInterpRotation = true;

	ProjectileMovementComponent->bIsHomingProjectile = true;

	ProjectileMovementComponent->Bounciness = 1.0f;

	NetUpdateFrequency = 30.0f;

	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	SetRootComponent(DefaultSceneRoot);

	VisualRootComponent = CreateDefaultSubobject<USceneComponent>("VisualRootComponent");
	VisualRootComponent->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicatingMovement(true);
}

// Called when the game starts or when spawned
void AFPGAProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		ProjectileMovementComponent->bRotationFollowsVelocity = false;
		ProjectileMovementComponent->bSimulationEnabled = false;
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		// set initial velocity
		FVector TargetLocation;
		if (UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(TargetData, 0, TargetLocation))
		{
			FVector ToTarget = TargetLocation - GetActorLocation();
			ToTarget.Z = 0;
			ProjectileMovementComponent->Velocity = ToTarget.GetSafeNormal2D() * ProjectileMovementComponent->InitialSpeed;
			// ProjectileMovementComponent->Velocity = ToTarget.GetSafeNormal2D() * ProjectileMovementComponent->InitialSpeed;
		}
	}

	ProjectileMovementComponent->SetInterpolatedComponent(VisualRootComponent);

	OnActorHit.AddUniqueDynamic(this, &AFPGAProjectile::HandleOnActorHit);
	OnActorBeginOverlap.AddUniqueDynamic(this, &AFPGAProjectile::HandleOnBeginOverlap);
}

void AFPGAProjectile::InitProjectile(const FGameplayAbilityTargetDataHandle& InTargetData, const FFPGAProjectileEffectData& InProjectileEffectData)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	TargetData = InTargetData;
	ProjectileEffectData = InProjectileEffectData;

	if (ProjectileMovementComponent->bIsHomingProjectile)
	{
		TargetSceneComponent = NewObject<USceneComponent>(this);
		TargetSceneComponent->RegisterComponent();

		ProjectileMovementComponent->HomingTargetComponent = TargetSceneComponent;

		if (AActor* TargetActor = UFPGAGameplayAbilitiesLibrary::GetFirstActorFromTargetData(TargetData))
		{
			TargetActor->OnDestroyed.AddUniqueDynamic(this, &AFPGAProjectile::OnTargetDestroyed);
			TargetSceneComponent->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else
		{
			FVector TargetLocation;
			if (UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(TargetData, 0, TargetLocation))
			{
				TargetSceneComponent->SetWorldLocation(TargetLocation);
			}
		}
	}

	// TODO init projectile stats, poe style
	int NumBounces = 4;
	if (NumBounces)
	{
		ProjectileMovementComponent->bShouldBounce = true;
	}
}

void AFPGAProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	// UE_LOG(LogTemp, Warning, TEXT("Overlap actor?"));
}

void AFPGAProjectile::Destroyed()
{
	Super::Destroyed();

	if (AActor* TargetActor = UFPGAGameplayAbilitiesLibrary::GetFirstActorFromTargetData(TargetData))
	{
		TargetActor->OnDestroyed.RemoveAll(this);
	}
}

void AFPGAProjectile::PostNetReceiveLocationAndRotation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		const FRepMovement& LocalRepMovement = GetReplicatedMovement();
		const FVector NewLocation = FRepMovement::RebaseOntoLocalOrigin(LocalRepMovement.Location, this);
		ProjectileMovementComponent->MoveInterpolationTarget(NewLocation, LocalRepMovement.Rotation);

		// DrawDebugSphere(GetWorld(), NewLocation, 8, 4, FColor::Green);
	}
}

// Called every frame
void AFPGAProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	FVector TargetLocation;
	if (!UFPGAGameplayAbilitiesLibrary::GetLocationFromTargetData(TargetData, 0, TargetLocation))
	{
		return;
	}

	if (TargetSceneComponent.IsValid())
	{
		const FVector ToTarget = TargetSceneComponent->GetComponentLocation() - GetActorLocation();
		if (ToTarget.SizeSquared2D() < 25 * 25)
		{
			ApplyEffect(UFPGAGameplayAbilitiesLibrary::GetFirstActorFromTargetData(TargetData));
			Destroy();
		}
	}

	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Updating velocity"));
	// 	ProjectileMovementComponent->Velocity = ToTarget.GetSafeNormal2D() * 1000.0f;
	// }
}

bool AFPGAProjectile::ApplyEffect(AActor* TargetActor)
{
	if (!TargetActor || !Owner)
	{
		return false;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = Owner->FindComponentByClass<UAbilitySystemComponent>())
	{
		ExecuteOnHitGameplayCue();

		// apply the gameplay effect to the target
		if (Owner->GetLocalRole() == ROLE_Authority)
		{
			if (UAbilitySystemComponent* TargetASC = TargetActor->FindComponentByClass<UAbilitySystemComponent>())
			{
				FGameplayEffectSpec Spec = UFPGAGameplayAbilitiesLibrary::GetEffectSpecFromHandle(ProjectileEffectData.OnHitGameplayEffect);
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(Spec, TargetASC);
				return true;
			}
		}
	}

	return false;
}

void AFPGAProjectile::OnTargetDestroyed(AActor* Actor)
{
	TargetSceneComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AFPGAProjectile::HandleOnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IsPendingKillPending())
	{
		return;
	}

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Porject overlap %s"), *OtherActor->GetName());

	if (ApplyEffect(OtherActor))
	{
		Destroy();
	}
	else if (!ProjectileMovementComponent->bIsHomingProjectile)
	{
		ExecuteOnHitGameplayCue();
		Destroy();
	}
}

void AFPGAProjectile::HandleOnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsPendingKillPending())
	{
		return;
	}

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (!ProjectileMovementComponent->bIsHomingProjectile)
	{
		ExecuteOnHitGameplayCue();
		Destroy();
	}
}

void AFPGAProjectile::ExecuteOnHitGameplayCue()
{
	if (UAbilitySystemComponent* AbilitySystemComponent = Owner->FindComponentByClass<UAbilitySystemComponent>())
	{
		if (OnHitGameplayCueTag.IsValid())
		{
			FGameplayCueParameters Parameters;
			Parameters.Location = GetActorLocation();
			AbilitySystemComponent->ExecuteGameplayCue(OnHitGameplayCueTag, Parameters);
		}
	}
}
