// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSR_init() {}
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_SR;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_SR()
	{
		if (!Z_Registration_Info_UPackage__Script_SR.OuterSingleton)
		{
			static const UECodeGen_Private::FPackageParams PackageParams = {
				"/Script/SR",
				nullptr,
				0,
				PKG_CompiledIn | 0x00000000,
				0x728C935B,
				0x5CCDCAA8,
				METADATA_PARAMS(0, nullptr)
			};
			UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_SR.OuterSingleton, PackageParams);
		}
		return Z_Registration_Info_UPackage__Script_SR.OuterSingleton;
	}
	static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_SR(Z_Construct_UPackage__Script_SR, TEXT("/Script/SR"), Z_Registration_Info_UPackage__Script_SR, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0x728C935B, 0x5CCDCAA8));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
