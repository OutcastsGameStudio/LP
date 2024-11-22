// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Character/Controller/SR_PlayerController.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SR_SR_PlayerController_generated_h
#error "SR_PlayerController.generated.h already included, missing '#pragma once' in SR_PlayerController.h"
#endif
#define SR_SR_PlayerController_generated_h

#define FID_Dev_SR_Source_SR_Character_Controller_SR_PlayerController_h_17_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASR_PlayerController(); \
	friend struct Z_Construct_UClass_ASR_PlayerController_Statics; \
public: \
	DECLARE_CLASS(ASR_PlayerController, APlayerController, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SR"), NO_API) \
	DECLARE_SERIALIZER(ASR_PlayerController)


#define FID_Dev_SR_Source_SR_Character_Controller_SR_PlayerController_h_17_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ASR_PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ASR_PlayerController(ASR_PlayerController&&); \
	ASR_PlayerController(const ASR_PlayerController&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASR_PlayerController); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASR_PlayerController); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ASR_PlayerController) \
	NO_API virtual ~ASR_PlayerController();


#define FID_Dev_SR_Source_SR_Character_Controller_SR_PlayerController_h_14_PROLOG
#define FID_Dev_SR_Source_SR_Character_Controller_SR_PlayerController_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Dev_SR_Source_SR_Character_Controller_SR_PlayerController_h_17_INCLASS_NO_PURE_DECLS \
	FID_Dev_SR_Source_SR_Character_Controller_SR_PlayerController_h_17_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> SR_API UClass* StaticClass<class ASR_PlayerController>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Dev_SR_Source_SR_Character_Controller_SR_PlayerController_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
