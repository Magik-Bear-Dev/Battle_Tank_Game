// Copyright marksappstudio


#include "TankAimingComponent.h"
#include "Projectile.h"
#include "TankTurret.h"
#include "TankBarrel.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    bWantsBeginPlay = true;
    PrimaryComponentTick.bCanEverTick = true;
    // ...
}

void UTankAimingComponent::BeginPlay()
{
    // Do that first fire is after intial reload
    LastFireTime = FPlatformTime::Seconds();
}

void UTankAimingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if ((FPlatformTime::Seconds() - LastFireTime) > ReloadTimeInSeconds)
    {
        FiringState = EFiringState::Reloading;
    }
    // TODO Handle aiming and locked states
}

void UTankAimingComponent::Initialise(UTankBarrel* BarrelToSet, UTankTurret* TurretToSet)
{
    Barrel = BarrelToSet;
    Turret = TurretToSet;
}

void UTankAimingComponent::AimAt(FVector HitLocation)
{
    if (!Barrel) { return; }
    
    FVector OutLaunchVelocity;
    FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));
    bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity
    (
     this,
     OutLaunchVelocity,
     StartLocation,
     HitLocation,
     LaunchSpeed,
     false,
     0,
     0,
     ESuggestProjVelocityTraceOption::DoNotTrace // Parameter must be present to prevent bug
     );
    if (bHaveAimSolution)
    {
        auto AimDirection = OutLaunchVelocity.GetSafeNormal();
        MoveBarrelTowards(AimDirection);
    }
    // If no solution found do nothing
}

void UTankAimingComponent::MoveBarrelTowards(FVector AimDirection)
{
    if (!Barrel || !Turret) { return; }
  
    // Work-out difference between current barrel roation, and AimDirection
    auto BarrelRotator = Barrel->GetForwardVector().Rotation();
    auto AimAsRotator = AimDirection.Rotation();
    auto DeltaRotator = AimAsRotator - BarrelRotator;
    
    Barrel->Elevate(DeltaRotator.Pitch);
    Turret->Rotate(DeltaRotator.Yaw);
}

void UTankAimingComponent::Fire()
{
    if (FiringState != EFiringState::Reloading)
    {
       // Spawn a projectile at the socket location on the barrel
       if (!Barrel) { return; }
       if (!ProjectileBlueprint) { return; }
       auto Projectile = GetWorld()->SpawnActor<AProjectile>(
       ProjectileBlueprint,
       Barrel->GetSocketLocation(FName("Projectile")),
       Barrel->GetSocketRotation(FName("Projectile"))
       );
        
        Projectile->LaunchProjectile(LaunchSpeed);
        LastFireTime = FPlatformTime::Seconds();
    }
}
