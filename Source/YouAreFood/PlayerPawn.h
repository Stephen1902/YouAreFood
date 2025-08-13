// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDistanceUpdated, FText, DistanceUpdatedText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseKeyPressed);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameOver, FText, DistanceThisGame, FText, PlayerHighScore, bool, NewHighScore);

UCLASS()
class YOUAREFOOD_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

/*	UPROPERTY(BlueprintAssignable, Category = "Player Gameplay")
	FOnDistanceUpdated OnDistanceUpdated;
*/
	UPROPERTY(BlueprintAssignable, Category = "Player Gameplay")
	FOnPauseKeyPressed OnPauseKeyPressed;
/*
	UPROPERTY(BlueprintAssignable, Category = "Player Gameplay")
	FOnGameOver OnGameOver;
*/	
protected:
	UPROPERTY(Category = "Player Pawn", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;
	
	UPROPERTY(Category = "Player Pawn", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(Category = "Player Pawn", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShieldMeshComp;
	
	UPROPERTY(Category = "Player Pawn", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(Category = "Player Pawn", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;

	UPROPERTY(Category = "Player Pawn", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class UPlayerWidget> PlayerWidget;

	// Units in cm the player will move forward per second
	UPROPERTY(EditDefaultsOnly, Category = "Movement Options")
	float MovementSpeed;

	// Time in seconds it takes for the player to reach top speed
	UPROPERTY(EditDefaultsOnly, Category = "Movement Options")
	float SpeedOfAcceleration;
	
	// Time in seconds it takes to move the player to another lane 
	UPROPERTY(EditDefaultsOnly, Category = "Movement Options")
	float TurnSpeed;

	// Amount of visible "turn" in the player when changing lanes
	UPROPERTY(EditDefaultsOnly, Category = "Movement Options")
	float TurnDegrees;
	
	UPROPERTY(Category = "Player Pawn", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	float LifeDrainPerSecond;
	
	UPROPERTY()
	UTimelineComponent* TurnTimeline;

	UFUNCTION(BlueprintCallable, Category = " Gameplay")
	float GetMaxSpeed() const { return MovementSpeed; }

	UFUNCTION(BlueprintCallable, Category = " Gameplay")
	void SetMaxSpeed(const float SpeedIn) { MovementSpeed += SpeedIn; }

	void SetDesiredRotation(const float NewRotation) { DesiredRotation = NewRotation; }

	void SetNewPositionAfterTurn(const FVector ArrowLocation, const FRotator NewRotation);
	
	void AddToLaneLocations(const float LocationIn);
	
	UFUNCTION()
	virtual void OnMeshBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void TimelineFloatReturn(float Value);

	/** Declare a delegate to call with TimeLineFloatReturn */
	FOnTimelineFloat InterpFunction{};

	// Current speed of the player when moving
	float CurrentSpeed;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool GetHasShield() const { return bHasShield; }
	void SetHasShield(UStaticMesh* ShieldMesh, FVector ScaleToSet);
	void RemoveShield();
	void GameOver();
	void AdjustLife(const float LifeAmountIn);
	void AdjustSpeed(const float SpeedMultiplier);
private:
	void TurnLeft();
	void TurnRight();
	virtual void Turn(const float DeltaTime);

	bool bGameIsPaused;
	bool bIsTurning;

	TArray<float> YLocations;
	float DesiredRotation;
	bool bCanTurn;
	bool bHasTurned;
	
	int32 LocationIndex;
	int32 LastLocationIndex;
	float YLocationToMoveTo;
	float TimeSinceTurnStarted;

	// Variables for turning player on a curve
	FVector StartPlayerLocation;
	FVector EndPlayerLocation;
	FRotator StartPlayerRotation;
	FRotator EndPlayerRotation;
		
	void MovePlayerForward(float DeltaTime);
	
	UPROPERTY()
	UTimelineComponent* TimelineComponent;

	/** Curve to use when turning the  round a corner */
	UPROPERTY()
	UCurveFloat* FCurve;

	UPROPERTY()
	class AYafGameStateBase* GameStateRef;

	void GetReferences();

	void GetMovementCurve();
	
	// Time taken for curve to complete in seconds
	float MaxCurveTimeRange;

	// Distance between lanes for the s to turn in to
	float DistanceBetweenLanes;

	float CurrentPlayerLife;
	
	// To quickly flash the player speed, making it look like it's varying slightly
	float TimeSinceSpeedoVaried;

	void AddPlayerWidgetToScreen();
	
	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;
	
	bool bTurnSoundIsPlaying;
	bool bCornerSoundIsPlaying;

	//void PlayRevvingSound() const;
	void AddToDistanceTravelled(float TimeIn);
	
	float DistanceTravelled;
	float TimeSinceDistanceUpdated;
	void PauseKeyPressed();

	int32 HighScore;

	UPROPERTY()
	class USaveGame* SaveGameRef;
	
	UPROPERTY()
	class UYafSaveGame* ThisSaveGameRef;
	
	FString SaveGameSlot;
	void SetSaveGameReferences();

	void ChangeInputMode(bool ShouldShowMouse) const;

	UPROPERTY()
	UPlayerWidget* PlayerWidgetRef;

	bool bHasShield;
	bool bGameOver;

	void DrainPlayerLife(float TimeIn);

	FTimerHandle SpeedMultiplierTimer;
	UFUNCTION()
	void SpeedMultiplierEnded();
};
