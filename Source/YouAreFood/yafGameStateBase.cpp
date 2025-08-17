// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "YafGameStateBase.h"
#include "YafLevelMaster.h"

AYafGameStateBase::AYafGameStateBase()
{
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CreateTurn = 0;

	CurrentPieceYaw = 0;
	LevelMasterRef = nullptr;
	bLastSpawnWasFlat = false;
	DistanceBetweenLanes = 280.f;
	SidePieceDecoration = -1;
}

void AYafGameStateBase::TryToSpawnNextPiece()
{
	CreateTurn += 1;
	bool CreatedTurn = false;

	// If more than 12 straight pieces have been spawned, chance of a turn occuring, if there are turn pieces in the array
	if (TurnPiecesToSpawn.Num() > 0 && CreateTurn > 12)
	{
		// Always turn if 18 straights in a row
		if (FMath::RandRange(0, 99) > 49 || CreateTurn > 18)
		{
			// Reset the chance of a turn piece being spawned
			CreateTurn = 0;
			// Pick a random turn piece from the array and spawn it 
			const int32 RandomSpawn = FMath::RandRange(0, TurnPiecesToSpawn.Num() - 1);
			SpawnNextLevelPiece(false, RandomSpawn, false);
			// Mark that a turn was created.  Don't create a straight.
			CreatedTurn = true;
		}
	}

	// A turn wasn't created, create a straight
	if (!CreatedTurn)
	{
		// Increase the chance of spawning an enemy by 10 to increase the chance of one being spawned this time.
		SpawnEnemyChance += 10;
		
		// Don't allow for the possibility of 2 non-special straight pieces to spawn consecutively and check there are special straights in the array
		if (StraightPiecesToSpawn.Num() > 1 && FMath::RandRange(25, 124) < SpecialStraightSpawnChance)
		{
			// Reset the possibility of a special straight being created next time
			SpecialStraightSpawnChance = 0;
			// Pick a random special piece from the array and spawn it
			const int32 RandomSpawn = FMath::RandRange(1, StraightPiecesToSpawn.Num() - 1);
			SpawnNextLevelPiece(true, RandomSpawn, false);
		}
		else
		{
			// Spawn a standard straight
			SpawnNextLevelPiece(true, 0, true);
			
			// Increase the chance of it being a special straight next time
			SpecialStraightSpawnChance += 25;

			// As it's a straight piece, try to spawn a pickup on it
            //TryToSpawnPickup();
		}


	}
}

void AYafGameStateBase::SetCurrentPieceYaw(const float NewYaw)
{
	CurrentPieceYaw = NewYaw;
}

void AYafGameStateBase::GetSidePieceDecorations(UStaticMesh*& MeshToDisplay, USkeletalMesh*& SkeletalMeshToDisplay)
{
	MeshToDisplay = nullptr;
 	SkeletalMeshToDisplay = nullptr;
	
	if (SidePieceDecoration > -1)
	{		
		if (EdgeSkeletalMeshes.Num() > 0)
		{
			MeshToDisplay = EdgeStaticMeshes[SidePieceDecoration];
		}

		if (EdgeSkeletalMeshes.Num() > 0)
		{
			SkeletalMeshToDisplay = EdgeSkeletalMeshes[SidePieceDecoration];
		}
	}
}

void AYafGameStateBase::UpdateSidePieceDecoration()
{
	int32 NewDecoration = 0;
	
	if (EdgeStaticMeshes.Num() > 0)
	{
		do
		{
			NewDecoration = FMath::RandRange(0, EdgeStaticMeshes.Num());
		}
		while (NewDecoration == SidePieceDecoration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Update called but Array is empty"));
	}

	// There should be a chance there is no side decoration so minus 1 off of the list to allow for this 
	SidePieceDecoration = NewDecoration - 1;
}

void AYafGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnStartLevel();
}

void AYafGameStateBase::SpawnStartLevel()
{
	if (StraightPiecesToSpawn.Num() > 0)
	{
		// The first 5 pieces will have no pickups on, to give the player chance to get ready
		for (int32 i = 0; i < 5; ++i)
		{
			SpawnNextLevelPiece(true, 0, false);
		}

		for (int32 i = 0; i < 5; ++i)
		{
			SpawnNextLevelPiece(true, 0, true);
		}
		
	}
}

void AYafGameStateBase::TryToSpawnPickup()
{
	/*if (LevelMasterRef)
 	{
		if (FMath::RandRange(0, 75) < SpawnPickupChance && LevelMasterRef->SpawnPickup())
 		{
 			SpawnPickupChance = 0;
 		}
	}*/
}

void AYafGameStateBase::SpawnNextLevelPiece(const bool SpawnStraight, const int32 PieceToSpawn, const bool SpawnPickup)
{
	AYafLevelMaster* NewLevelPiece;
	
	if (SpawnStraight)
	{
		NewLevelPiece = GetWorld()->SpawnActor<AYafLevelMaster>(StraightPiecesToSpawn[PieceToSpawn], SpawnTransform, SpawnInfo);
	}
	else
	{
		NewLevelPiece = GetWorld()->SpawnActor<AYafLevelMaster>(TurnPiecesToSpawn[PieceToSpawn], SpawnTransform, SpawnInfo);
	}
		
	SpawnTransform = NewLevelPiece->GetNextSpawnPoint();
	NewLevelPiece->GetSpawnPointInfo(SpawnLocation, SpawnRotation);

	// Don't spawn any random parts when the game starts to give the player time to get ready
	if (SpawnPickup)
	{
		if (FMath::RandRange(40, 90) > SpawnEnemyChance)
		{
			NewLevelPiece->SpawnPickup();
		}
		else
		{
			SpawnEnemyChance = 0;
			NewLevelPiece->SpawnEnemy();
		}
	}
}
