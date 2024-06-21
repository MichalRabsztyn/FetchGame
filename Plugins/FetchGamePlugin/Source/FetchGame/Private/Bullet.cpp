#include "Bullet.h"
#include "Net/UnrealNetwork.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	if(IsValid(BulletMesh))
	{
		RootComponent = BulletMesh;
	}

	BulletMesh->BodyInstance.bUseCCD = true;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(BulletMesh))
	{
		return;
	}

	if (HasAuthority())
	{
		BulletMesh->SetSimulatePhysics(true);
	}
	else
	{
		BulletMesh->SetSimulatePhysics(false);
	}
}

void ABullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABullet, BulletPosition);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		BulletPosition = GetActorLocation();
	}
	else
	{
		const FVector NewLocation = FMath::VInterpTo(GetActorLocation(), BulletPosition, DeltaTime, 20.0f);
		SetActorLocation(NewLocation);
	}
}