// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "MainMenuGameMode.h"

#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidget)
	{
		UMainMenuWidget* MenuWidgetRef = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidget);
		MenuWidgetRef->AddToViewport();

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());
		}
	}
}
