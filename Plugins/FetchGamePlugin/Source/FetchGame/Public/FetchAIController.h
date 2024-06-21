#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "FetchAIController.generated.h"


UCLASS()
class FETCHGAME_API AFetchAIController : public AAIController
{
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> BulletActor;
	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> PlayerActor;

public:
	AFetchAIController();

	UFUNCTION(Server, Reliable)
	void Server_SetTargetActor(AActor* NewTarget);
	UFUNCTION(Server, Reliable)
	void Server_SetBulletActor(AActor* NewBullet);
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerActor(AActor* NewPlayer);

private:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	void Sv_PlayFetch();
};
