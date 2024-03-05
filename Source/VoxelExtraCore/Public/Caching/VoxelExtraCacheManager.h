#pragma once

#include "VoxelMinimal.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Array.h"
#include "Misc/DateTime.h"
#include "VoxelExtraCacheManager.generated.h"

USTRUCT()
struct FVoxelExtraCachedData
{
    GENERATED_BODY()

    UPROPERTY()
    FString Key;

    UPROPERTY()
    TArray<uint16> Data;

    UPROPERTY()
    FDateTime Timestamp;

    FVoxelExtraCachedData() {}

    FVoxelExtraCachedData(const TArray<uint16>& InData) : Data(InData), Timestamp(FDateTime::Now()) {}
};

/**
 * A simple cache manager with thread-safe caching and expiration logic.
 */
UCLASS()
class VOXELEXTRACORE_API UVoxelExtraCacheManager : public UObject
{
    GENERATED_BODY()

public:
    UVoxelExtraCacheManager();
	
	/**
     * Returns Singleton
     */
	static UVoxelExtraCacheManager* GetInstance();

    /**
     * Cache data with a generic key. If the key already exists, it updates the existing entry.
     * @param Key - The generic key associated with the data.
     * @param DataToCache - The data to be cached.
     */
    UFUNCTION()
    void CacheData(const FString& Key, const TArray<uint16>& DataToCache);

    /**
     * Retrieve cached data based on a generic key.
     * @param Key - The generic key associated with the data.
     * @param OutData - (Out) The retrieved data.
     * @return True if the key was found, false otherwise.
     */
    UFUNCTION()
    bool RetrieveData(const FString& Key, TArray<uint16>& OutData);

private:
    /**
     * Remove expired items from the cache.
     */
    void RemoveExpiredItems();
	
	/** Instance of manager. */
    static UVoxelExtraCacheManager* Instance;

    /** The array of cached data entries. */
    TArray<FVoxelExtraCachedData> CachedArray;

    /** The critical section to ensure thread safety. */
    FVoxelCriticalSection CriticalSection;

    /** The expiration time for cached items. */
    FTimespan CacheExpirationTime;

    /** The timer handle for periodic cleanup. */
    FTimerHandle CleanupTimerHandle;
};
