// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#include "YafSpawnedPickup.h"

void AYafSpawnedPickup::OnMeshOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Super::OnBeginOverlap(HitComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (PlayerPawnRef && OtherActor == PlayerPawnRef)
	{
		// Check if this pickup is a shield and whether the player already has one
		switch (PickupType)
		{
		case EPickupTypes::PT_Shield:
			{
				PlayerPawnRef->SetHasShield(StaticMeshComp->GetStaticMesh(), StaticMeshComp->GetRelativeScale3D());
				break;
			}
		case EPickupTypes::PT_Food:
			{
				break;	
			}
		case EPickupTypes::PT_Boost:
			{
				break;
			}
		}
	}

	Destroy();
}
