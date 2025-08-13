// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "YafLevelMaster.h"

#include "PlayerPawn.h"
#include "yafGameStateBase.h"
#include "YafSpawnArrowComponent.h"
#include "YafSpawnedEnemy.h"
#include "Components/BoxComponent.h"

// Sets default values
AYafLevelMaster::AYafLevelMaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComp);

	MeshCompBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor Mesh"));
	MeshCompBase->SetupAttachment(RootComponent);
	MeshCompBase->SetRelativeLocation(FVector(500.f, 0.f, 0.f));

	StartCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Start Collision"));
	StartCollision->SetupAttachment(RootComponent);
	StartCollision->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	//StartCollision->OnComponentBeginOverlap.AddDynamic(this, &AEVRLevelMaster::OnBeginOverlapStartComp);
	
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Point"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

	EndCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("End Collision"));
	EndCollision->SetupAttachment(ArrowComp);
	EndCollision->SetRelativeLocation(FVector(0.f, 0.f,120.f));
	EndCollision->SetBoxExtent(FVector(16.f, 500.f, 100.f));
	EndCollision->OnComponentBeginOverlap.AddDynamic(this, &AYafLevelMaster::OnBeginOverlap);

	SpawnPointLeft = CreateDefaultSubobject<UYafSpawnArrowComponent>(TEXT("Spawn Point Left"));
	SpawnPointLeft->SetupAttachment(RootComponent);
	SpawnPointLeft->SetRelativeLocation(FVector(100.f, -333.f, 30.f));

	SpawnPointCentre = CreateDefaultSubobject<UYafSpawnArrowComponent>(TEXT("Spawn Point Centre"));
	SpawnPointCentre->SetupAttachment(RootComponent);
	SpawnPointCentre->SetRelativeLocation(FVector(100.f, 0.f, 30.f));

	SpawnPointRight = CreateDefaultSubobject<UYafSpawnArrowComponent>(TEXT("Spawn Point Right"));
	SpawnPointRight->SetupAttachment(RootComponent);
	SpawnPointRight->SetRelativeLocation(FVector(100.f, 333.f, 30.f));

	OnRoadActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Spawned Pickup Actor"));
	OnRoadActor->SetupAttachment(RootComponent);

	NewRoadsideActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Spawned Roadside Actor"));
	NewRoadsideActor->SetupAttachment(RootComponent);
	
	TurnSpawnChance = 0.f;
	BlockSpawnChance = 50;

	LastSpawnedType = ESpawnedTypes::ST_Enemy;
}

// Called when the game starts or when spawned
void AYafLevelMaster::BeginPlay()
{
	Super::BeginPlay();

	GetReferences();
	SetArrowAndBoxTransforms();
	SpawnRoadsidePiece();
}

bool AYafLevelMaster::SpawnPickup()
{
	// Check for valid spawn points and valid items in this level piece's array
	if (SpawnPointArray.Num() > 0 && ItemsToSpawn.Num() > 0)
	{
		// Always spawn at least 1 item
		const int32 RandomItemToSpawn = FMath::RandRange(0, ItemsToSpawn.Num() - 1);
		const int32 RandomTransformToSpawnAt = FMath::RandRange(0,2);
		
		const FActorSpawnParameters SpawnInfo;
		
		FVector WorldLocationToSpawn = SpawnPointArray[RandomTransformToSpawnAt].GetLocation();
		WorldLocationToSpawn.Z += 20.f;
		SpawnedItems.Add(GetWorld()->SpawnActor<AYafSpawnedMaster>(ItemsToSpawn[RandomItemToSpawn], WorldLocationToSpawn, ArrowComp->GetComponentRotation(), SpawnInfo));

		// Add second item if required
		if (FMath::RandBool())
		{
			const int32 SecondItemToSpawn = FMath::RandRange(0, ItemsToSpawn.Num() - 1);

			int32 SecondSpawnPoint;
			do 
			{
				SecondSpawnPoint = FMath::RandRange(0,2);
			}
			while (RandomTransformToSpawnAt == SecondSpawnPoint);
			
			WorldLocationToSpawn = SpawnPointArray[SecondSpawnPoint].GetLocation();
			WorldLocationToSpawn.Z += 20.f;
			SpawnedItems.Add(GetWorld()->SpawnActor<AYafSpawnedMaster>(ItemsToSpawn[SecondItemToSpawn], WorldLocationToSpawn, ArrowComp->GetComponentRotation(), SpawnInfo));
			
		}
		
		if (SpawnedItems.Num() > 0)
		{
			return true;
		}
	}

	return false;
}

