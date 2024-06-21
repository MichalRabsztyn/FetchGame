#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletShootComponent.generated.h"


UCLASS(ClassGroup = (BulletShootComponent), meta = (BlueprintSpawnableComponent))
class FETCHGAME_API UBulletShootComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	FTraceDelegate TraceDelegate;

	UPROPERTY(EditAnywhere, Category = "Bullet")
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditAnywhere, Category = "Trajectory Prediction")
	float MaxTraceDistance = 10000.0f;
	UPROPERTY(EditAnywhere, Category = "Trajectory Prediction")
	float ForwardBulletSpawnOffset = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Trajectory Prediction")
	float UpBulletSpawnOffset = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Trajectory Prediction")
	float SimTime = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Trajectory Prediction")
	float SimFrequency = 30.0f;

	UPROPERTY(EditAnywhere, Category = "AIActor")
	TSubclassOf<AActor> AIActorClass;
	UPROPERTY(EditAnywhere, Category = "AIActor")
	FVector AIActorSpawnOffset = FVector::ZeroVector;

	UPROPERTY(Replicated)
	FVector BulletVelocity = FVector::ZeroVector;
	UPROPERTY(Replicated)
	FVector BulletSpawnLocation = FVector::ZeroVector;

	TArray<AActor*> BulletTrajectory;

public:
	UBulletShootComponent();

	void SetBulletTrajectoryPreview(bool bShouldBeVisible);
	void PredictBulletPath();
	void ShootBullet();

	UFUNCTION(BlueprintCallable)
	void OnButtonReleased();
	UFUNCTION(BlueprintCallable)
	void OnButtonPressed();

	UFUNCTION(Server, Reliable)
	void Server_ShootBullet();

	UFUNCTION(Server, Reliable)
	void Server_RequestAsyncTrace();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Sv_OnBulletHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	void Sv_OnAsyncTraceDone(const FTraceHandle& Handle, FTraceDatum& Data);
};
