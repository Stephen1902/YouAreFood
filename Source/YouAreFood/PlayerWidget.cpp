// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#include "PlayerWidget.h"

#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RetryButton->OnClicked.AddDynamic(this, &UPlayerWidget::OnRetryButtonClicked);
	QuitToMenuButton->OnClicked.AddDynamic(this, &UPlayerWidget::OnQuitToMenuButtonClicked);
	QuitToDesktopButton->OnClicked.AddDynamic(this, &UPlayerWidget::OnQuitToDesktopButtonClicked);
}


void UPlayerWidget::SetCurrentDistanceText(const FText NewDistance) const
{
	CurrentDistanceText->SetText(NewDistance);
}

void UPlayerWidget::GameOver(const FText TotalDistance, const FText RecordDistance, const bool IsNewRecord) const
{
	// Hide the running total text 
	CurrentDistanceText->SetVisibility(ESlateVisibility::Hidden);

	// Make the overlay containing all the game over details visible and update them
	GameOverOverlay->SetVisibility(ESlateVisibility::Visible);
	TotalDistanceText->SetText(TotalDistance);
	RecordDistanceText->SetText(RecordDistance);
	if (IsNewRecord)
	{
		NewRecordText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlayerWidget::SetPlayerLife(const float LifeAsPercent)
{
	PlayerLifeBar->SetPercent(LifeAsPercent);
}

void UPlayerWidget::OnRetryButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("DefaultLevel"));	
}

void UPlayerWidget::OnQuitToMenuButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"));
}

void UPlayerWidget::OnQuitToDesktopButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
