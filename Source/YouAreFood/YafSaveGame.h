// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "YafSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class YOUAREFOOD_API UYafSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Saved Game Options")
	int32 SavedHighScore;

	UPROPERTY(EditAnywhere, Category = "Saved Game Options")
	bool bShowHelpScreen;
};
