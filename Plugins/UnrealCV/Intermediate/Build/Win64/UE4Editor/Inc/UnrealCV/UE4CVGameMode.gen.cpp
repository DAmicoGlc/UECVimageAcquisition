// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "UnrealCV/Public/UE4CVGameMode.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeUE4CVGameMode() {}
// Cross Module References
	UNREALCV_API UClass* Z_Construct_UClass_AUE4CVGameMode_NoRegister();
	UNREALCV_API UClass* Z_Construct_UClass_AUE4CVGameMode();
	ENGINE_API UClass* Z_Construct_UClass_AGameMode();
	UPackage* Z_Construct_UPackage__Script_UnrealCV();
	UNREALCV_API UClass* Z_Construct_UClass_AUE4CVPawn_NoRegister();
	UNREALCV_API UClass* Z_Construct_UClass_AUE4CVPawn();
	ENGINE_API UClass* Z_Construct_UClass_ADefaultPawn();
	UNREALCV_API UClass* Z_Construct_UClass_AUE4CVCharacter_NoRegister();
	UNREALCV_API UClass* Z_Construct_UClass_AUE4CVCharacter();
	ENGINE_API UClass* Z_Construct_UClass_ACharacter();
// End Cross Module References
	void AUE4CVGameMode::StaticRegisterNativesAUE4CVGameMode()
	{
	}
	UClass* Z_Construct_UClass_AUE4CVGameMode_NoRegister()
	{
		return AUE4CVGameMode::StaticClass();
	}
	struct Z_Construct_UClass_AUE4CVGameMode_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AUE4CVGameMode_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameMode,
		(UObject* (*)())Z_Construct_UPackage__Script_UnrealCV,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AUE4CVGameMode_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n *\n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "UE4CVGameMode.h" },
		{ "ModuleRelativePath", "Public/UE4CVGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AUE4CVGameMode_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AUE4CVGameMode>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AUE4CVGameMode_Statics::ClassParams = {
		&AUE4CVGameMode::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_AUE4CVGameMode_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AUE4CVGameMode_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AUE4CVGameMode()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AUE4CVGameMode_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AUE4CVGameMode, 3471628488);
	template<> UNREALCV_API UClass* StaticClass<AUE4CVGameMode>()
	{
		return AUE4CVGameMode::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AUE4CVGameMode(Z_Construct_UClass_AUE4CVGameMode, &AUE4CVGameMode::StaticClass, TEXT("/Script/UnrealCV"), TEXT("AUE4CVGameMode"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AUE4CVGameMode);
	void AUE4CVPawn::StaticRegisterNativesAUE4CVPawn()
	{
	}
	UClass* Z_Construct_UClass_AUE4CVPawn_NoRegister()
	{
		return AUE4CVPawn::StaticClass();
	}
	struct Z_Construct_UClass_AUE4CVPawn_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AUE4CVPawn_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ADefaultPawn,
		(UObject* (*)())Z_Construct_UPackage__Script_UnrealCV,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AUE4CVPawn_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * UE4CVPawn can move freely in the 3D space\n */" },
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "UE4CVGameMode.h" },
		{ "ModuleRelativePath", "Public/UE4CVGameMode.h" },
		{ "ToolTip", "UE4CVPawn can move freely in the 3D space" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AUE4CVPawn_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AUE4CVPawn>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AUE4CVPawn_Statics::ClassParams = {
		&AUE4CVPawn::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_AUE4CVPawn_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AUE4CVPawn_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AUE4CVPawn()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AUE4CVPawn_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AUE4CVPawn, 3618976572);
	template<> UNREALCV_API UClass* StaticClass<AUE4CVPawn>()
	{
		return AUE4CVPawn::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AUE4CVPawn(Z_Construct_UClass_AUE4CVPawn, &AUE4CVPawn::StaticClass, TEXT("/Script/UnrealCV"), TEXT("AUE4CVPawn"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AUE4CVPawn);
	void AUE4CVCharacter::StaticRegisterNativesAUE4CVCharacter()
	{
	}
	UClass* Z_Construct_UClass_AUE4CVCharacter_NoRegister()
	{
		return AUE4CVCharacter::StaticClass();
	}
	struct Z_Construct_UClass_AUE4CVCharacter_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AUE4CVCharacter_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ACharacter,
		(UObject* (*)())Z_Construct_UPackage__Script_UnrealCV,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AUE4CVCharacter_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * UE4CVCharacter acts like a walking human\n */" },
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "UE4CVGameMode.h" },
		{ "ModuleRelativePath", "Public/UE4CVGameMode.h" },
		{ "ToolTip", "UE4CVCharacter acts like a walking human" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AUE4CVCharacter_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AUE4CVCharacter>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AUE4CVCharacter_Statics::ClassParams = {
		&AUE4CVCharacter::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_AUE4CVCharacter_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AUE4CVCharacter_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AUE4CVCharacter()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AUE4CVCharacter_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AUE4CVCharacter, 1329420382);
	template<> UNREALCV_API UClass* StaticClass<AUE4CVCharacter>()
	{
		return AUE4CVCharacter::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AUE4CVCharacter(Z_Construct_UClass_AUE4CVCharacter, &AUE4CVCharacter::StaticClass, TEXT("/Script/UnrealCV"), TEXT("AUE4CVCharacter"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AUE4CVCharacter);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
