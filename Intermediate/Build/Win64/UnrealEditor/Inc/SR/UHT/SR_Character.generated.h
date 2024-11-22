// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Character/SR_Character.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SR_SR_Character_generated_h
#error "SR_Character.generated.h already included, missing '#pragma once' in SR_Character.h"
#endif
#define SR_SR_Character_generated_h

#define FID_Dev_SR_Source_SR_Character_SR_Character_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execTryWallJump);


#define FID_Dev_SR_Source_SR_Character_SR_Character_h_21_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASR_Character(); \
	friend struct Z_Construct_UClass_ASR_Character_Statics; \
public: \
	DECLARE_CLASS(ASR_Character, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SR"), NO_API) \
	DECLARE_SERIALIZER(ASR_Character)


#define FID_Dev_SR_Source_SR_Character_SR_Character_h_21_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ASR_Character(ASR_Character&&); \
	ASR_Character(const ASR_Character&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASR_Character); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASR_Character); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASR_Character) \
	NO_API virtual ~ASR_Character();


#define FID_Dev_SR_Source_SR_Character_SR_Character_h_18_PROLOG
#define FID_Dev_SR_Source_SR_Character_SR_Character_h_21_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Dev_SR_Source_SR_Character_SR_Character_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Dev_SR_Source_SR_Character_SR_Character_h_21_INCLASS_NO_PURE_DECLS \
	FID_Dev_SR_Source_SR_Character_SR_Character_h_21_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> SR_API UClass* StaticClass<class ASR_Character>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Dev_SR_Source_SR_Character_SR_Character_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
