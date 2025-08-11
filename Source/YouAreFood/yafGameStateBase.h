// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "yafGameStateBase.generated.h"

class AEVRLevelMaster;
/**
 * 
 */
UCLASS()
class YOUAREFOOD_API AYafGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
/*	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Level Pieces")
	TArray<TSubclassOf<AEVRLevelMaster>> StraightPiecesToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Level Pieces")
	TArray<TSubclassOf<AEVRLevelMaster>> TurnPiecesToSpawn;
*/
	// Distance between lanes for vehicles turning between them
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Options")
	float DistanceBetweenLanes;

	bool TryToSpawnNextPiece();

	void SetCurrentPieceYaw(const float NewYaw);

	FRotator GetCurrentRotation() const { return SpawnRotation; }

	void SetYafLevelMasterRef(class AYafLevelMaster* ReferenceIn) { LevelMasterRef = ReferenceIn; }

	float GetDistanceBetweenLanes() const { return DistanceBetweenLanes; }

	void SetLastPieceWasFlat(const bool WasFlatIn ) { bLastSpawnWasFlat = WasFlatIn; }
	bool GetLastPieceWasFlat() const { return bLastSpawnWasFlat; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FTransform SpawnTransform;
	// Spawn Properties
	FVector SpawnLocation;
	FRotator SpawnRotation;
	FActorSpawnParameters SpawnInfo;
	
	void SpawnStartLevel();
	void SpawnNextLevelPiece(const bool SpawnStraight, const int32 PieceToSpawn);
	void TryToSpawnPickup();

	int32 CreateTurn;

	float CurrentPieceYaw;

	int32 SpecialStraightSpawnChance = 0;
	int32 SpawnPickupChance = 0;

	UPROPERTY()
	class AYafLevelMaster* LevelMasterRef;

	bool bLastSpawnWasFlat;
};
