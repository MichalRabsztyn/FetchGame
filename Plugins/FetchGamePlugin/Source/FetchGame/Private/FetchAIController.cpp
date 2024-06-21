#include "FetchAIController.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AFetchAIController::AFetchAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
}

void AFetchAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(AFetchAIController, TargetActor, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(AFetchAIController, BulletActor, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(AFetchAIController, PlayerActor, SharedParams);
}

void AFetchAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Sv_PlayFetch();
}

void AFetchAIController::Server_SetTargetActor_Implementation(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

void AFetchAIController::Server_SetBulletActor_Implementation(AActor* NewBullet)
{
	BulletActor = NewBullet;
}

void AFetchAIController::Server_SetPlayerActor_Implementation(AActor* NewPlayer)
{
	PlayerActor = NewPlayer;
}

void AFetchAIController::Sv_PlayFetch()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	AActor* const SelfActor = GetPawn();
	if (!IsValid(SelfActor))
	{
		Destroy();
		return;
	}

	if(!TargetActor.IsValid())
	{
		SelfActor->Destroy();
		Destroy();
		return;
	}

	MoveToActor(TargetActor.Get(), 5.0f, true, true, true, 0, true);

	const FVector SelfLocation = SelfActor->GetActorLocation();
	const float DistanceSquared = FVector::DistSquared(TargetActor->GetActorLocation(), SelfLocation);
	if (DistanceSquared > 12500.0f)
	{
		return;
	}

	if (!BulletActor.IsValid())
	{
		return;
	}

	if (TargetActor == BulletActor)
	{
		TargetActor = PlayerActor;

		MARK_PROPERTY_DIRTY_FROM_NAME(AFetchAIController, TargetActor, this);

		BulletActor->DisableComponentsSimulatePhysics();
		BulletActor->SetActorEnableCollision(false);
		BulletActor->AttachToComponent(SelfActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		BulletActor->SetActorLocation(SelfLocation + SelfActor->GetActorForwardVector() * 50.0f + SelfActor->GetActorUpVector() * 50.0f);
	}
	else if (TargetActor == PlayerActor)
	{
		BulletActor->Destroy();
		BulletActor = nullptr;

		MARK_PROPERTY_DIRTY_FROM_NAME(AFetchAIController, BulletActor, this);

		SelfActor->Destroy();
		Destroy();
	}
}