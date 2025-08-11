// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "YafGameStateBase.h"

bool AYafGameStateBase::TryToSpawnNextPiece()
{
	return true;
}

void AYafGameStateBase::SetCurrentPieceYaw(const float NewYaw)
{
}

void AYafGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void AYafGameStateBase::SpawnStartLevel()
{
}

void AYafGameStateBase::SpawnNextLevelPiece(const bool SpawnStraight, const int32 PieceToSpawn)
{
}

void AYafGameStateBase::TryToSpawnPickup()
{
}
