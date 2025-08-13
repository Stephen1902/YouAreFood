// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.


#include "MainMenuWidget.h"
#include "YafSaveGame.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetSavedInfo();

	StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
	GameCreditsOkButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGameCreditsButtonClicked);
	RecordsOkButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnRecordsOkButtonClicked);
	AboutButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnAboutButtonClicked);
	ResetHighScoreCheck->OnCheckStateChanged.AddDynamic(this, &UMainMenuWidget::OnResetScoreClicked);
	HelpPageButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHelpPageButtonClicked);
}

void UMainMenuWidget::OnStartGameButtonClicked()
{
	if (!bShowHintPage)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("DefaultLevel"));
	}
	else
	{
		HelpPageOverlay->SetVisibility(ESlateVisibility::Visible);
		MainMenuOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenuWidget::OnAboutButtonClicked()
{
	GameCreditsOverlay->SetVisibility(ESlateVisibility::Visible);
	MainMenuOverlay->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void UMainMenuWidget::OnGameCreditsButtonClicked()
{
	GameCreditsOverlay->SetVisibility(ESlateVisibility::Hidden);
	MainMenuOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::OnRecordsOkButtonClicked()
{
	if (ResetHighScoreCheck->IsChecked())
	{
		if (UGameplayStatics::DoesSaveGameExist("SaveGameSlot", 0))
		{
			USaveGame* SaveGameRef = UGameplayStatics::LoadGameFromSlot("SaveGameSlot", 0);
			// Cast to the specific instance of the SaveGame class for the HighScore variable
			UYafSaveGame* ThisSaveGameRef = Cast<UYafSaveGame>(SaveGameRef);

			ThisSaveGameRef->SavedHighScore = 0;
			HighScoreText->SetText(FText::FromString(FString::FromInt(0)));
			UGameplayStatics::SaveGameToSlot(ThisSaveGameRef, "SaveGameSlot", 0);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to reset the high score but the save game does not exist."));
		}

		ResetHighScoreCheck->SetIsChecked(false);
	}
}

void UMainMenuWidget::OnResetScoreClicked(bool bIsChecked)
{
	RecordsOkButton->SetIsEnabled(bIsChecked);
}

void UMainMenuWidget::OnHelpPageButtonClicked()
{
	if (DoNotShowHelpPage->IsChecked())
	{
		if (UGameplayStatics::DoesSaveGameExist("SaveGameSlot", 0))
		{
			USaveGame* SaveGameRef = UGameplayStatics::LoadGameFromSlot("SaveGameSlot", 0);
			// Cast to the specific instance of the SaveGame class for the HighScore variable
			UYafSaveGame* ThisSaveGameRef = Cast<UYafSaveGame>(SaveGameRef);

			ThisSaveGameRef->bShowHelpScreen = false;
			UGameplayStatics::SaveGameToSlot(ThisSaveGameRef, "SaveGameSlot", 0);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to reset the high score but the save game does not exist."));
		}

		DoNotShowHelpPage->SetIsChecked(false);
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("DefaultLevel"));	
}

void UMainMenuWidget::GetSavedInfo()
{
	USaveGame* SaveGameRef;
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
	UYafSaveGame* ThisSaveGameRef = Cast<UYafSaveGame>(SaveGameRef);
	
	if (ThisSaveGameRef)
	{
		const FText HighScoreToDisplay = FText::FromString(FString::FromInt(ThisSaveGameRef->SavedHighScore));
		HighScoreText->SetText(HighScoreToDisplay);

		bShowHintPage = ThisSaveGameRef->bShowHelpScreen;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get a valid Save Game Reference in PlayerPawn."));
	}
}

