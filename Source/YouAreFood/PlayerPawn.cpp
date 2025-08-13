// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "PlayerPawn.h"

#include "PlayerWidget.h"
#include "yafGameStateBase.h"
#include "YafLevelMaster.h"
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
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	StaticMeshComp->SetMobility(EComponentMobility::Movable);
	StaticMeshComp->SetupAttachment(SceneComponent);
	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &APlayerPawn::OnMeshBeginOverlap);

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

	bIsTurning = false;
	bCanTurn = false;
	MovementSpeed = 500.f;
	SpeedOfAcceleration = 5.0f;
	TurnSpeed = 0.25f;
	TurnDegrees = 100.f;
	DesiredRotation = 180.f;
	
	TimeSinceTurnStarted = 0.f;
	LocationIndex = 1;

	bHasShield = true;
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
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &APlayerPawn::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &APlayerPawn::TurnRight);
	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &APlayerPawn::PauseKeyPressed);
}


void APlayerPawn::RemoveShield()
{
	bHasShield = false;
}

void APlayerPawn::GameOver()
{
	if (PlayerWidgetRef)
	{
		PlayerWidgetRef->GameOver(FText::FromString("9998"), FText::FromString("9999"), false);
		ChangeInputMode(true);
	}
	
	bGameOver = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
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
	FVector NewLocation = StaticMeshComp->GetRelativeLocation();
	FRotator NewRotation = StaticMeshComp->GetRelativeRotation();
	if (LastLocationIndex > LocationIndex)
	{
		NewLocation.Y = StaticMeshComp->GetRelativeLocation().Y - (DeltaTime * (DistanceBetweenLanes / TurnSpeed));
		if (TimeSinceTurnStarted <= TurnSpeed / 2)
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw - (DeltaTime * TurnDegrees);
		}
		else
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw + (DeltaTime * TurnDegrees);
		}
	}
	else
	{
		NewLocation.Y = StaticMeshComp->GetRelativeLocation().Y + (DeltaTime * (DistanceBetweenLanes / TurnSpeed));
		if (TimeSinceTurnStarted <= TurnSpeed / 2)
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw + (DeltaTime * TurnDegrees);
		}
		else
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw - (DeltaTime * TurnDegrees);
		}
	}

	StaticMeshComp->SetRelativeLocation(NewLocation);
	StaticMeshComp->SetRelativeRotation(NewRotation);
	
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
	int32 RandomNumber = FMath::RandRange(0, 9999);
	if (PlayerWidgetRef)
	{
		PlayerWidgetRef->SetCurrentDistanceText(FText::FromString(FString::FromInt(RandomNumber)));
	}
}

void APlayerPawn::PauseKeyPressed()
{
}

void APlayerPawn::SetSaveGameReferences()
{
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

