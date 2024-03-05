#pragma once

#include "Caching/VoxelExtraCacheManager.h"
#include "TimerManager.h"

UVoxelExtraCacheManager* UVoxelExtraCacheManager::Instance = nullptr;

UVoxelExtraCacheManager::UVoxelExtraCacheManager()
{
    CacheExpirationTime = FTimespan(0, 0, 10);
}

UVoxelExtraCacheManager* UVoxelExtraCacheManager::GetInstance()
{
    if (!Instance)
    {
        // If the instance does not exist, create it
        Instance = NewObject<UVoxelExtraCacheManager>();
    }

    return Instance;
}

void UVoxelExtraCacheManager::CacheData(const FString& Key, const TArray<uint16>& DataToCache)
{
    // Lock the critical section to ensure thread safety
    VOXEL_SCOPE_LOCK(CriticalSection);

    // Check if the key already exists in the cache and update it if it does
    for (int32 Index = 0; Index < CachedArray.Num(); ++Index)
    {
        if (CachedArray[Index].Key == Key)
        {
            CachedArray[Index].Data = DataToCache;
            CachedArray[Index].Timestamp = FDateTime::Now();
            return; // Exit early since the key was found and updated
        }
    }

    // If the key does not exist, add a new entry to the cache
    FVoxelExtraCachedData CachedItem(DataToCache);
    CachedItem.Key = Key;
    CachedArray.Add(CachedItem);

    // Set up a timer to check for and remove expired items
    GetWorld()->GetTimerManager().SetTimer(CleanupTimerHandle, this, &UVoxelExtraCacheManager::RemoveExpiredItems, 1.0f, true);
}

bool UVoxelExtraCacheManager::RetrieveData(const FString& Key, TArray<uint16>& OutData)
{
    // Lock the critical section to ensure thread safety
    VOXEL_SCOPE_LOCK(CriticalSection);

    // Find the entry with the specified key
    for (const FVoxelExtraCachedData& CachedItem : CachedArray)
    {
        if (CachedItem.Key == Key)
        {
            OutData = CachedItem.Data;
            return true; // Data found and retrieved
        }
    }

    return false; // Key not found
}

void UVoxelExtraCacheManager::RemoveExpiredItems()
{
    const FDateTime CurrentTime = FDateTime::Now();

    // Lock the critical section to ensure thread safety
    VOXEL_SCOPE_LOCK(CriticalSection);

    for (int32 Index = CachedArray.Num() - 1; Index >= 0; --Index)
    {
        if ((CurrentTime - CachedArray[Index].Timestamp) >= CacheExpirationTime)
        {
            CachedArray.RemoveAt(Index);
        }
    }
}
