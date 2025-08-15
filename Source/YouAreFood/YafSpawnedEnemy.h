// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "YafSpawnedMaster.h"
#include "YafSpawnedEnemy.generated.h"

/**
 * 
 */
UCLASS()
class YOUAREFOOD_API AYafSpawnedEnemy : public AYafSpawnedMaster
{
	GENERATED_BODY()
	
	// Set up default settings
	AYafSpawnedEnemy();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	class UBoxComponent* CollisionComp;

	UFUNCTION(BlueprintImplementableEvent)
	void OverlapTriggered();
	
	UFUNCTION()
	void OnCollisionOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
