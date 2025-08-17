// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "YafWalkwaySideActor.generated.h"

UCLASS()
class YOUAREFOOD_API AYafWalkwaySideActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AYafWalkwaySideActor();

	void SetActorThatSpawned(class AYafLevelMaster* ActorIn);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	UStaticMeshComponent* SceneryStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	UChildActorComponent* ActorMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Walkway Sides")
	class AYafLevelMaster* ActorThatSpawned;


private:
	UPROPERTY()
	class AYafGameStateBase* GameStateRef;

	
	void SetMeshes(UStaticMesh* MeshToDisplay, TSubclassOf<AActor> ActorToAdd);
	
};
