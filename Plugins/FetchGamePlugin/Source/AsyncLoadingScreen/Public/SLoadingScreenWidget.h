#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FDeferredCleanupSlateBrush;

class ASYNCLOADINGSCREEN_API SLoadingScreenWidget : public SCompoundWidget
{
private:
	TSharedPtr<FDeferredCleanupSlateBrush> ImageBrush;

public:
	SLATE_BEGIN_ARGS(SLoadingScreenWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};
