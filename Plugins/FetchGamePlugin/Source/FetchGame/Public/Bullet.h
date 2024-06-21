#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class FETCHGAME_API ABullet : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
	FVector BulletPosition;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BulletMesh;

public:	
	ABullet();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
