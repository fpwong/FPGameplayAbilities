//// Fill out your copyright notice in the Description page of Project Settings.
//
#include "FPGAEditorUtilities.h"
//#include "AssetToolsModule.h"
//#include "GameplayAbilitiesEditor/Public/GameplayAbilitiesBlueprintFactory.h"
//#include "AbilitySystem/FPGAGameplayAbility.h"
//#include "GameplayAbilityBlueprint.h"
//#include "Engine/ObjectLibrary.h"
//#include "Engine/AssetManager.h"
//#include "Engine/BlueprintGeneratedClass.h"
//
//void UFPGAEditorUtilities::CreateAbility()
//{
//	// Create an appropriate and unique name 
//
//	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
//	//AssetToolsModule.Get().CreateUniqueAssetName(Parent->GetOutermost()->GetName(), DefaultSuffix, PackageName, Name);
//
//	UGameplayAbilitiesBlueprintFactory* Factory = NewObject<UGameplayAbilitiesBlueprintFactory>();
//	//Factory->ParentClass = ParentClass;
//	//UObject* Child = AssetToolsModule.Get().CreateAssetWithDialog(AbilityName, FPackageName::GetLongPackagePath(Location), Factory->ParentClass, Factory);
//	//UObject* AbilityObj = AssetToolsModule.Get().CreateAssetWithDialog(AssetName, FPackageName::GetLongPackagePath(AbilityPath), Factory->SupportedClass, Factory);
//
//	FString AssetName;
//	FString AbilityPath = "/Game/";
//	UObject* AbilityObj = AssetToolsModule.Get().CreateAssetWithDialog(Factory->SupportedClass, Factory);
//
//	UE_LOG(LogTemp, Warning, TEXT("Created asset 2"));
//
//	//	FindField;
//	//if (AbilityObj)
//	//   {
//	//       float FoundFloat, IncreasedFloat;
//	//       UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);
//	//       {
//	//           if (FloatProp)
//	//           {
//	//               FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
//	//               IncreasedFloat = FoundFloat + ValueToAdd;  //increase the value 
//	//               FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat); //and then set it
//	//               outFloat = IncreasedFloat;
//	//               return true;
//	//           }
//	//       }
//	//   }
//
//	//if (UFPGAGameplayAbility* GameplayAbility = Cast<UFPGAGameplayAbility>(AbilityObj))
//	if (UGameplayAbilityBlueprint* Blueprint = Cast<UGameplayAbilityBlueprint>(AbilityObj))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Try obj library %s | %s | %s"), *Blueprint->GetPathName(), *Blueprint->BlueprintDisplayName, *Blueprint->GetName());
//		//UObjectLibrary* ObjLibrary = UObjectLibrary::CreateLibrary(UGameplayAbilityBlueprint::StaticClass(), true, true);
//		UObjectLibrary* ObjLibrary = UObjectLibrary::CreateLibrary(UGameplayAbility::StaticClass(), true, true);
//
//		//UObjectLibrary* ObjLibrary = UObjectLibrary::CreateLibrary(InterfaceClass, true, true);
//		//ObjLibrary->LoadBlueprintAssetDataFromPath(TEXT("/Game/"));
//		//ObjLibrary->GetAssetDataList(GameplayCueInterfaceActors);
//
//		FString ObjectPath = Blueprint->GetPathName() + "_C";
//
//		UE_LOG(LogTemp, Warning, TEXT("Checking obj path 1 %s"), *ObjectPath);
//
//		UObject* Target = FindObject<UObject>(NULL, *ObjectPath);
//		if (Target == NULL)
//		{
//			Target = LoadObject<UObject>(NULL, *ObjectPath);
//			UE_LOG(LogTemp, Warning, TEXT("My Target?"));
//		}
//
//		if (UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(Target))
//		{
//			UE_LOG(LogTemp, Warning, TEXT("BP gen Class is valid"));
//			for (auto elem : BPClass->PropertyGuids)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("%s"), *elem.Key.ToString());
//			}
//		}
//
//		//Blueprint->GetClass();
//
//		TArray<UObject*> SubObjects;
//		Blueprint->GetDefaultSubobjects(SubObjects);
//
//		UObject* DefaultObject = Target->GetClass()->GetDefaultObject();
//
//		UE_LOG(LogTemp, Warning, TEXT("Default Obj %s"), *DefaultObject->GetName());
//
//		for (UObject* Obj : SubObjects)
//		{
//			for (TFieldIterator<UProperty> PropIt(Obj->GetClass()); PropIt; ++PropIt)
//			{
//				UProperty* Property = *PropIt;
//				// Do something with the property
//				UE_LOG(LogTemp, Warning, TEXT("SubObjs Property %s"), *Property->GetDisplayNameText().ToString());
//			}
//		}
//
//		for (TFieldIterator<UProperty> PropIt(DefaultObject->GetClass()); PropIt; ++PropIt)
//		{
//			UProperty* Property = *PropIt;
//			// Do something with the property
//			UE_LOG(LogTemp, Warning, TEXT("Default Obj Property %s"), *Property->GetDisplayNameText().ToString());
//		}
//
//		if (Target)
//		{
//			//UE_LOG(LogTemp, Warning, TEXT("%s | %s | %s"), *Blueprint->GetFriendlyName(), *Blueprint->GetBlueprintClass()->FindPropertyByName());
//
//			//Target->GetBlueprintClass();
//
//			UE_LOG(LogTemp, Warning, TEXT("Target is not null? 2 %s"), *Target->GetClass()->GetPathName());
//
//			//for (TFieldIterator<UProperty> PropIt(Target->GetClass()); PropIt; ++PropIt)
//			for (TFieldIterator<UProperty> PropIt(Target->GetClass()); PropIt; ++PropIt)
//			{
//				UProperty* Property = *PropIt;
//				// Do something with the property
//				UE_LOG(LogTemp, Warning, TEXT("Property %s"), *Property->GetDisplayNameText().ToString());
//			}
//
//			float FoundFloat, IncreasedFloat;
//			UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), TEXT("Testing"));
//			{
//				if (FloatProp)
//				{
//					UE_LOG(LogTemp, Warning, TEXT("I FOUND THE FLOAT PROPERYT"));
//
//					FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
//					IncreasedFloat = FoundFloat + 10; //increase the value 
//					FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat); //and then set it
//					//outFloat = IncreasedFloat;
//				}
//			}
//		}
//
//		//return;
//
//		if (ObjLibrary)
//		{
//			//ObjLibrary->AddToRoot();
//			//ObjLibrary->LoadBlueprintsFromPath(Blueprint->GetPathName());
//			//ObjLibrary->LoadBlueprintAssetDataFromPath(AbilityPath);
//
//			FString Left;
//			FString Right;
//			if (Blueprint->GetPathName().Split("/", &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
//			{
//				Left = "/Game" + Left + "/";
//				UE_LOG(LogTemp, Warning, TEXT("%s"), *Left);
//
//				//ObjLibrary->LoadBlueprintAssetDataFromPath(Left);
//
//				ObjLibrary->LoadBlueprintAssetDataFromPath("/Game/");
//
//				//UE_LOG(LogTemp, Warning, TEXT("Path %s"), *Left);
//				ObjLibrary->LoadAssetsFromAssetData();
//
//				TArray<FAssetData> AssetDatas;
//				ObjLibrary->GetAssetDataList(AssetDatas);
//				UE_LOG(LogTemp, Warning, TEXT("Found objects: %d"), AssetDatas.Num());
//
//				for (FAssetData& Data : AssetDatas)
//				{
//					UE_LOG(LogTemp, Warning, TEXT("\tAsset Data %s"), *Data.AssetName.ToString());
//				}
//
//				if (AssetDatas.Num() > 0)
//				{
//					AssetDatas[0].ToStringReference().TryLoad();
//					UObject* Target = AssetDatas[0].GetAsset(); //Always returns null
//					UE_LOG(LogTemp, Warning, TEXT("Teinartarts"));
//
//					if (Target)
//					{
//						for (TFieldIterator<UProperty> PropIt(Target->GetClass()); PropIt; ++PropIt)
//						{
//							UProperty* Property = *PropIt;
//							// Do something with the property
//							UE_LOG(LogTemp, Warning, TEXT("ObjLibrary Property %s"), *Property->GetDisplayNameText().ToString());
//						}
//
//						//float FoundFloat; //, IncreasedFloat;
//						//UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), "Testing");
//						//{
//						//	if (FloatProp)
//						//	{
//						//		FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
//						//		//IncreasedFloat = FoundFloat + ValueToAdd; //increase the value 
//						//		//FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat); //and then set it
//						//		//outFloat = IncreasedFloat;
//						//	}
//						//}
//					}
//				}
//			}
//		}
//	}
//	/*if (UGameplayAbility* GameplayAbility = Cast<UGameplayAbility>(AbilityObj))
//	{
//		if (UFPGAGameplayAbility * FPGAGameplayAbility = Cast<UFPGAGameplayAbility>(AbilityObj))
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Set variables!"));
//
//			FPGAGameplayAbility->SetCostEffectClass(UGameplayEffect::StaticClass());
//			FPGAGameplayAbility->SetCooldownEffectClass(UGameplayEffect::StaticClass());
//		}
//		UE_LOG(LogTemp, Warning, TEXT("It's a GA?"));
//	}*/
//
//	//AssetToolsModule.Get().CreateAsset(AbilityName + "COST", AbilityObj->GetPathName(), Factory->SupportedClass, Factory);
//
//	//UE_LOG(LogTemp, Warning, TEXT("Created assetB"));
//}
