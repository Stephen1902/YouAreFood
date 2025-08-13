// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UOverlay;
class UCheckBox;
class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class YOUAREFOOD_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UOverlay* MainMenuOverlay;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UButton* StartGameButton;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UButton* AboutButton;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UButton* QuitButton;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UOverlay* GameCreditsOverlay;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UButton* GameCreditsOkButton;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UTextBlock* HighScoreText;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UCheckBox* ResetHighScoreCheck;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UButton* RecordsOkButton;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UOverlay* HelpPageOverlay;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UCheckBox* DoNotShowHelpPage;

	UPROPERTY(BlueprintReadOnly, Category = "Main Menu Widget", meta=(BindWidget))
	UButton* HelpPageButton;

private:
	UFUNCTION()
	void OnStartGameButtonClicked();

	UFUNCTION()
	void OnAboutButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnGameCreditsButtonClicked();

	UFUNCTION()
	void OnRecordsOkButtonClicked();

	UFUNCTION()
	void OnResetScoreClicked(bool bIsChecked);

	UFUNCTION()
	void OnHelpPageButtonClicked();

	void GetSavedInfo();

	bool bShowHintPage;
};
