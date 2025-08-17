// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerPawn.h"
#include "YafSpawnedMaster.generated.h"

UENUM()
enum class ESpawnedTypes : uint8
{
	ST_Static		UMETA(DisplayName="Static"),
	ST_Pickup		UMETA(DisplayName="Pickup"),
	ST_Enemy		UMETA(DisplayName="Enemy")
};

UCLASS()
class YOUAREFOOD_API AYafSpawnedMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AYafSpawnedMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	UStaticMeshComponent* StaticMeshComp;

	// Particle to be used when this actor is hit by the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	UParticleSystem* EndOfLifeParticle;

	// Sound to be played when this actor is hit by the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawned Items")
	USoundBase* EndOfLifeSound; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	ESpawnedTypes SpawnedActorType;
	
	UPROPERTY()
    APlayerPawn* PlayerPawnRef;

	UFUNCTION()
	virtual void OnMeshOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY()
	class AYafGameStateBase* GameStateRef;

	void SetReferences();
	void DestroySpawnedActor();
	
};
