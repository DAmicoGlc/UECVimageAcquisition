// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "UnrealCV/Public/GTCaptureComponent.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGTCaptureComponent() {}
// Cross Module References
	UNREALCV_API UClass* Z_Construct_UClass_UGTCaptureComponent_NoRegister();
	UNREALCV_API UClass* Z_Construct_UClass_UGTCaptureComponent();
	ENGINE_API UClass* Z_Construct_UClass_USceneComponent();
	UPackage* Z_Construct_UPackage__Script_UnrealCV();
// End Cross Module References
	void UGTCaptureComponent::StaticRegisterNativesUGTCaptureComponent()
	{
	}
	UClass* Z_Construct_UClass_UGTCaptureComponent_NoRegister()
	{
		return UGTCaptureComponent::StaticClass();
	}
	struct Z_Construct_UClass_UGTCaptureComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGTCaptureComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_USceneComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_UnrealCV,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGTCaptureComponent_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Use USceneCaptureComponent2D to export information from the scene.\n * This class needs to be tickable to update the rotation of the USceneCaptureComponent2D\n */" },
		{ "HideCategories", "Trigger PhysicsVolume" },
		{ "IncludePath", "GTCaptureComponent.h" },
		{ "ModuleRelativePath", "Public/GTCaptureComponent.h" },
		{ "ToolTip", "Use USceneCaptureComponent2D to export information from the scene.\nThis class needs to be tickable to update the rotation of the USceneCaptureComponent2D" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGTCaptureComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGTCaptureComponent>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGTCaptureComponent_Statics::ClassParams = {
		&UGTCaptureComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UGTCaptureComponent_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UGTCaptureComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGTCaptureComponent()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGTCaptureComponent_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGTCaptureComponent, 2420693668);
	template<> UNREALCV_API UClass* StaticClass<UGTCaptureComponent>()
	{
		return UGTCaptureComponent::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGTCaptureComponent(Z_Construct_UClass_UGTCaptureComponent, &UGTCaptureComponent::StaticClass, TEXT("/Script/UnrealCV"), TEXT("UGTCaptureComponent"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGTCaptureComponent);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
