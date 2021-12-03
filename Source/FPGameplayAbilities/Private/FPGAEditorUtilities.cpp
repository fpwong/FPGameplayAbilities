// //// Fill out your copyright notice in the Description page of Project Settings.
// //
// #include "FPGAEditorUtilities.h"
// #include "Factories/AnimMontageFactory.h"
// #include "AssetToolsModule.h"
// #include "DesktopPlatform/Public/IDesktopPlatform.h"
// #include "DesktopPlatform/Public/DesktopPlatformModule.h"
// #include "Framework/Application/SlateApplication.h"
// #include "EditorDirectories.h"
// #include "Misc/MessageDialog.h"
// #include "Misc/PackageName.h"
// #include "Factories/BlueprintFactory.h"
// #include "Factories/AnimBlueprintFactory.h"
// #include "Factories/BlendSpaceFactory1D.h"
// #include "Animation/BlendSpaceBase.h"
// #include "Animation/AnimInstance.h"
// #include "GameFramework/Character.h"
// #include "Kismet2/KismetEditorUtilities.h"
// #include "Animation/AnimBlueprint.h"
// //#include "AssetToolsModule.h"
// //#include "GameplayAbilitiesEditor/Public/GameplayAbilitiesBlueprintFactory.h"
// //#include "AbilitySystem/FPGAGameplayAbility.h"
// //#include "GameplayAbilityBlueprint.h"
// //#include "Engine/ObjectLibrary.h"
// //#include "Engine/AssetManager.h"
// //#include "Engine/BlueprintGeneratedClass.h"
// //
// //void UFPGAEditorUtilities::CreateAbility()
// //{
// //	// Create an appropriate and unique name 
// //
// //	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
// //	//AssetToolsModule.Get().CreateUniqueAssetName(Parent->GetOutermost()->GetName(), DefaultSuffix, PackageName, Name);
// //
// //	UGameplayAbilitiesBlueprintFactory* Factory = NewObject<UGameplayAbilitiesBlueprintFactory>();
// //	//Factory->ParentClass = ParentClass;
// //	//UObject* Child = AssetToolsModule.Get().CreateAssetWithDialog(AbilityName, FPackageName::GetLongPackagePath(Location), Factory->ParentClass, Factory);
// //	//UObject* AbilityObj = AssetToolsModule.Get().CreateAssetWithDialog(AssetName, FPackageName::GetLongPackagePath(AbilityPath), Factory->SupportedClass, Factory);
// //
// //	FString AssetName;
// //	FString AbilityPath = "/Game/";
// //	UObject* AbilityObj = AssetToolsModule.Get().CreateAssetWithDialog(Factory->SupportedClass, Factory);
// //
// //	UE_LOG(LogTemp, Warning, TEXT("Created asset 2"));
// //
// //	//	FindField;
// //	//if (AbilityObj)
// //	//   {
// //	//       float FoundFloat, IncreasedFloat;
// //	//       UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);
// //	//       {
// //	//           if (FloatProp)
// //	//           {
// //	//               FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
// //	//               IncreasedFloat = FoundFloat + ValueToAdd;  //increase the value 
// //	//               FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat); //and then set it
// //	//               outFloat = IncreasedFloat;
// //	//               return true;
// //	//           }
// //	//       }
// //	//   }
// //
// //	//if (UFPGAGameplayAbility* GameplayAbility = Cast<UFPGAGameplayAbility>(AbilityObj))
// //	if (UGameplayAbilityBlueprint* Blueprint = Cast<UGameplayAbilityBlueprint>(AbilityObj))
// //	{
// //		UE_LOG(LogTemp, Warning, TEXT("Try obj library %s | %s | %s"), *Blueprint->GetPathName(), *Blueprint->BlueprintDisplayName, *Blueprint->GetName());
// //		//UObjectLibrary* ObjLibrary = UObjectLibrary::CreateLibrary(UGameplayAbilityBlueprint::StaticClass(), true, true);
// //		UObjectLibrary* ObjLibrary = UObjectLibrary::CreateLibrary(UGameplayAbility::StaticClass(), true, true);
// //
// //		//UObjectLibrary* ObjLibrary = UObjectLibrary::CreateLibrary(InterfaceClass, true, true);
// //		//ObjLibrary->LoadBlueprintAssetDataFromPath(TEXT("/Game/"));
// //		//ObjLibrary->GetAssetDataList(GameplayCueInterfaceActors);
// //
// //		FString ObjectPath = Blueprint->GetPathName() + "_C";
// //
// //		UE_LOG(LogTemp, Warning, TEXT("Checking obj path 1 %s"), *ObjectPath);
// //
// //		UObject* Target = FindObject<UObject>(NULL, *ObjectPath);
// //		if (Target == NULL)
// //		{
// //			Target = LoadObject<UObject>(NULL, *ObjectPath);
// //			UE_LOG(LogTemp, Warning, TEXT("My Target?"));
// //		}
// //
// //		if (UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(Target))
// //		{
// //			UE_LOG(LogTemp, Warning, TEXT("BP gen Class is valid"));
// //			for (auto elem : BPClass->PropertyGuids)
// //			{
// //				UE_LOG(LogTemp, Warning, TEXT("%s"), *elem.Key.ToString());
// //			}
// //		}
// //
// //		//Blueprint->GetClass();
// //
// //		TArray<UObject*> SubObjects;
// //		Blueprint->GetDefaultSubobjects(SubObjects);
// //
// //		UObject* DefaultObject = Target->GetClass()->GetDefaultObject();
// //
// //		UE_LOG(LogTemp, Warning, TEXT("Default Obj %s"), *DefaultObject->GetName());
// //
// //		for (UObject* Obj : SubObjects)
// //		{
// //			for (TFieldIterator<UProperty> PropIt(Obj->GetClass()); PropIt; ++PropIt)
// //			{
// //				UProperty* Property = *PropIt;
// //				// Do something with the property
// //				UE_LOG(LogTemp, Warning, TEXT("SubObjs Property %s"), *Property->GetDisplayNameText().ToString());
// //			}
// //		}
// //
// //		for (TFieldIterator<UProperty> PropIt(DefaultObject->GetClass()); PropIt; ++PropIt)
// //		{
// //			UProperty* Property = *PropIt;
// //			// Do something with the property
// //			UE_LOG(LogTemp, Warning, TEXT("Default Obj Property %s"), *Property->GetDisplayNameText().ToString());
// //		}
// //
// //		if (Target)
// //		{
// //			//UE_LOG(LogTemp, Warning, TEXT("%s | %s | %s"), *Blueprint->GetFriendlyName(), *Blueprint->GetBlueprintClass()->FindPropertyByName());
// //
// //			//Target->GetBlueprintClass();
// //
// //			UE_LOG(LogTemp, Warning, TEXT("Target is not null? 2 %s"), *Target->GetClass()->GetPathName());
// //
// //			//for (TFieldIterator<UProperty> PropIt(Target->GetClass()); PropIt; ++PropIt)
// //			for (TFieldIterator<UProperty> PropIt(Target->GetClass()); PropIt; ++PropIt)
// //			{
// //				UProperty* Property = *PropIt;
// //				// Do something with the property
// //				UE_LOG(LogTemp, Warning, TEXT("Property %s"), *Property->GetDisplayNameText().ToString());
// //			}
// //
// //			float FoundFloat, IncreasedFloat;
// //			UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), TEXT("Testing"));
// //			{
// //				if (FloatProp)
// //				{
// //					UE_LOG(LogTemp, Warning, TEXT("I FOUND THE FLOAT PROPERYT"));
// //
// //					FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
// //					IncreasedFloat = FoundFloat + 10; //increase the value 
// //					FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat); //and then set it
// //					//outFloat = IncreasedFloat;
// //				}
// //			}
// //		}
// //
// //		//return;
// //
// //		if (ObjLibrary)
// //		{
// //			//ObjLibrary->AddToRoot();
// //			//ObjLibrary->LoadBlueprintsFromPath(Blueprint->GetPathName());
// //			//ObjLibrary->LoadBlueprintAssetDataFromPath(AbilityPath);
// //
// //			FString Left;
// //			FString Right;
// //			if (Blueprint->GetPathName().Split("/", &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
// //			{
// //				Left = "/Game" + Left + "/";
// //				UE_LOG(LogTemp, Warning, TEXT("%s"), *Left);
// //
// //				//ObjLibrary->LoadBlueprintAssetDataFromPath(Left);
// //
// //				ObjLibrary->LoadBlueprintAssetDataFromPath("/Game/");
// //
// //				//UE_LOG(LogTemp, Warning, TEXT("Path %s"), *Left);
// //				ObjLibrary->LoadAssetsFromAssetData();
// //
// //				TArray<FAssetData> AssetDatas;
// //				ObjLibrary->GetAssetDataList(AssetDatas);
// //				UE_LOG(LogTemp, Warning, TEXT("Found objects: %d"), AssetDatas.Num());
// //
// //				for (FAssetData& Data : AssetDatas)
// //				{
// //					UE_LOG(LogTemp, Warning, TEXT("\tAsset Data %s"), *Data.AssetName.ToString());
// //				}
// //
// //				if (AssetDatas.Num() > 0)
// //				{
// //					AssetDatas[0].ToStringReference().TryLoad();
// //					UObject* Target = AssetDatas[0].GetAsset(); //Always returns null
// //					UE_LOG(LogTemp, Warning, TEXT("Teinartarts"));
// //
// //					if (Target)
// //					{
// //						for (TFieldIterator<UProperty> PropIt(Target->GetClass()); PropIt; ++PropIt)
// //						{
// //							UProperty* Property = *PropIt;
// //							// Do something with the property
// //							UE_LOG(LogTemp, Warning, TEXT("ObjLibrary Property %s"), *Property->GetDisplayNameText().ToString());
// //						}
// //
// //						//float FoundFloat; //, IncreasedFloat;
// //						//UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), "Testing");
// //						//{
// //						//	if (FloatProp)
// //						//	{
// //						//		FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
// //						//		//IncreasedFloat = FoundFloat + ValueToAdd; //increase the value 
// //						//		//FloatProp->SetPropertyValue_InContainer(Target, IncreasedFloat); //and then set it
// //						//		//outFloat = IncreasedFloat;
// //						//	}
// //						//}
// //					}
// //				}
// //			}
// //		}
// //	}
// //	/*if (UGameplayAbility* GameplayAbility = Cast<UGameplayAbility>(AbilityObj))
// //	{
// //		if (UFPGAGameplayAbility * FPGAGameplayAbility = Cast<UFPGAGameplayAbility>(AbilityObj))
// //		{
// //			UE_LOG(LogTemp, Warning, TEXT("Set variables!"));
// //
// //			FPGAGameplayAbility->SetCostEffectClass(UGameplayEffect::StaticClass());
// //			FPGAGameplayAbility->SetCooldownEffectClass(UGameplayEffect::StaticClass());
// //		}
// //		UE_LOG(LogTemp, Warning, TEXT("It's a GA?"));
// //	}*/
// //
// //	//AssetToolsModule.Get().CreateAsset(AbilityName + "COST", AbilityObj->GetPathName(), Factory->SupportedClass, Factory);
// //
// //	//UE_LOG(LogTemp, Warning, TEXT("Created assetB"));
// //}
//
//
// UFPGAEditorUtilities::UFPGAEditorUtilities()
// {
// 	DefaultPath.Path = TEXT("/Game");
// 	CharacterName = "DefaultName";
// 	CharacterClass = ACharacter::StaticClass();
// 	AnimInstanceClass = UAnimInstance::StaticClass();
// }
//
// void UFPGAEditorUtilities::CreateCharacter()
// {
// 	if (!CharacterSkeleton)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("%s No character skeleton selected"), TEXT(__FUNCTION__));
// 		return;
// 	}
// 	
// 	static FName AssetToolsModuleName = FName("AssetTools");
// 	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>(AssetToolsModuleName);
// 	
// 	if (!SelectDestinationFolder())
// 	{
// 		return;
// 	}
//
// 	for (FString Suffix : MontagesToCreate)
// 	{
// 		CreateMontage(AssetToolsModule, Suffix);
// 	}
//
// 	CreateAnimBP(AssetToolsModule);
// 	CreateBlendspace(AssetToolsModule);
//
// 	CreateCharacter(AssetToolsModule);
// 	
// 	// UE_LOG(LogTemp, Warning, TEXT("%s |%s | %s"), *DestinationFolder, *PackagePath, *OutPath);
// 	// AssetToolsModule.Get().CreateUniqueAssetName(Path + TEXT("/") + NewFactory->GetDefaultNewAssetName(), TEXT(""), PackageNameToUse, DefaultAssetName);
// 	// CreateNewAsset(DefaultAssetName, SelectedPath, NewFactory->GetSupportedClass(), NewFactory);
// }
//
// void UFPGAEditorUtilities::TryLoadObj()
// {
// 	FString FullPath = FString::Printf(TEXT("%sBP_%s.BP_%s"), *DefaultPath.Path, *CharacterName, *CharacterName);
//
// 	UE_LOG(LogTemp, Warning, TEXT("Tryin to load object?"));
//
// 	auto Actor = LoadObject<AActor>(nullptr, TEXT("/Game/ArenaGame/Heroes/Test/BP_Test241.BP_Test241"));
// 	if (Actor)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Loaded properly?"));
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Failed to load actor"));
// 	}
// }
//
// bool UFPGAEditorUtilities::SelectDestinationFolder()
// {
// 	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
// 	if (!DesktopPlatform)
// 		return false;
// 	
// 	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
//
// 	const FString Title = FString("Choose a destination");
// 	bool bFolderAccepted = false;
//
// 	FString FilenameDefaultPath;
// 	FPackageName::TryConvertLongPackageNameToFilename(DefaultPath.Path, FilenameDefaultPath);
//
// 	FString SelectedFolder;
// 	while (!bFolderAccepted)
// 	{
// 		const bool bFolderSelected = DesktopPlatform->OpenDirectoryDialog(
// 			ParentWindowWindowHandle,
// 			Title,
// 			FilenameDefaultPath,
// 			SelectedFolder
// 		);
//
// 		if (!bFolderSelected)
// 		{
// 			// User canceled, return
// 			return false;
// 		}
//
// 		FPaths::NormalizeFilename(SelectedFolder);
// 		if (!SelectedFolder.EndsWith(TEXT("/")))
// 		{
// 			SelectedFolder += TEXT("/");
// 		}
//
// 		bFolderAccepted = true;
// 	}
//
// 	FPackageName::TryConvertFilenameToLongPackageName(SelectedFolder, DestinationFolder);
// 	return true;
// }
//
// void UFPGAEditorUtilities::CreateMontage(FAssetToolsModule& AssetToolsModule, FString Suffix)
// {
// 	UAnimMontageFactory* Factory = NewObject<UAnimMontageFactory>();
// 	Factory->TargetSkeleton = CharacterSkeleton->Skeleton;
//
// 	FString Name = FString::Printf(TEXT("Montage_%s_%s"), *CharacterName, *Suffix);
// 	UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(Name, DestinationFolder, Factory->SupportedClass, Factory);
// 	UAnimMontage* Montage = Cast<UAnimMontage>(CreatedAsset);
// 	if (Montage)
// 	{
// 		Montage->BlendIn = 0.1f;
// 		Montage->BlendOut = 0.1f;
//
// 		if (Suffix == "Death")
// 		{
// 			Montage->bEnableAutoBlendOut = false;
// 		}
// 	}
// 	
// 	if (CreatedAsset)
// 	{
// 		SaveAsset(CreatedAsset);
// 	}
// }
//
// void UFPGAEditorUtilities::CreateCharacter(FAssetToolsModule& AssetToolsModule)
// {
// 	UBlueprintFactory* Factory = NewObject<UBlueprintFactory>();
// 	Factory->ParentClass = CharacterClass;
//
// 	FString Name = FString::Printf(TEXT("BP_%s"), *CharacterName);
//
// 	UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(Name, DestinationFolder, Factory->SupportedClass, Factory);
//
// 	if (CreatedAsset)
// 	{
// 		SaveAsset(CreatedAsset);
// 		EditCharacterBlueprint(CreatedAsset);
// 	}
// }
//
// void UFPGAEditorUtilities::CreateAnimBP(FAssetToolsModule& AssetToolsModule)
// {
// 	UAnimBlueprintFactory* Factory = NewObject<UAnimBlueprintFactory>();
// 	Factory->TargetSkeleton = CharacterSkeleton->Skeleton;
// 	Factory->ParentClass = AnimInstanceClass;
//
// 	FString Name = FString::Printf(TEXT("ABP_%s"), *CharacterName);
// 	UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(Name, DestinationFolder, Factory->SupportedClass, Factory);
// 	if (CreatedAsset)
// 	{
// 		SaveAsset(CreatedAsset);
// 		CreatedAnimBlueprint = Cast<UAnimBlueprint>(CreatedAsset);
// 		UE_LOG(LogTemp, Warning, TEXT("%s"), *CreatedAsset->GetName());
// 	}
// }
//
// void UFPGAEditorUtilities::CreateBlendspace(FAssetToolsModule& AssetToolsModule)
// {
// 	UBlendSpaceFactory1D* Factory = NewObject<UBlendSpaceFactory1D>();
// 	Factory->TargetSkeleton = CharacterSkeleton->Skeleton;
//
// 	FString Name = FString::Printf(TEXT("BS_%s_Walk"), *CharacterName);
// 	UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(Name, DestinationFolder, Factory->SupportedClass, Factory);
//
// 	if (CreatedAsset)
// 	{
// 		SaveAsset(CreatedAsset);
// 	}
// 	
// 	// UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(CharacterName, DestinationFolder, Factory->SupportedClass, Factory);
// 	// if (UBlendSpaceBase* BlendSpace = Cast<UBlendSpaceBase>(CreatedAsset))
// 	// {
// 	// 	BlendSpace->GetBlendParameter(0);
// 	// }
// }
//
// void UFPGAEditorUtilities::EditCharacterBlueprint(UObject* CreatedAsset)
// {
// 	// UBlueprint* Blueprint = Cast<UBlueprint>(Object);
// 	UBlueprint* Blueprint = Cast<UBlueprint>(CreatedAsset);
// 	check(Blueprint);
//
// 	// FKismetEditorUtilities::CompileBlueprint(Blueprint);
//
//
// 	// if (auto BoolProperty = FindField<UBoolProperty>(Blueprint->GetClass(), "bRunConstructionScriptInSequencer"))
// 	// {
// 	// 	UE_LOG(LogTemp, Warning, TEXT("Set bAlwaysRelevant"))
// 	//
// 	// 	auto Oldv = BoolProperty->GetPropertyValue(Blueprint);
// 	// 	UE_LOG(LogTemp, Warning, TEXT("Old value %s"), Oldv ? *FString("True") : *FString("False"));
// 	//
// 	// 	BoolProperty->SetPropertyValue_InContainer(Blueprint, true);
// 	// 	auto Test = BoolProperty->GetPropertyValue(Blueprint);
// 	// 	UE_LOG(LogTemp, Warning, TEXT("New value %s"), Test ? *FString("True") : *FString("False"));
// 	// }
// 	//
// 	//
// 	// auto CDO = Blueprint->GeneratedClass->GetDefaultObject();
// 	//
// 	// // TESTING STUFF
// 	// if (auto IsCrouched = Cast<UBoolProperty>(Blueprint->GeneratedClass->FindPropertyByName("bAlwaysRelevant")))
// 	// {
// 	// 	UE_LOG(LogTemp, Warning, TEXT("Set bAlwaysRelevant"))
// 	//
// 	// 		auto Oldv = IsCrouched->GetPropertyValue(CDO);
// 	// 	UE_LOG(LogTemp, Warning, TEXT("Old value %s"), Oldv ? *FString("True") : *FString("False"));
// 	//
// 	//
// 	// 	IsCrouched->SetPropertyValue_InContainer(CDO, true);
// 	// 	auto Test = IsCrouched->GetPropertyValue(CDO);
// 	// 	UE_LOG(LogTemp, Warning, TEXT("New value %s"), Test ? *FString("True") : *FString("False"));
// 	// }
// 	//
// 	// for (TFieldIterator<UProperty> PropIt(Blueprint->GeneratedClass.Get()); PropIt; ++PropIt)
// 	// {
// 	// 	UProperty* Property = *PropIt;
// 	// 	UE_LOG(LogTemp, Warning, TEXT("\tGen class Property %s %s"), *Property->GetName(), *Property->GetClass()->GetName());
// 	// }
// 	//
// 	// // Get the mesh component
// 	// if (UProperty* MeshProperty = Blueprint->GeneratedClass->FindPropertyByName("Mesh"))
// 	// {
// 	// 	UE_LOG(LogTemp, Warning, TEXT("Found mesh property!"));
// 	//
// 	// 	if (UObjectProperty* ObjProperty = Cast<UObjectProperty>(MeshProperty))
// 	// 	{
// 	// 		// set the skeleton
// 	// 		UE_LOG(LogTemp, Warning, TEXT("Is object proprety"));
// 	// 		if (auto SkeleMeshProperty = Cast<UObjectProperty>(ObjProperty->PropertyClass->FindPropertyByName("SkeletalMesh")))
// 	// 		{
// 	// 			UE_LOG(LogTemp, Warning, TEXT("Set skele mesh"));
// 	// 			auto SkeleMeshDefaultObj = ObjProperty->PropertyClass->GetDefaultObject();
// 	// 			SkeleMeshProperty->SetObjectPropertyValue(SkeleMeshProperty->ContainerPtrToValuePtr<UObject*>(SkeleMeshDefaultObj), CharacterSkeleton->GetClass());
// 	// 		}
// 	//
// 	// 		for (TFieldIterator<UProperty> PropIt(ObjProperty->PropertyClass); PropIt; ++PropIt)
// 	// 		{
// 	// 			UProperty* Property = *PropIt;
// 	// 			UE_LOG(LogTemp, Warning, TEXT("\tMesh Property %s %s"), *Property->GetName(), *Property->GetClass()->GetName());
// 	// 		}
// 	// 	}
// 	// }
// 	
// 	if (auto CreatedActorr = Cast<AActor>(Blueprint->GeneratedClass->GetDefaultObject()))
// 	{
// 		if (ACharacter* Character = Cast<ACharacter>(CreatedActorr))
// 		{
// 			Character->GetMesh()->SetSkeletalMesh(CharacterSkeleton);
//
// 			if (CreatedAnimBlueprint)
// 			{
// 				Character->GetMesh()->SetAnimClass(CreatedAnimBlueprint->GeneratedClass.Get());
// 			}
// 			UE_LOG(LogTemp, Warning, TEXT("Set character skeleton"));
// 		}
// 	}
// }
//
// void UFPGAEditorUtilities::SaveAsset(UObject* CreatedAsset)
// {
// 	UPackage* const Package = CreatedAsset->GetOutermost();
// 	FString const PackageName = Package->GetName();
// 	FString const PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
// 	UPackage::SavePackage(Package, nullptr, RF_Standalone, *PackageFileName, GError, nullptr, false, true, SAVE_NoError);
// }