bool AYafLevelMaster::SpawnEnemy()
{
	if (SpawnPointArray.Num() > 0 && EnemiesToSpawn.Num() > 0)
	{
		const int32 RandomItemToSpawn = FMath::RandRange(0, EnemiesToSpawn.Num() - 1);
		const int32 RandomTransformToSpawnAt = FMath::RandRange(0,2);

		const FActorSpawnParameters SpawnInfo;
		
		FVector WorldLocationToSpawn = SpawnPointArray[RandomTransformToSpawnAt].GetLocation();
		WorldLocationToSpawn.Z += 20.f;
		FRotator WorldRotationToSpawn = ArrowComp->GetComponentRotation();

		// If the enemy has spawned in the middle, randomly select which way they face.  If they spawn on the right, always turn them around
		if ((RandomTransformToSpawnAt == 1 && FMath::RandBool()) || RandomTransformToSpawnAt == 2)
		{
				WorldRotationToSpawn.Yaw += 180.f;
		}
		
		SpawnedItems.Add(GetWorld()->SpawnActor<AYafSpawnedMaster>(EnemiesToSpawn[RandomItemToSpawn], WorldLocationToSpawn, WorldRotationToSpawn, SpawnInfo));
	}
		
	if (SpawnedItems.Num() > 0)
	{
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Enemy failed to spawn."))
	return false;
}

void AYafLevelMaster::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerPawnRef)
	{
		if (GameStateRef)
		{
			GameStateRef->TryToSpawnNextPiece();
		}

		FTimerHandle DestroyTimer;
		GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &AYafLevelMaster::DestroyThisPiece, 2.0f, false, -1.f);
	}
}

void AYafLevelMaster::GetSpawnPointInfo(FVector& NewSpawnLocation, FRotator& NewSpawnRotation) const
{
	NewSpawnLocation = ArrowComp->GetComponentLocation();
	NewSpawnRotation = ArrowComp->GetComponentRotation();
}

void AYafLevelMaster::SetArrowAndBoxTransforms()
{
	// Only spawn items that can be collided with on the straight pieces
	if (SpawnPointLeft->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointLeft->GetComponentTransform());
	}
	
	if (SpawnPointCentre->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointCentre->GetComponentTransform());
	}

	if (SpawnPointRight->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointRight->GetComponentTransform());
	}

	switch (FloorType)
	{
	case EFloorType::FT_Straight:
	default:
		StartCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StartCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		StartCollision->SetGenerateOverlapEvents(false);
		break;
	case EFloorType::FT_Left:
		ArrowComp->SetRelativeLocation(FVector(500.f, -500.f, 0.f));
		ArrowComp->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));
		break;
	case EFloorType::FT_Right:
		ArrowComp->SetRelativeLocation(FVector(500.f, 500.f, 0.f));
		ArrowComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		break;
	}
}

void AYafLevelMaster::GetReferences()
{
	GameStateRef = Cast<AYafGameStateBase>(GetWorld()->GetGameState());

	if (!GameStateRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game State Ref failed to set in YafLevelMaster"));
	}
	else
	{
		GameStateRef->SetLevelMasterRef(this);
	}
	
	PlayerPawnRef = Cast<APlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (!PlayerPawnRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Vehicle Ref failed to set in EVRLevelMaster"));
	}
}

bool AYafLevelMaster::SpawnRoadsidePiece()
{
	return true;
}

void AYafLevelMaster::DestroyThisPiece()
{
	if (SpawnedItems.Num() > 0)
	{
		for (int32 i = 0; i < SpawnedItems.Num(); ++i)
		{
			// Check if the item still exists in the world, destroy it if it does
			if (SpawnedItems[i])
			{
				SpawnedItems[i]->Destroy();
			}
		}
	}
		
	Destroy();
}
