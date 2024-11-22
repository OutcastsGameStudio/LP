// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameMode/SR_GameMode.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SR_SR_GameMode_generated_h
#error "SR_GameMode.generated.h already included, missing '#pragma once' in SR_GameMode.h"
#endif
#define SR_SR_GameMode_generated_h

#define FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASR_GameMode(); \
	friend struct Z_Construct_UClass_ASR_GameMode_Statics; \
public: \
	DECLARE_CLASS(ASR_GameMode, AGameModeBase, COMPILED_IN_FLAGS(0 | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/SR"), NO_API) \
	DECLARE_SERIALIZER(ASR_GameMode)


#define FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_15_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ASR_GameMode(ASR_GameMode&&); \
	ASR_GameMode(const ASR_GameMode&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASR_GameMode); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASR_GameMode); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASR_GameMode) \
	NO_API virtual ~ASR_GameMode();


#define FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_12_PROLOG
#define FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_15_INCLASS_NO_PURE_DECLS \
	FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> SR_API UClass* StaticClass<class ASR_GameMode>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Dev_SR_Source_SR_GameMode_SR_GameMode_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
