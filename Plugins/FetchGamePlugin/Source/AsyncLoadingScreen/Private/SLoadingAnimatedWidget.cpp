#include "SLoadingAnimatedWidget.h"
#include "Rendering/DrawElements.h"

void SLoadingAnimatedWidget::Construct(const FArguments& InArgs)
{
	MaxScale = InArgs._MaxScale;
	MinScale = InArgs._MinScale;
	ScaleSpeed = InArgs._ScaleSpeed;
	SpinSpeed = InArgs._SpinSpeed;
	EaseFactor = InArgs._EaseFactor;
	DesiredDiagonal = InArgs._DesiredDiagonal;

	MaxDiagonal = DesiredDiagonal * MaxScale * FMath::Sqrt(2.f);
}

void SLoadingAnimatedWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	RotationAngle += SpinSpeed * InDeltaTime;

	const float ScaleChange = (bIsIncreasing ? ScaleSpeed : -ScaleSpeed) * InDeltaTime;
	CurrentScale += ScaleChange * EaseFactor * (1 - EaseFactor * (CurrentScale / MaxScale));
	CurrentScale = FMath::Clamp(CurrentScale, MinScale, MaxScale);

	if ((bIsIncreasing && CurrentScale >= MaxScale) || (!bIsIncreasing && CurrentScale <= MinScale))
	{
		bIsIncreasing = !bIsIncreasing;
	}

	Invalidate(EInvalidateWidget::Paint);
}

int32 SLoadingAnimatedWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FVector2D Center = AllottedGeometry.GetLocalSize() * 0.5f;
	const float BoundingBoxSideLength = DesiredDiagonal * CurrentScale;
	const FVector2D PaintSize = FVector2D(BoundingBoxSideLength, BoundingBoxSideLength);
	const FVector2D TopLeft = Center - FVector2D(BoundingBoxSideLength * 0.5f, BoundingBoxSideLength * 0.5f);
	const FPaintGeometry PaintGeometry(AllottedGeometry.ToPaintGeometry(TopLeft, PaintSize));

	FSlateDrawElement::MakeRotatedBox(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		RotationAngle,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		FLinearColor::White
	);

	return LayerId + 1;
}

FVector2D SLoadingAnimatedWidget::ComputeDesiredSize(float) const
{
	return FVector2D(MaxDiagonal, MaxDiagonal);
}