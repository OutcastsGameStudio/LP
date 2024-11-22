// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SR/GameMode/SR_GameMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSR_GameMode() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
SR_API UClass* Z_Construct_UClass_ASR_GameMode();
SR_API UClass* Z_Construct_UClass_ASR_GameMode_NoRegister();
UPackage* Z_Construct_UPackage__Script_SR();
// End Cross Module References

// Begin Class ASR_GameMode
void ASR_GameMode::StaticRegisterNativesASR_GameMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ASR_GameMode);
UClass* Z_Construct_UClass_ASR_GameMode_NoRegister()
{
	return ASR_GameMode::StaticClass();
}
struct Z_Construct_UClass_ASR_GameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "GameMode/SR_GameMode.h" },
		{ "ModuleRelativePath", "GameMode/SR_GameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASR_GameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ASR_GameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_SR,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASR_GameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASR_GameMode_Statics::ClassParams = {
	&ASR_GameMode::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASR_GameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_ASR_GameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ASR_GameMode()
{
	if (!Z_Registration_Info_UClass_ASR_GameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASR_GameMode.OuterSingleton, Z_Construct_UClass_ASR_GameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASR_GameMode.OuterSingleton;
}
template<> SR_API UClass* StaticClass<ASR_GameMode>()
{
	return ASR_GameMode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ASR_GameMode);
ASR_GameMode::~ASR_GameMode() {}
// End Class ASR_GameMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASR_GameMode, ASR_GameMode::StaticClass, TEXT("ASR_GameMode"), &Z_Registration_Info_UClass_ASR_GameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASR_GameMode), 57958807U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_1951810800(TEXT("/Script/SR"),
	Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
