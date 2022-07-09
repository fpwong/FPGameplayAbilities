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
