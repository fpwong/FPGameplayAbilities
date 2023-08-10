// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAAttributeSetInitter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "DataRegistrySubsystem.h"

void FFPGAAttributeSetInitter::PreloadAttributeSetData(const TArray<UCurveTable*>& CurveData)
{
	if (!ensure(CurveData.Num() > 0))
	{
		return;
	}

	/**
	 *	Get list of AttributeSet classes loaded
	 */

	TArray<TSubclassOf<UAttributeSet> >	ClassList;
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* TestClass = *ClassIt;
		if (TestClass->IsChildOf(UAttributeSet::StaticClass()))
		{
			ClassList.Add(TestClass);
		}
	}

	/**
	 *	Loop through CurveData table and build sets of Defaults that keyed off of Name + Level
	 */
	for (const UCurveTable* CurTable : CurveData)
	{
		for (const TPair<FName, FRealCurve*>& CurveRow : CurTable->GetRowMap())
		{
			FString RowName = CurveRow.Key.ToString();
			FString ClassName;
			FString SetName;
			FString AttributeName;
			FString Temp;

			RowName.Split(TEXT("."), &ClassName, &Temp);
			Temp.Split(TEXT("."), &SetName, &AttributeName);

			if (!ensure(!ClassName.IsEmpty() && !SetName.IsEmpty() && !AttributeName.IsEmpty()))
			{
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to parse row %s in %s"), *RowName, *CurTable->GetName());
				continue;
			}

			// Find the AttributeSet

			TSubclassOf<UAttributeSet> Set = FindBestAttributeClass(ClassList, SetName);
			if (!Set)
			{
				// This is ok, we may have rows in here that don't correspond directly to attributes
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match AttributeSet from %s (row: %s)"), *SetName, *RowName);
				continue;
			}

			// Find the FProperty
			FProperty* Property = FindFProperty<FProperty>(*Set, *AttributeName);
			if (!IsSupportedProperty(Property))
			{
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match Attribute from %s (row: %s)"), *AttributeName, *RowName);
				continue;
			}

			FRealCurve* Curve = CurveRow.Value;
			FName ClassFName = FName(*ClassName);
			FAttributeSetDefaultsCollection& DefaultCollection = Defaults.FindOrAdd(ClassFName);

			float FirstLevelFloat = 0.f;
			float LastLevelFloat = 0.f;
			Curve->GetTimeRange(FirstLevelFloat, LastLevelFloat);

			int32 FirstLevel = FMath::RoundToInt32(FirstLevelFloat);
			int32 LastLevel = FMath::RoundToInt32(LastLevelFloat);

			// Only log these as warnings, as they're not deal breakers.
			if (FirstLevel != 1)
			{
				ABILITY_LOG(Warning, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData First level should be 1"));
				continue;
			}

			DefaultCollection.LevelData.SetNum(FMath::Max(LastLevel, DefaultCollection.LevelData.Num()));

			for (int32 Level = 1; Level <= LastLevel; ++Level)
			{
				float Value = Curve->Eval(static_cast<float>(Level));

				FAttributeSetDefaults& SetDefaults = DefaultCollection.LevelData[Level-1];

				FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(Set);
				if (DefaultDataList == nullptr)
				{
					ABILITY_LOG(Verbose, TEXT("Initializing new default set for %s[%d]. PropertySize: %d.. DefaultSize: %d"), *Set->GetName(), Level, Set->GetPropertiesSize(), UAttributeSet::StaticClass()->GetPropertiesSize());

					DefaultDataList = &SetDefaults.DataMap.Add(Set);
				}

				// Import curve value into default data

				check(DefaultDataList);
				DefaultDataList->AddPair(Property, Value);
			}
		}
	}
}
void FFPGAAttributeSetInitter::PreloadAttributeSetData2(const TArray<UCurveTable*>& CurveData)
{
	if (!ensure(CurveData.Num() > 0))
	{
		return;
	}

	/**
	 *	Get list of AttributeSet classes loaded
	 */

	TArray<TSubclassOf<UAttributeSet>> ClassList;
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* TestClass = *ClassIt;
		if (TestClass->IsChildOf(UAttributeSet::StaticClass()))
		{
			ClassList.Add(TestClass);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AttributeSetInitter: NUm attribute set classes %d"), ClassList.Num());
	for (TSubclassOf<UAttributeSet> Class : ClassList)
	{
		UE_LOG(LogTemp, Warning, TEXT("\t%s"), *Class->GetName());
	}

	// UE_LOG(LogTemp, Warning, TEXT("Raw Data"));
	int NumColumns = 0;
	TMap<FString, float> CompressedRows; 
	for (const UCurveTable* CurTable : CurveData)
	{
		for (const TPair<FName, FRealCurve*>& CurveRow : CurTable->GetRowMap())
		{
			FString RowName = CurveRow.Key.ToString();
			FRealCurve* Curve = CurveRow.Value;
			for (auto KeyIter = Curve->GetKeyHandleIterator(); KeyIter; ++KeyIter)
			{
				const FKeyHandle& KeyHandle = *KeyIter;
				TPair<float, float> LevelValuePair = Curve->GetKeyTimeValuePair(KeyHandle);
				int32 Level = LevelValuePair.Key;
				NumColumns = FMath::Max(NumColumns, Level);
				// UE_LOG(LogTemp, Warning, TEXT("%s (%d): %f"), *RowName, Level, LevelValuePair.Value);
			}
		}

		for (const TPair<FName, FRealCurve*>& CurveRow : CurTable->GetRowMap())
		{
			FString RowName = CurveRow.Key.ToString();
			FRealCurve* Curve = CurveRow.Value;
			if (Curve->GetNumKeys() == 2)
			{
				float FirstValue = Curve->GetKeyValue(Curve->GetFirstKeyHandle());
				float LastKeyValue = Curve->GetKeyValue(Curve->GetLastKeyHandle());
				float Diff = (LastKeyValue - FirstValue);
				float Step = Diff == 0 ? 0 : (LastKeyValue - FirstValue) / static_cast<float>(NumColumns - 1);
				CompressedRows.Add(RowName, Step);
				// UE_LOG(LogTemp, Warning, TEXT("Found compressed row %s: %f (%f, %f)"), *RowName, Step, Diff, Step);
			}
			// for (auto KeyIter = Curve->GetKeyHandleIterator(); KeyIter; ++KeyIter)
			// {
			// 	const FKeyHandle& KeyHandle = *KeyIter;
			// 	TPair<float, float> LevelValuePair = Curve->GetKeyTimeValuePair(KeyHandle);
			// 	int32 Level = LevelValuePair.Key;
			// 	NumColumns = FMath::Max(NumColumns, Level);
			// 	UE_LOG(LogTemp, Warning, TEXT("%s (%d): %f"), *RowName, Level, LevelValuePair.Value);
			// }
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Filtered Data"));
	/**
	 *	Loop through CurveData table and build sets of Defaults that keyed off of Name + Level
	 */
	for (const UCurveTable* CurTable : CurveData)
	{
		for (const TPair<FName, FRealCurve*>& CurveRow : CurTable->GetRowMap())
		{
			FString RowName = CurveRow.Key.ToString();
			FString ClassName;
			FString SetName;
			FString AttributeName;
			FString Temp;

			RowName.Split(TEXT("."), &ClassName, &Temp);
			Temp.Split(TEXT("."), &SetName, &AttributeName);

			if (!ensure(!ClassName.IsEmpty() && !SetName.IsEmpty() && !AttributeName.IsEmpty()))
			{
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to parse row %s in %s"), *RowName, *CurTable->GetName());
				continue;
			}

			// Find the AttributeSet

			TSubclassOf<UAttributeSet> Set = FindBestAttributeClass(ClassList, SetName);
			if (!Set)
			{
				// This is ok, we may have rows in here that don't correspond directly to attributes
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match AttributeSet from %s (row: %s)"), *SetName, *RowName);
				continue;
			}

			// Find the FProperty
			FProperty* Property = FindFProperty<FProperty>(*Set, *AttributeName);
			if (!IsSupportedProperty(Property))
			{
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match Attribute from %s (row: %s)"), *AttributeName, *RowName);
				continue;
			}

			FRealCurve* Curve = CurveRow.Value;
			FName ClassFName = FName(*ClassName);
			FAttributeSetDefaultsCollection& DefaultCollection = Defaults.FindOrAdd(ClassFName);

			// Check our curve to make sure the keys match the expected format
			int32 ExpectedLevel = 1;
			bool bShouldSkip = false;
			// for (auto KeyIter = Curve->GetKeyHandleIterator(); KeyIter; ++KeyIter)
			// {
			// 	const FKeyHandle& KeyHandle = *KeyIter;
			// 	if (KeyHandle == FKeyHandle::Invalid())
			// 	{
			// 		ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Data contains an invalid key handle (row: %s)"), *RowName);
			// 		bShouldSkip = true;
			// 		break;
			// 	}
			//
			// 	int32 Level = Curve->GetKeyTimeValuePair(KeyHandle).Key;
			// 	if (ExpectedLevel != Level)
			// 	{
			// 		ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Keys are expected to start at 1 and increase by 1 for every key (row: %s level: %d expected: %d)"), *RowName, Level, ExpectedLevel);
			// 		bShouldSkip = true;
			// 		break;
			// 	}
			//
			// 	++ExpectedLevel;
			// }

			if (bShouldSkip)
			{
				continue;
			}

			// int MaxLevel = -1;
			// for (auto KeyIter = Curve->GetKeyHandleIterator(); KeyIter; ++KeyIter)
			// {
			// 	const FKeyHandle& KeyHandle = *KeyIter;
			//
			// 	TPair<float, float> LevelValuePair = Curve->GetKeyTimeValuePair(KeyHandle);
			// 	int32 Level = LevelValuePair.Key;
			// 	MaxLevel = FMath::Max(Level, MaxLevel);
			//
			// 	UE_LOG(LogTemp, Warning, TEXT("%s (%d): %f"), *RowName, Level, LevelValuePair.Value);
			// }

			int32 LastLevel = Curve->GetKeyTime(Curve->GetLastKeyHandle());
			// DefaultCollection.LevelData.SetNum(MaxLevel);
			DefaultCollection.LevelData.SetNum(NumColumns);

			float PreviousValidValue = -1.0f;
			for (int Level = 1; Level <= NumColumns; ++Level)
			{
				int Value = 0.0f;
				
				if (Curve->KeyExistsAtTime(Level))
				{
					const FKeyHandle& KeyHandle = Curve->FindKey(Level);
					TPair<float, float> LevelValuePair = Curve->GetKeyTimeValuePair(KeyHandle);
					Value = LevelValuePair.Value;
					PreviousValidValue = Value;
				}
				else
				{
					Value = PreviousValidValue;
					if (auto Step = CompressedRows.Find(RowName))
					{
						Value += (*Step) * (Level - 1);
					}
				}

				FAttributeSetDefaults& SetDefaults = DefaultCollection.LevelData[Level - 1];
				FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(Set);
				if (DefaultDataList == nullptr)
				{
					ABILITY_LOG(Verbose, TEXT("Initializing new default set for %s[%d]. PropertySize: %d.. DefaultSize: %d"), *Set->GetName(), Level, Set->GetPropertiesSize(), UAttributeSet::StaticClass()->GetPropertiesSize());

					DefaultDataList = &SetDefaults.DataMap.Add(Set);
				}

				// Import curve value into default data

				check(DefaultDataList);
				DefaultDataList->AddPair(Property, Value);
			}

			// //At this point we know the Name of this "class"/"group", the AttributeSet, and the Property Name. Now loop through the values on the curve to get the attribute default value at each level.
			// for (auto KeyIter = Curve->GetKeyHandleIterator(); KeyIter; ++KeyIter)
			// {
			// 	const FKeyHandle& KeyHandle = *KeyIter;
			//
			// 	TPair<float, float> LevelValuePair = Curve->GetKeyTimeValuePair(KeyHandle);
			// 	int32 Level = LevelValuePair.Key;
			// 	float Value = LevelValuePair.Value;
			//
			// 	FAttributeSetDefaults& SetDefaults = DefaultCollection.LevelData[Level - 1];
			//
			// 	FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(Set);
			// 	if (DefaultDataList == nullptr)
			// 	{
			// 		ABILITY_LOG(Verbose, TEXT("Initializing new default set for %s[%d]. PropertySize: %d.. DefaultSize: %d"), *Set->GetName(), Level, Set->GetPropertiesSize(), UAttributeSet::StaticClass()->GetPropertiesSize());
			//
			// 		DefaultDataList = &SetDefaults.DataMap.Add(Set);
			// 	}
			//
			// 	// Import curve value into default data
			//
			// 	check(DefaultDataList);
			// 	DefaultDataList->AddPair(Property, Value);
			// }
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("Init attribute defaults %d"), Defaults.Num());
	// for (auto& Default : Defaults)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("\t%s"), *Default.Key.ToString());
	// 	for (FAttributeSetDefaults& LevelData : Default.Value.LevelData)
	// 	{
	// 		for (auto& DataMap : LevelData.DataMap)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("\t\t%s"), *DataMap.Key->GetName());
	// 			for (TTuple<FProperty*, float>& AttributeValue : DataMap.Value.AttributeValues)
	// 			{
	// 				UE_LOG(LogTemp, Warning, TEXT("\t\t\t%s: %f"), *AttributeValue.Key->GetName(), AttributeValue.Value);
	// 			}
	// 		}
	// 	}
	// }
}

void FFPGAAttributeSetInitter::InitAttributeSetDefaults(UAbilitySystemComponent* AbilitySystemComponent, FName GroupName, int32 Level, bool bInitialInit) const
{
	check(AbilitySystemComponent != nullptr);

	FName DefaultAndGroupNames[2] = { FName(TEXT("Default")), GroupName };

	for (const FName& Name : DefaultAndGroupNames)
	{
		const FAttributeSetDefaultsCollection* Collection = Defaults.Find(Name);
		if (!Collection)
		{
			// ABILITY_LOG(Warning, TEXT("Unable to find DefaultAttributeSet Group %s. Falling back to Defaults"), *GroupName.ToString());
			ABILITY_LOG(Warning, TEXT("Unable to find DefaultAttributeSet Group %s skipping"), *GroupName.ToString());
			continue;
			// Collection = Defaults.Find(FName(TEXT("Default")));
			// if (!Collection)
			// {
			// 	ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
			// 	return;
			// }
		}

		if (!Collection->LevelData.IsValidIndex(Level - 1))
		{
			// We could eventually extrapolate values outside of the max defined levels
			ABILITY_LOG(Warning, TEXT("Attribute defaults for Level %d are not defined! Skipping"), Level);
			return;
		}
	
		const FAttributeSetDefaults& SetDefaults = Collection->LevelData[Level - 1];

		const auto SetAttributeValues = [&SetDefaults, &bInitialInit, &AbilitySystemComponent](const UAttributeSet* Set, TSubclassOf<UAttributeSet> Class)
		{
			const FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(Class);
			if (DefaultDataList)
			{
				ABILITY_LOG(Log, TEXT("Initializing Set %s (%s)"), *Set->GetName(), *Class->GetName());

				for (auto& DataPair : DefaultDataList->AttributeValues)
				{
					check(DataPair.Key);
					if (Set->ShouldInitProperty(bInitialInit, DataPair.Key))
					{
						FGameplayAttribute AttributeToModify(DataPair.Key);
						AbilitySystemComponent->SetNumericAttributeBase(AttributeToModify, DataPair.Value);
					}
				}
			}
		};

		for (const UAttributeSet* Set : AbilitySystemComponent->GetSpawnedAttributes())
		{
			if (!Set)
			{
				continue;
			}

			TSubclassOf<UAttributeSet> NextClass(Set->GetClass());
			while (NextClass && NextClass->IsChildOf(UAttributeSet::StaticClass()))
			{
				SetAttributeValues(Set, NextClass);

				if (UClass* ParentClass = NextClass->GetSuperClass())
				{
					NextClass = TSubclassOf<UAttributeSet>(ParentClass);
				}
				else
				{
					NextClass = nullptr;
				}
			}
		}
	}

	AbilitySystemComponent->ForceReplication();
}

void FFPGAAttributeSetInitter::ApplyAttributeDefault(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, FName GroupName, int32 Level) const
{
	const FAttributeSetDefaultsCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Warning, TEXT("Unable to find DefaultAttributeSet Group %s. Falling back to Defaults"), *GroupName.ToString());
		Collection = Defaults.Find(FName(TEXT("Default")));
		if (!Collection)
		{
			ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
			return;
		}
	}

	if (!Collection->LevelData.IsValidIndex(Level - 1))
	{
		// We could eventually extrapolate values outside of the max defined levels
		ABILITY_LOG(Warning, TEXT("Attribute defaults for Level %d are not defined! Skipping"), Level);
		return;
	}

	const FAttributeSetDefaults& SetDefaults = Collection->LevelData[Level - 1];
	for (const UAttributeSet* Set : AbilitySystemComponent->GetSpawnedAttributes())
	{
		if (!Set)
		{
			continue;
		}

		const FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(Set->GetClass());
		if (DefaultDataList)
		{
			ABILITY_LOG(Log, TEXT("Initializing Set %s"), *Set->GetName());

			for (auto& DataPair : DefaultDataList->AttributeValues)
			{
				check(DataPair.Key);
				if (DataPair.Key == InAttribute.GetUProperty())
				{
					FGameplayAttribute AttributeToModify(DataPair.Key);
					AbilitySystemComponent->SetNumericAttributeBase(AttributeToModify, DataPair.Value);
				}
			}
		}
	}

	AbilitySystemComponent->ForceReplication();
}

TArray<float> FFPGAAttributeSetInitter::GetAttributeSetValues(UClass* AttributeSetClass, FProperty* AttributeProperty, FName GroupName) const
{
	TArray<float> AttributeSetValues;
	const FAttributeSetDefaultsCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Error, TEXT("FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
		return TArray<float>();
	}

	for (const FAttributeSetDefaults& SetDefaults : Collection->LevelData)
	{
		const FAttributeDefaultValueList* DefaultDataList = SetDefaults.DataMap.Find(AttributeSetClass);
		if (DefaultDataList)
		{
			for (auto& DataPair : DefaultDataList->AttributeValues)
			{
				check(DataPair.Key);
				if (DataPair.Key == AttributeProperty)
				{
					AttributeSetValues.Add(DataPair.Value);
				}
			}
		}
	}

	return AttributeSetValues;
}

void FFPGAAttributeSetInitter::InitAttributeSetValues(UAttributeSet* Set, const FName& GroupName, int32 Level, bool bInitialInit)
{
	if (!Set)
	{
		return;
	}

	const FAttributeDefaultValueList* DefaultDataList = GetDefaultValueList(Set, "Default", Level, bInitialInit);
	const FAttributeDefaultValueList* GroupDataList = GetDefaultValueList(Set, GroupName, Level, bInitialInit);
	TSet<FProperty*> AppliedProperties;

	for (const FAttributeDefaultValueList* DataList : { GroupDataList, DefaultDataList })
	{
		if (!DataList)
		{
			continue;
		}

		for (auto& DataPair : DataList->AttributeValues)
		{
			check(DataPair.Key);
			if (DataPair.Key)
			{
				if (AppliedProperties.Contains(DataPair.Key))
				{
					continue;
				}

				AppliedProperties.Add(DataPair.Key);

				FGameplayAttribute AttributeToModify(DataPair.Key);
				if (FGameplayAttributeData* Data = AttributeToModify.GetGameplayAttributeData(Set))
				{
					Data->SetBaseValue(DataPair.Value);
					Data->SetCurrentValue(DataPair.Value);
				}
			}
		}
	}
}

const FAttributeDefaultValueList* FFPGAAttributeSetInitter::GetDefaultValueList(UAttributeSet* Set, const FName& GroupName, int32 Level, bool bInitialInit)
{
	if (!Set)
	{
		return nullptr;
	}

	const FAttributeSetDefaultsCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Warning, TEXT("Failed to find group collection!"));
		return nullptr;
	}

	if (!Collection->LevelData.IsValidIndex(Level - 1))
	{
		// We could eventually extrapolate values outside of the max defined levels
		ABILITY_LOG(Warning, TEXT("Attribute defaults for Level %d are not defined! Skipping"), Level);
		return nullptr;
	}

	const FAttributeSetDefaults& SetDefaults = Collection->LevelData[Level - 1];

	return SetDefaults.DataMap.Find(Set->GetClass());
}

bool FFPGAAttributeSetInitter::IsSupportedProperty(FProperty* Property) const
{
	return (Property && (CastField<FNumericProperty>(Property) || FGameplayAttribute::IsGameplayAttributeDataProperty(Property)));
}

TSubclassOf<UAttributeSet> FFPGAAttributeSetInitter::FindBestAttributeClass(TArray<TSubclassOf<UAttributeSet>>& ClassList, FString PartialName)
{
	for (TSubclassOf<UAttributeSet> Class : ClassList)
	{
		if (Class->GetName().Equals(PartialName))
		{
			return Class;
		}
	}

	return nullptr;
}