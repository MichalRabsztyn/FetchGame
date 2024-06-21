#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class ASYNCLOADINGSCREEN_API SLoadingAnimatedWidget : public SCompoundWidget
{
private:
	float MaxScale;
	float MinScale;
	float ScaleSpeed;
	float SpinSpeed;
	float EaseFactor;
	float DesiredDiagonal;

	float CurrentScale = 1.0f;
	float RotationAngle = 0.0f;
	bool bIsIncreasing = true;

	float MaxDiagonal;

public:
	SLATE_BEGIN_ARGS(SLoadingAnimatedWidget)
		: _MaxScale(1.0f)
		, _MinScale(0.5f)
		, _ScaleSpeed(1.5f)
		, _SpinSpeed(3.0f)
		, _EaseFactor(0.5f)
		, _DesiredDiagonal(100.0f)
		{}
		SLATE_ARGUMENT(float, MaxScale)
		SLATE_ARGUMENT(float, MinScale)
		SLATE_ARGUMENT(float, ScaleSpeed)
		SLATE_ARGUMENT(float, SpinSpeed)
		SLATE_ARGUMENT(float, EaseFactor)
		SLATE_ARGUMENT(float, DesiredDiagonal)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
};
