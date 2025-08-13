// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "YafSpawnedMaster.h"
#include "yafGameStateBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AYafSpawnedMaster::AYafSpawnedMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	StaticMeshComp->SetupAttachment(SceneComp);
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	StaticMeshComp->SetGenerateOverlapEvents(true);

	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AYafSpawnedMaster::OnMeshOverlap);
	
	SpawnedActorType = ESpawnedTypes::ST_Static;
}

// Called when the game starts or when spawned
void AYafSpawnedMaster::BeginPlay()
{
	Super::BeginPlay();

	SetReferences();
}

void AYafSpawnedMaster::SetReferences()
{
	GameStateRef = Cast<AYafGameStateBase>(GetWorld()->GetGameState());
	PlayerPawnRef = Cast<APlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AYafSpawnedMaster::DestroySpawnedActor()
{
	if (EndOfLifeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndOfLifeParticle, GetActorLocation(), GetActorRotation());
	}

	if (EndOfLifeSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EndOfLifeSound);	
	}
		
	Destroy();
}

void AYafSpawnedMaster::OnMeshOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check what was hit is the player
	if (PlayerPawnRef && OtherActor == PlayerPawnRef)
	{
		// Check what type of actor the player has his the mesh of
		if (SpawnedActorType == ESpawnedTypes::ST_Static)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Static"));
			// Turn off all collisions so that overlaps only get triggered once
			StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);

			// The player has hit something they can't pick up.  Check if they have a shield.
			if (PlayerPawnRef->GetHasShield())
			{
				// They do.  Remove it.
				PlayerPawnRef->RemoveShield();
				// Remove the item that has been hit.
				DestroySpawnedActor();
			}
			else
			{
				// Tell the player it's game over
				PlayerPawnRef->GameOver();
			}
		}

		if (SpawnedActorType == ESpawnedTypes::ST_Enemy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Enemy"));
			// The player has hit the mesh of an enemy much larger than them.  It's game over.
			//PlayerPawnRef->GameOver();
		}
	}
}

