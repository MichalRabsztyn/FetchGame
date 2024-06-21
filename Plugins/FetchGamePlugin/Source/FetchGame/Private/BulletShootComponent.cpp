#include "BulletShootComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Kismet/GameplayStatics.h"

#include "FetchAIController.h"

UBulletShootComponent::UBulletShootComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
}

void UBulletShootComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UBulletShootComponent, BulletVelocity, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UBulletShootComponent, BulletSpawnLocation, SharedParams);
}

void UBulletShootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Server_RequestAsyncTrace();

	PredictBulletPath();
}

void UBulletShootComponent::Sv_OnBulletHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		return;
	}

	SelfActor->OnActorHit.RemoveDynamic(this, &UBulletShootComponent::Sv_OnBulletHit);

	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (!IsValid(AIActorClass))
	{
		return;
	}

	const AController* const PlayerController = Cast<AController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	APawn* const PlayerPawn = PlayerController->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		return;
	}

	APawn* const AIActor = World->SpawnActor<APawn>(AIActorClass, PlayerPawn->GetActorLocation() + AIActorSpawnOffset, PlayerPawn->GetActorRotation());
	if (!IsValid(AIActor))
	{
		return;
	}

	AFetchAIController* const AIController = World->SpawnActor<AFetchAIController>(AIActor->GetActorLocation(), AIActor->GetActorRotation());
	if (!IsValid(AIController))
	{
		return;
	}

	AIController->Server_SetTargetActor(SelfActor);
	AIController->Server_SetBulletActor(SelfActor);
	AIController->Server_SetPlayerActor(PlayerPawn);
	AIController->Possess(AIActor);
}

void UBulletShootComponent::Server_RequestAsyncTrace_Implementation()
{
	UWorld* const CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld))
	{
		return;
	}

	const APlayerController* const PlayerController = Cast<APlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	const APlayerCameraManager* const CameraManager = PlayerController->PlayerCameraManager;
	if (!IsValid(CameraManager))
	{
		return;
	}

	const FVector CameraLocation = CameraManager->GetCameraLocation();
	const FVector CameraForwardVector = CameraManager->GetCameraRotation().Vector();

	const FVector TraceStart = CameraLocation;
	const FVector TraceEnd = CameraLocation + (CameraForwardVector * MaxTraceDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerController);
	Params.AddIgnoredActor(PlayerController->GetPawn());

	CurrentWorld->AsyncLineTraceByChannel(EAsyncTraceType::Single, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera, Params, FCollisionResponseParams::DefaultResponseParam, &TraceDelegate);

	TraceDelegate.BindUObject(this, &UBulletShootComponent::Sv_OnAsyncTraceDone);
}

void UBulletShootComponent::Sv_OnAsyncTraceDone(const FTraceHandle& Handle, FTraceDatum& Data)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	const APlayerController* const PlayerController = Cast<APlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	const APawn* const Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}

	BulletSpawnLocation = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * ForwardBulletSpawnOffset + Pawn->GetActorUpVector() * UpBulletSpawnOffset;
	MARK_PROPERTY_DIRTY_FROM_NAME(UBulletShootComponent, BulletSpawnLocation, this);
	
	FVector TargetLocation = Data.End;
	if (Data.OutHits.Num() > 0)
	{
		TargetLocation = Data.OutHits[0].ImpactPoint;
	}

	FVector NewBulletVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(World, NewBulletVelocity, BulletSpawnLocation, TargetLocation);

	if (NewBulletVelocity != BulletVelocity)
	{
		BulletVelocity = NewBulletVelocity;
		MARK_PROPERTY_DIRTY_FROM_NAME(UBulletShootComponent, BulletVelocity, this);
	}
}

void UBulletShootComponent::SetBulletTrajectoryPreview(bool bShouldBeVisible)
{
	SetComponentTickEnabled(bShouldBeVisible);

	for (AActor* Bullet : BulletTrajectory)
	{
		Bullet->Destroy();
	}

	BulletTrajectory.Reset();
}

void UBulletShootComponent::PredictBulletPath()
{
	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (!IsValid(BulletClass))
	{
		return;
	}

	for (AActor* Bullet : BulletTrajectory)
	{
		if (IsValid(Bullet))
		{
			Bullet->Destroy();
		}
	}
	BulletTrajectory.Reset();

	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = BulletSpawnLocation;
	PredictParams.LaunchVelocity = BulletVelocity;
	PredictParams.MaxSimTime = SimTime;
	PredictParams.SimFrequency = SimFrequency;

	FPredictProjectilePathResult PredictResult;
	UGameplayStatics::PredictProjectilePath(World, PredictParams, PredictResult);

	for (FPredictProjectilePathPointData& Point : PredictResult.PathData)
	{
		AActor* const BulletPreview = World->SpawnActor<AActor>(BulletClass, Point.Location, FRotator::ZeroRotator);
		if (!IsValid(BulletPreview))
		{
			continue;
		}

		UStaticMeshComponent* const StaticMeshComponent = Cast<UStaticMeshComponent>(BulletPreview->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (!IsValid(StaticMeshComponent))
		{
			continue;
		}

		StaticMeshComponent->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
		StaticMeshComponent->SetSimulatePhysics(false);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMeshComponent->SetCastShadow(false);
		BulletPreview->SetReplicates(false);

		BulletTrajectory.Add(BulletPreview);
	}
}

void UBulletShootComponent::OnButtonReleased()
{
	SetBulletTrajectoryPreview(false);

	Server_ShootBullet();
}

void UBulletShootComponent::OnButtonPressed()
{
	SetBulletTrajectoryPreview(true);
}

void UBulletShootComponent::ShootBullet()
{
	if (!IsValid(BulletClass))
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	const APlayerController* const PlayerController = Cast<APlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	const APawn* const Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}

	AActor* const Bullet = World->SpawnActor<AActor>(BulletClass, BulletSpawnLocation, Pawn->GetActorRotation());
	if (!IsValid(Bullet))
	{
		return;
	}

	UStaticMeshComponent* const StaticMeshComponent = Cast<UStaticMeshComponent>(Bullet->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!IsValid(StaticMeshComponent))
	{
		return;
	}

	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetPhysicsLinearVelocity(BulletVelocity, true);
	StaticMeshComponent->SetNotifyRigidBodyCollision(true);
	Bullet->OnActorHit.AddDynamic(this, &UBulletShootComponent::Sv_OnBulletHit);
	Bullet->SetReplicates(true);
}

void UBulletShootComponent::Server_ShootBullet_Implementation()
{
	ShootBullet();
}