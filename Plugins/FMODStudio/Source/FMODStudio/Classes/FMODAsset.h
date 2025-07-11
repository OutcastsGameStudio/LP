// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2025.

#pragma once

#include "Misc/Guid.h"
#include "CoreMinimal.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "FMODAsset.generated.h"

class FAssetRegistryTagsContext;
/**
 * FMOD Asset.
 */
UCLASS(BlueprintType)
class FMODSTUDIO_API UFMODAsset : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    /** The unique Guid, which matches the one exported from FMOD Studio */
    UPROPERTY()
    FGuid AssetGuid;

protected:
    /** Get tags to show in content view */
    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;

private:
    /** Non default instances of UFMODAsset are assets */
    virtual bool IsAsset() const override;
};
