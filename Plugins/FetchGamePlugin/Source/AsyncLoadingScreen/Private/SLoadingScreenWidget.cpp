#include "SLoadingScreenWidget.h"

#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Slate/DeferredCleanupSlateBrush.h"

#include "SLoadingAnimatedWidget.h"
#include "AsyncLoadingScreenSettings.h"

void SLoadingScreenWidget::Construct(const FArguments& InArgs)
{
	const UAsyncLoadingScreenSettings* const Settings = GetDefault<UAsyncLoadingScreenSettings>();
	const FSoftObjectPath& ImageAsset = Settings->TexturePath;
	UTexture2D* const LoadingImage = Cast<UTexture2D>(ImageAsset.TryLoad());

	FVector2D ViewportSize = FVector2D::UnitVector;
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportDimensions;
		GEngine->GameViewport->GetViewportSize(ViewportDimensions);
		ViewportSize = ViewportDimensions;
	}
	const float ViewportDiagonal = FMath::Sqrt(FMath::Square(ViewportSize.X) + FMath::Square(ViewportSize.Y));

	if (LoadingImage)
	{
		ImageBrush = FDeferredCleanupSlateBrush::CreateBrush(LoadingImage);
		ChildSlot
			[
				SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SScaleBox)
							.Stretch(EStretch::Fill)
							[
								SNew(SImage)
									.Image(ImageBrush.IsValid() ? ImageBrush->GetSlateBrush() : nullptr)
							]
					]
					+ SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SLoadingAnimatedWidget)
							.DesiredDiagonal(ViewportDiagonal * 0.1f)
					]
			];
	}
}