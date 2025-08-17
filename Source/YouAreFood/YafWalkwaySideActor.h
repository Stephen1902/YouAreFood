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

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	UStaticMeshComponent* SceneryStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Walkway Sides")
	USkeletalMeshComponent* ScenerySkeletalMesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	class AYafGameStateBase* GameStateRef;
	
	void SetMeshes(UStaticMesh* MeshToDisplay, USkeletalMesh* SkeletalMeshToDisplay);
	
};
