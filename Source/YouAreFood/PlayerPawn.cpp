// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "PlayerPawn.h"

#include "PlayerWidget.h"
#include "yafGameStateBase.h"
#include "YafLevelMaster.h"
#include "YafSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
	
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	SkeletalMeshComp->SetMobility(EComponentMobility::Movable);
	SkeletalMeshComp->SetupAttachment(SceneComponent);
	SkeletalMeshComp->OnComponentBeginOverlap.AddDynamic(this, &APlayerPawn::OnMeshBeginOverlap);

	ShieldMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Mesh Comp"));
	ShieldMeshComp->SetupAttachment(SkeletalMeshComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(SceneComponent);
	SpringArmComp->TargetArmLength = 1000.f;
	SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	SpringArmComp->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	TurnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFloatObject(TEXT("CurveFloat'/Game/Blueprints/Player/FloatCurveNormal.FloatCurveNormal'"));
	if (CurveFloatObject.Succeeded())
	{
		FCurve = CurveFloatObject.Object;

		InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	}

	CurrentPlayerLife = 100.0f;
	LifeDrainPerSecond = 1.0f;
	
	bIsTurning = false;
	bCanTurn = false;
	MovementSpeed = 500.f;
	SpeedOfAcceleration = 5.0f;
	TurnSpeed = 0.25f;
	TurnDegrees = 100.f;
	DesiredRotation = 180.f;
	
	TimeSinceTurnStarted = 0.f;
	LocationIndex = 1;

	bHasShield = false;
	bGameOver = false;
}

void APlayerPawn::SetNewPositionAfterTurn(const FVector ArrowLocation, const FRotator NewRotation)
{
	StartPlayerLocation = GetActorLocation();
	StartPlayerRotation = GetActorRotation();
	EndPlayerRotation = NewRotation;

	if (!bHasTurned)
	{
		EndPlayerLocation = FVector(ArrowLocation.X, StartPlayerLocation.Y, StartPlayerLocation.Z);

		bHasTurned = true;
	}
	else
	{
		EndPlayerLocation = FVector(StartPlayerLocation.X, ArrowLocation.Y, StartPlayerLocation.Z);

		bHasTurned = false;
	}

	if (FCurve)
	{
		TurnTimeline->PlayFromStart();
	}
}

void APlayerPawn::AddToLaneLocations(const float LocationIn)
{
	YLocations.Add(LocationIn);
}

void APlayerPawn::OnMeshBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the other actor hit was a level piece
	AYafLevelMaster* LevelPieceHit = Cast<AYafLevelMaster>(OtherActor);
	
	if (LevelPieceHit)
	{
		// Check if this vehicle has already been turned by this piece and  that the piece hit was not a straight
		if (!LevelPieceHit->GetHasTurnedPlayer() && LevelPieceHit->GetFloorType() != EFloorType::FT_Straight)
		{
			LevelPieceHit->SetHasTurnedPlayer();
			
			// Check that the part which was hit was the Start Collision Box Component
			if (const UBoxComponent* BoxThatWasHit = LevelPieceHit->GetStartCollision())
			{
				float CurrentPlayerYaw = GetActorRotation().Yaw;

				// Check whether the Yaw needs to be reduced or increase
				if (LevelPieceHit->GetFloorType() == EFloorType::FT_Left)
				{
					CurrentPlayerYaw -= 90.f;
				}
				else
				{
					CurrentPlayerYaw += 90.f;
				}

				const FRotator NewVehicleRotation = FRotator(0.f, CurrentPlayerYaw, 0.f);

				// Get the arrow component of the current piece
				const FVector ArrowCompLocation = LevelPieceHit->GetArrowComp()->GetComponentLocation();

				SetNewPositionAfterTurn(ArrowCompLocation, NewVehicleRotation);
			}
		}
	}
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	GetMovementCurve();
	GetReferences();
	AddPlayerWidgetToScreen();
	SetSaveGameReferences();
	ChangeInputMode(false);
		
	CurrentSpeed = 0.f;
}

void APlayerPawn::TimelineFloatReturn(float Value)
{
	CurrentSpeed = MovementSpeed * 0.75f;
	
	SetActorLocation(FMath::Lerp(StartPlayerLocation, EndPlayerLocation, Value));
	SetActorRotation(FMath::Lerp(StartPlayerRotation, EndPlayerRotation, Value));
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bGameOver)
	{
		MovePlayerForward(DeltaTime);
		
		if (bIsTurning)
		{
			Turn(DeltaTime);
		}
	}

	AddToDistanceTravelled(DeltaTime);
	DrainPlayerLife(DeltaTime);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &APlayerPawn::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &APlayerPawn::TurnRight);
	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &APlayerPawn::PauseKeyPressed);
}

