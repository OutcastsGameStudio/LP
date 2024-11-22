// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SR/Character/Components/SR_CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/HitResult.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSR_CharacterMovementComponent() {}

// Begin Cross Module References
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UCharacterMovementComponent();
ENGINE_API UClass* Z_Construct_UClass_UPrimitiveComponent_NoRegister();
ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FHitResult();
SR_API UClass* Z_Construct_UClass_USR_CharacterMovementComponent();
SR_API UClass* Z_Construct_UClass_USR_CharacterMovementComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_SR();
// End Cross Module References

// Begin Class USR_CharacterMovementComponent Function OnHit
struct Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics
{
	struct SR_CharacterMovementComponent_eventOnHit_Parms
	{
		UPrimitiveComponent* HitComponent;
		AActor* OtherActor;
		UPrimitiveComponent* OtherComp;
		FVector NormalImpulse;
		FHitResult Hit;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Character/Components/SR_CharacterMovementComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HitComponent_MetaData[] = {
		{ "EditInline", "true" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OtherComp_MetaData[] = {
		{ "EditInline", "true" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Hit_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_HitComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_OtherActor;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_OtherComp;
	static const UECodeGen_Private::FStructPropertyParams NewProp_NormalImpulse;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Hit;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_HitComponent = { "HitComponent", nullptr, (EPropertyFlags)0x0010000000080080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SR_CharacterMovementComponent_eventOnHit_Parms, HitComponent), Z_Construct_UClass_UPrimitiveComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HitComponent_MetaData), NewProp_HitComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_OtherActor = { "OtherActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SR_CharacterMovementComponent_eventOnHit_Parms, OtherActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_OtherComp = { "OtherComp", nullptr, (EPropertyFlags)0x0010000000080080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SR_CharacterMovementComponent_eventOnHit_Parms, OtherComp), Z_Construct_UClass_UPrimitiveComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OtherComp_MetaData), NewProp_OtherComp_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_NormalImpulse = { "NormalImpulse", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SR_CharacterMovementComponent_eventOnHit_Parms, NormalImpulse), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_Hit = { "Hit", nullptr, (EPropertyFlags)0x0010008008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SR_CharacterMovementComponent_eventOnHit_Parms, Hit), Z_Construct_UScriptStruct_FHitResult, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Hit_MetaData), NewProp_Hit_MetaData) }; // 4100991306
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_HitComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_OtherActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_OtherComp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_NormalImpulse,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::NewProp_Hit,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_USR_CharacterMovementComponent, nullptr, "OnHit", nullptr, nullptr, Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::PropPointers), sizeof(Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::SR_CharacterMovementComponent_eventOnHit_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00C20401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::Function_MetaDataParams), Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::SR_CharacterMovementComponent_eventOnHit_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USR_CharacterMovementComponent::execOnHit)
{
	P_GET_OBJECT(UPrimitiveComponent,Z_Param_HitComponent);
	P_GET_OBJECT(AActor,Z_Param_OtherActor);
	P_GET_OBJECT(UPrimitiveComponent,Z_Param_OtherComp);
	P_GET_STRUCT(FVector,Z_Param_NormalImpulse);
	P_GET_STRUCT_REF(FHitResult,Z_Param_Out_Hit);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnHit(Z_Param_HitComponent,Z_Param_OtherActor,Z_Param_OtherComp,Z_Param_NormalImpulse,Z_Param_Out_Hit);
	P_NATIVE_END;
}
// End Class USR_CharacterMovementComponent Function OnHit

// Begin Class USR_CharacterMovementComponent
void USR_CharacterMovementComponent::StaticRegisterNativesUSR_CharacterMovementComponent()
{
	UClass* Class = USR_CharacterMovementComponent::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "OnHit", &USR_CharacterMovementComponent::execOnHit },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(USR_CharacterMovementComponent);
UClass* Z_Construct_UClass_USR_CharacterMovementComponent_NoRegister()
{
	return USR_CharacterMovementComponent::StaticClass();
}
struct Z_Construct_UClass_USR_CharacterMovementComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Custom" },
		{ "IncludePath", "Character/Components/SR_CharacterMovementComponent.h" },
		{ "ModuleRelativePath", "Character/Components/SR_CharacterMovementComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WallJumpSpeed_MetaData[] = {
		{ "Category", "WallRun" },
		{ "ModuleRelativePath", "Character/Components/SR_CharacterMovementComponent.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_WallJumpSpeed;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_USR_CharacterMovementComponent_OnHit, "OnHit" }, // 1853753432
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USR_CharacterMovementComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_USR_CharacterMovementComponent_Statics::NewProp_WallJumpSpeed = { "WallJumpSpeed", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USR_CharacterMovementComponent, WallJumpSpeed), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WallJumpSpeed_MetaData), NewProp_WallJumpSpeed_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USR_CharacterMovementComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USR_CharacterMovementComponent_Statics::NewProp_WallJumpSpeed,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USR_CharacterMovementComponent_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_USR_CharacterMovementComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCharacterMovementComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_SR,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USR_CharacterMovementComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USR_CharacterMovementComponent_Statics::ClassParams = {
	&USR_CharacterMovementComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_USR_CharacterMovementComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_USR_CharacterMovementComponent_Statics::PropPointers),
	0,
	0x00B000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USR_CharacterMovementComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_USR_CharacterMovementComponent_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_USR_CharacterMovementComponent()
{
	if (!Z_Registration_Info_UClass_USR_CharacterMovementComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USR_CharacterMovementComponent.OuterSingleton, Z_Construct_UClass_USR_CharacterMovementComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USR_CharacterMovementComponent.OuterSingleton;
}
template<> SR_API UClass* StaticClass<USR_CharacterMovementComponent>()
{
	return USR_CharacterMovementComponent::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(USR_CharacterMovementComponent);
USR_CharacterMovementComponent::~USR_CharacterMovementComponent() {}
// End Class USR_CharacterMovementComponent

// Begin Registration
struct Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USR_CharacterMovementComponent, USR_CharacterMovementComponent::StaticClass, TEXT("USR_CharacterMovementComponent"), &Z_Registration_Info_UClass_USR_CharacterMovementComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USR_CharacterMovementComponent), 2113125801U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_501935570(TEXT("/Script/SR"),
	Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
