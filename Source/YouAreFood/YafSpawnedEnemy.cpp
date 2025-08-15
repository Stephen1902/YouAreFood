// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "YafSpawnedEnemy.h"
#include "Components/BoxComponent.h"

AYafSpawnedEnemy::AYafSpawnedEnemy()
{
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("Skeletal Mesh Comp");
	SkeletalMeshComp->SetupAttachment(StaticMeshComp);

	CollisionComp = CreateDefaultSubobject<UBoxComponent>("Collision Comp");
	CollisionComp->SetupAttachment(StaticMeshComp);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AYafSpawnedEnemy::OnCollisionOverlap);
}

void AYafSpawnedEnemy::OnCollisionOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerPawnRef && PlayerPawnRef == OtherActor)
	{
		// Trigger the overlap animation in Blueprint
		OverlapTriggered();
		// Turn off collision so the event is only triggered once
		CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		// Check if the player has the shield
		if (PlayerPawnRef->GetHasShield())
		{
			PlayerPawnRef->RemoveShield();
		}
		else
		{
			// Player does not have the shield, and it's game over, allowing time for the animation to play.
			PlayerPawnRef->DelayedGameOver();


		}
	}
}