void APlayerPawn::SetHasShield(UStaticMesh* ShieldMesh, const FVector ScaleToSet)
{
	// Make sure there isn't already a shield in place
	if (!bHasShield)
	{
		bHasShield = true;
		if (ShieldMesh)
		{
			ShieldMeshComp->SetStaticMesh(ShieldMesh);
			//ShieldMeshComp->SetRelativeScale3D(ScaleToSet);
		}
	}
}

void APlayerPawn::RemoveShield()
{
	bHasShield = false;
	ShieldMeshComp->SetStaticMesh(nullptr);
}

void APlayerPawn::GameOver()
{
	
	if (ThisSaveGameRef && DistanceTravelled > HighScore)
	{
		HighScore = FMath::Floor(DistanceTravelled);
		ThisSaveGameRef->SavedHighScore = HighScore;

		if (UGameplayStatics::SaveGameToSlot(ThisSaveGameRef, "SaveGameSlot", 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Saved"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Game failed to save"));
		}
	}
		
	if (PlayerWidgetRef)
	{
		const FText DistanceToBroadcast =  FText::FromString(FString::SanitizeFloat(FMath::Floor(DistanceTravelled), 0) + "M");
		PlayerWidgetRef->GameOver(DistanceToBroadcast, FText::FromString(FString::FromInt(HighScore)), DistanceTravelled > HighScore);
		ChangeInputMode(true);
	}
	
	bGameOver = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void APlayerPawn::DelayedGameOver()
{
	// Stop the player from moving
	bGameOver = true;
	// Make the player mesh invisible to make it look like they've been eaten
	FTimerHandle RemoveMeshTimer;
	GetWorld()->GetTimerManager().SetTimer(RemoveMeshTimer, this, &APlayerPawn::RemoveMesh, 0.5f, false, 0.5f);
	// An event has occured which requires a delayed showing of the game over screen.  Delay by 1 second.
	FTimerHandle DelayTimer;
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &APlayerPawn::GameOver, 1.0f, false, 1.0f);
}

void APlayerPawn::RemoveMesh()
{
	SkeletalMeshComp->SetVisibility(false);
}

void APlayerPawn::TurnLeft()
{
	if (!bCanTurn && LocationIndex > 0 && !bIsTurning)
	{
		LastLocationIndex = LocationIndex;
		LocationIndex -= 1;
		bIsTurning = true;
	}
}

void APlayerPawn::TurnRight()
{
	if (!bCanTurn && LocationIndex < 2 && !bIsTurning)
	{
		LastLocationIndex = LocationIndex;
		LocationIndex += 1;
		bIsTurning = true;
	}
}

void APlayerPawn::Turn(const float DeltaTime)
{
	FVector NewLocation = SkeletalMeshComp->GetRelativeLocation();
	FRotator NewRotation = SkeletalMeshComp->GetRelativeRotation();
	if (LastLocationIndex > LocationIndex)
	{
		NewLocation.Y = SkeletalMeshComp->GetRelativeLocation().Y - (DeltaTime * (DistanceBetweenLanes / TurnSpeed));
		if (TimeSinceTurnStarted <= TurnSpeed / 2)
		{
			NewRotation.Yaw = SkeletalMeshComp->GetRelativeRotation().Yaw - (DeltaTime * TurnDegrees);
		}
		else
		{
			NewRotation.Yaw = SkeletalMeshComp->GetRelativeRotation().Yaw + (DeltaTime * TurnDegrees);
		}
	}
	else
	{
		NewLocation.Y = SkeletalMeshComp->GetRelativeLocation().Y + (DeltaTime * (DistanceBetweenLanes / TurnSpeed));
		if (TimeSinceTurnStarted <= TurnSpeed / 2)
		{
			NewRotation.Yaw = SkeletalMeshComp->GetRelativeRotation().Yaw + (DeltaTime * TurnDegrees);
		}
		else
		{
			NewRotation.Yaw = SkeletalMeshComp->GetRelativeRotation().Yaw - (DeltaTime * TurnDegrees);
		}
	}

	SkeletalMeshComp->SetRelativeLocation(NewLocation);
	SkeletalMeshComp->SetRelativeRotation(NewRotation);
	
	TimeSinceTurnStarted += DeltaTime;
	if (TimeSinceTurnStarted >= TurnSpeed)
	{
		TimeSinceTurnStarted = 0.f;
		bIsTurning = false;
	}
}

void APlayerPawn::MovePlayerForward(float DeltaTime)
{
	if (CurrentSpeed < MovementSpeed)
	{
		CurrentSpeed += MovementSpeed / (MovementSpeed / SpeedOfAcceleration);
	}

	const FVector MoveDirection = FVector(CurrentSpeed * DeltaTime, 0.f, 0.f);
	AddActorLocalOffset(MoveDirection, true);
}

