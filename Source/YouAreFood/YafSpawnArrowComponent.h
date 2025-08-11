// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "YafSpawnArrowComponent.generated.h"

/**
 * 
 */
UCLASS()
class YOUAREFOOD_API UYafSpawnArrowComponent : public UArrowComponent
{
	GENERATED_BODY()

public:
	UYafSpawnArrowComponent();

	bool GetCanSpawnHere() const { return bCanSpawnHere; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spawn Option")
	bool bCanSpawnHere;	
};
