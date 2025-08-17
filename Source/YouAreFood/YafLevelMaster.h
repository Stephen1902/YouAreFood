// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "YafSpawnedMaster.h"
#include "YafLevelMaster.generated.h"

class UYafSpawnArrowComponent;
class UBoxComponent;

UENUM(BlueprintType)
enum class EFloorType : uint8
{
	FT_Straight		UMETA(DisplayName = "Straight"),
	FT_Left			UMETA(DisplayName = "Left"),
	FT_Right		UMETA(DisplayName = "Right")
};

UCLASS()
class YOUAREFOOD_API AYafLevelMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AYafLevelMaster();

	bool SpawnPickup();
	bool SpawnEnemy();

	UBoxComponent* GetStartCollision() const { return StartCollision; }
	UArrowComponent* GetArrowComp() const { return ArrowComp; }
	EFloorType GetFloorType() const { return FloorType; }
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UStaticMeshComponent* MeshCompBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UArrowComponent* ArrowComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UBoxComponent* EndCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UBoxComponent* StartCollision;
		
	// A component for spawning an item the player can hit.  Change Yaw to 180 if not to be used.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UYafSpawnArrowComponent* SpawnPointLeft;

	// A component for spawning an item the player can hit.  Change Yaw to 180 if not to be used.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UYafSpawnArrowComponent* SpawnPointCentre;

	// A component for spawning an item the player can hit.  Change Yaw to 180 if not to be used.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UYafSpawnArrowComponent* SpawnPointRight;

	// Whether the floor type is straight, turns the player left or right
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Game Level")
	EFloorType FloorType;

	// Items derived from the Spawned Items Master class that can be spawned by this level piece
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Level")
	TArray<TSubclassOf<class AYafSpawnedMaster>> ItemsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Game Level")
	TSubclassOf<class AYafWalkwaySideActor> SideActorToSpawn;
	
	// Items from the Spawned Enemy class that can be spawned by this level piece
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	TArray<TSubclassOf<class AYafSpawnedEnemy>> EnemiesToSpawn;
	
	// Non-Player vehicles to spawn in the level for the player to hit
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
//	TArray<TSubclassOf<class AEVRVehicleMaster>> NonPlayerVehicles;
	
	// Roadside spawn items
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
//	TArray<TSubclassOf<class AEVRRoadsideSpawns>> SidePiecesToSpawn;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

// Getters
public:	
	FTransform GetNextSpawnPoint() const { return ArrowComp->GetComponentTransform(); };

	void GetSpawnPointInfo(FVector &NewSpawnLocation, FRotator &NewSpawnRotation) const;
	int32 GetNumberOfLanes() const { return NumberOfLanes; }

	void SetHasTurnedPlayer() { bHasTurnedPlayer = true; }
	bool GetHasTurnedPlayer() const { return bHasTurnedPlayer; }
private:
	// Number of lanes in this piece so the player position is set correctly
	int32 NumberOfLanes;

	void SetArrowAndBoxTransforms();
	
	void GetReferences();

	UPROPERTY()
	class AYafWalkwaySideActor* NewEdgePiece;
	
	UPROPERTY()
	class AYafGameStateBase* GameStateRef;

	UPROPERTY()
	class APlayerPawn* PlayerPawnRef;

	UPROPERTY()
	TArray<FTransform> SpawnPointArray;

	UPROPERTY()
	TArray<AYafSpawnedMaster*> SpawnedItems;
	int32 LastSpawnPoint = -1;
	
	bool SpawnRoadsidePiece();
	
	float TurnSpawnChance;
	int32 BlockSpawnChance;
	
	void DestroyThisPiece();

	bool bHasTurnedPlayer = false;

	ESpawnedTypes LastSpawnedType;
};