void APlayerPawn::GetReferences()
{
	GameStateRef = Cast<AYafGameStateBase>(GetWorld()->GetGameState());

	if (GameStateRef)
	{
		DistanceBetweenLanes = GameStateRef->GetDistanceBetweenLanes();
	}
}

void APlayerPawn::GetMovementCurve()
{
	if (FCurve)
	{
		// Update function
		TurnTimeline->AddInterpFloat(FCurve, InterpFunction, FName("Alpha"));

		// Set the timeline settings
		TurnTimeline->SetLooping(false);
		TurnTimeline->SetIgnoreTimeDilation(true);

		float MinCurveTimeRange;
		FCurve->GetTimeRange(MinCurveTimeRange, MaxCurveTimeRange);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Float Curve was not found.  Check the location in code"));
	}
}

void APlayerPawn::AddPlayerWidgetToScreen()
{
	if (PlayerWidget)
	{
		PlayerWidgetRef = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidget);
		PlayerWidgetRef->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget not set for PlayerPawn"));
	}
}

void APlayerPawn::AddToDistanceTravelled(float TimeIn)
{
	// Convert m/s speed to MPH (x10 since speed works out to around 6.7MPH)
	const int32 SpeedAsMPH = (CurrentSpeed / 1000.f ) * 22.369363;

	DistanceTravelled += SpeedAsMPH * TimeIn;

	TimeSinceDistanceUpdated += TimeIn;

	if (TimeSinceDistanceUpdated > 0.08f)
	{
		TimeSinceDistanceUpdated = 0.f;
		
		if (PlayerWidgetRef)
		{
			const FText TextToBroadcast =  FText::FromString(FString::SanitizeFloat(FMath::Floor(DistanceTravelled), 0));
			PlayerWidgetRef->SetCurrentDistanceText(TextToBroadcast);
		}		
	}
	

}

void APlayerPawn::PauseKeyPressed()
{
}

void APlayerPawn::SetSaveGameReferences()
{
	// Check if a saved game already exists and if not, create it
	if (UGameplayStatics::DoesSaveGameExist("SaveGameSlot", 0))
	{
		SaveGameRef = UGameplayStatics::LoadGameFromSlot("SaveGameSlot", 0);
	}
	else
	{
		SaveGameRef = Cast<UYafSaveGame>(UGameplayStatics::CreateSaveGameObject(UYafSaveGame::StaticClass()));
	}

	// Cast to the specific instance of the SaveGame class for the HighScore variable
	ThisSaveGameRef = Cast<UYafSaveGame>(SaveGameRef);
	
	if (ThisSaveGameRef)
	{
		HighScore = ThisSaveGameRef->SavedHighScore;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get a valid Save Game Reference in PlayerPawn."));
	}
}

void APlayerPawn::ChangeInputMode(bool ShouldShowMouse) const
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetShowMouseCursor(ShouldShowMouse);
		if (ShouldShowMouse)
		{
			PC->SetInputMode(FInputModeUIOnly());
		}
		else
		{
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void APlayerPawn::DrainPlayerLife(float TimeIn)
{
	CurrentPlayerLife -= LifeDrainPerSecond * TimeIn;

	if (PlayerWidgetRef)
	{
		PlayerWidgetRef->SetPlayerLife(CurrentPlayerLife / 100.f);
	}

	if (CurrentPlayerLife <= 0.f && !bGameOver)
	{
		GameOver();
	}
}

void APlayerPawn::SpeedMultiplierEnded()
{
	GetWorld()->GetTimerManager().ClearTimer(SpeedMultiplierTimer);
	MovementSpeed = MovementSpeed / 2.0f;
}

void APlayerPawn::AdjustLife(const float LifeAmountIn)
{
	CurrentPlayerLife = FMath::Clamp(CurrentPlayerLife + LifeAmountIn, 0.f, 100.f);

	if (PlayerWidgetRef)
	{
		PlayerWidgetRef->SetPlayerLife(CurrentPlayerLife / 100.f);
	}
}

void APlayerPawn::AdjustSpeed(const float SpeedMultiplier)
{
	// Check if the player already has a speed booster active.  Clear it.
	if (GetWorld()->GetTimerManager().IsTimerActive(SpeedMultiplierTimer))
    {
    	GetWorld()->GetTimerManager().ClearTimer(SpeedMultiplierTimer);
    }
	else
	{
		// Adjust the speed
		MovementSpeed = MovementSpeed * 2.0f;
	}
	
	GetWorld()->GetTimerManager().SetTimer(SpeedMultiplierTimer, this, &APlayerPawn::SpeedMultiplierEnded, SpeedMultiplier, false, SpeedMultiplier);
}

