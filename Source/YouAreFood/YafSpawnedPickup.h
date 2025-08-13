// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "YafSpawnedMaster.h"
#include "YafSpawnedPickup.generated.h"

UENUM()
enum class EPickupTypes : uint8
{
	PT_Shield		UMETA(DisplayName = "Shield"),
	PT_Food			UMETA(DisplayName = "Food"),
	PT_Boost		UMETA(DisplayName = "Speed Boost")
};

/**
 * 
 */
UCLASS()
class YOUAREFOOD_API AYafSpawnedPickup : public AYafSpawnedMaster
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pick Up Items")
	EPickupTypes PickupType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pick Up Items", meta=(EditCondition="PickupType==EPickupType::PT_Food"))
	float RestoresFoodAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pick Up Items", meta=(EditCondition="PickupType==EPickupType::PT_Boost"))
	float BoostTimeInSeconds;

	virtual void OnMeshOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
};
