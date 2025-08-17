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

	ActorMeshComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scenery Skeltal Mesh"));
	ActorMeshComp->SetupAttachment(MeshComp);
}

void AYafWalkwaySideActor::SetActorThatSpawned(AYafLevelMaster* ActorIn)
{
	if (ActorIn)
	{
		ActorThatSpawned = ActorIn;
	}
}

// Called when the game starts or when spawned
void AYafWalkwaySideActor::BeginPlay()
{
	Super::BeginPlay();

	GameStateRef = Cast<AYafGameStateBase>(GetWorld()->GetGameState());
	if (GameStateRef)
	{
		UStaticMesh* DisplayMesh;
		TSubclassOf<AActor> ActorToGet;

		GameStateRef->GetSidePieceDecorations(DisplayMesh, ActorToGet);
		SetMeshes(DisplayMesh, ActorToGet);
	}
}

void AYafWalkwaySideActor::SetMeshes(UStaticMesh* MeshToDisplay, TSubclassOf<AActor> ActorToAdd)
{
	if (MeshToDisplay)
	{
		SceneryStaticMesh->SetStaticMesh(MeshToDisplay);
	}

	if (ActorToAdd)
	{
		ActorMeshComp->SetChildActorClass(ActorToAdd);
	}
}

