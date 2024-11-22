// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Character/Components/SR_CharacterMovementComponent.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
class UPrimitiveComponent;
struct FHitResult;
#ifdef SR_SR_CharacterMovementComponent_generated_h
#error "SR_CharacterMovementComponent.generated.h already included, missing '#pragma once' in SR_CharacterMovementComponent.h"
#endif
#define SR_SR_CharacterMovementComponent_generated_h

#define FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnHit);


#define FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUSR_CharacterMovementComponent(); \
	friend struct Z_Construct_UClass_USR_CharacterMovementComponent_Statics; \
public: \
	DECLARE_CLASS(USR_CharacterMovementComponent, UCharacterMovementComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SR"), NO_API) \
	DECLARE_SERIALIZER(USR_CharacterMovementComponent)


#define FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_13_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	USR_CharacterMovementComponent(USR_CharacterMovementComponent&&); \
	USR_CharacterMovementComponent(const USR_CharacterMovementComponent&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, USR_CharacterMovementComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(USR_CharacterMovementComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(USR_CharacterMovementComponent) \
	NO_API virtual ~USR_CharacterMovementComponent();


#define FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_10_PROLOG
#define FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_13_INCLASS_NO_PURE_DECLS \
	FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> SR_API UClass* StaticClass<class USR_CharacterMovementComponent>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Dev_SR_Source_SR_Character_Components_SR_CharacterMovementComponent_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
