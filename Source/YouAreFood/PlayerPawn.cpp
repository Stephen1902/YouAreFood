// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "PlayerPawn.h"

#include "yafGameStateBase.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
}

void APlayerPawn::SetNewPositionAfterTurn(const FVector ArrowLocation, const FRotator NewRotation)
{
}

void APlayerPawn::AddToLaneLocations(const float LocationIn)
{
}

void APlayerPawn::OnMeshBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	GetMovementCurve();
	GetReferences();
	
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

	
	MovePlayerForward(DeltaTime);
		
	if (bIsTurning)
	{
		Turn(DeltaTime);
	}
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &APlayerPawn::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &APlayerPawn::TurnRight);
	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &APlayerPawn::PauseKeyPressed);
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

void APlayerPawn::AddToDistanceTravelled(float TimeIn)
{
}

void APlayerPawn::PauseKeyPressed()
{
}

void APlayerPawn::SetSaveGameReferences()
{
}

void APlayerPawn::ShowMouseAndLockDisplay() const
{
}

