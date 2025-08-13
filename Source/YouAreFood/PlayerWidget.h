// Copyright 2025 DME Games.  Made for the Ryan Laley 2025 Game Jam.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class UOverlay;
class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class YOUAREFOOD_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCurrentDistanceText(const FText NewDistance) const;
	void GameOver(const FText TotalDistance, const FText RecordDistance, bool IsNewRecord) const;
	
protected:
	// Sets default values
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UTextBlock* CurrentDistanceText;

	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UOverlay* GameOverOverlay;

	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UTextBlock* TotalDistanceText;

	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UTextBlock* RecordDistanceText;

	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UTextBlock* NewRecordText;

	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UButton* RetryButton;

	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UButton* QuitToMenuButton;

	UPROPERTY(BlueprintReadOnly, Category = "Player Widget", meta=(BindWidget))
	UButton* QuitToDesktopButton;

private:
	UFUNCTION()
	void OnRetryButtonClicked();

	UFUNCTION()
	void OnQuitToMenuButtonClicked();

	UFUNCTION()
	void OnQuitToDesktopButtonClicked();
};
