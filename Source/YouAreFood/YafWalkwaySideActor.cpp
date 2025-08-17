// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "YafWalkwaySideActor.h"

#include <solver/PxSolverDefs.h>

#include "yafGameStateBase.h"

// Sets default values
AYafWalkwaySideActor::AYafWalkwaySideActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Display Mesh"));
	MeshComp->SetupAttachment(RootComp);

	SceneryStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scenery Static Mesh"));
	SceneryStaticMesh->SetupAttachment(MeshComp);

	ScenerySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Scenery Skeltal Mesh"));
	ScenerySkeletalMesh->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void AYafWalkwaySideActor::BeginPlay()
{
	Super::BeginPlay();

	GameStateRef = Cast<AYafGameStateBase>(GetWorld()->GetGameState());
	if (GameStateRef)
	{
		UStaticMesh* DisplayMesh;
		USkeletalMesh* DisplaySkeletal;

		GameStateRef->GetSidePieceDecorations(DisplayMesh, DisplaySkeletal);
		SetMeshes(DisplayMesh, DisplaySkeletal);
	}
}

void AYafWalkwaySideActor::SetMeshes(UStaticMesh* MeshToDisplay, USkeletalMesh* SkeletalMeshToDisplay)
{
	if (MeshToDisplay)
	{
		SceneryStaticMesh->SetStaticMesh(MeshToDisplay);
	}

	if (SkeletalMeshToDisplay)
	{
		ScenerySkeletalMesh->SetSkeletalMesh(SkeletalMeshToDisplay);
	}
}

