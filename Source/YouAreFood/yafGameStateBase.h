// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "yafGameStateBase.generated.h"

class AYafLevelMaster;
/**
 * 
 */
UCLASS()
class YOUAREFOOD_API AYafGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	AYafGameStateBase();
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Level Pieces")
	TArray<TSubclassOf<AYafLevelMaster>> StraightPiecesToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Level Pieces")
	TArray<TSubclassOf<AYafLevelMaster>> TurnPiecesToSpawn;

	// Distance between lanes for vehicles turning between them
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Options")
	float DistanceBetweenLanes;

	void SetLevelMasterRef(AYafLevelMaster* ReferenceIn) { LevelMasterRef = ReferenceIn; }
	
	void TryToSpawnNextPiece();

	void SetCurrentPieceYaw(const float NewYaw);

	FRotator GetCurrentRotation() const { return SpawnRotation; }

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
	void SpawnNextLevelPiece(const bool SpawnStraight, const int32 PieceToSpawn, const bool SpawnPickup);
	void TryToSpawnPickup();

	int32 CreateTurn;

	float CurrentPieceYaw;

	int32 SpecialStraightSpawnChance = 0;
	int32 SpawnPickupChance = 0;

	UPROPERTY()
	AYafLevelMaster* LevelMasterRef;
	
	bool bLastSpawnWasFlat;
};
