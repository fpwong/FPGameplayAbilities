#include "FPGATargetTypes.h"

FFPGATargetData_Vector* FFPGATargetData_Vector::MakeVectorTargetData(const FVector& Vector)
{
	FFPGATargetData_Vector* NewData = new FFPGATargetData_Vector();
	NewData->TargetLocation = Vector;
	return NewData;
}

bool FFPGATargetData_Vector::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	TargetLocation.NetSerialize(Ar, Map, bOutSuccess);
	bOutSuccess = true;
	return true;
}

FFPGATargetData_SingleActor* FFPGATargetData_SingleActor::MakeSingleActorTargetData(AActor* InActor)
{
	FFPGATargetData_SingleActor* NewData = new FFPGATargetData_SingleActor();
	NewData->ActorHandle = InActor;
	return NewData;
}

TArray<TWeakObjectPtr<AActor>> FFPGATargetData_SingleActor::GetActors() const
{
	TArray<TWeakObjectPtr<AActor>> Actors;
	if (ActorHandle.IsValid())
	{
		Actors.Add(TWeakObjectPtr<AActor>(ActorHandle.FetchActor()));
	}

	return Actors;
}

bool FFPGATargetData_SingleActor::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << ActorHandle;
	bOutSuccess = true;
	return true;
}
