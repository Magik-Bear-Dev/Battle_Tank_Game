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

void UTankAimingComponent::Initialise(UTankBarrel* BarrelToSet, UTankTurret* TurretToSet)
{
    Barrel = BarrelToSet;
    Turret = TurretToSet;
}

void UTankAimingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if (RoundsLeft <= 0)
    {
        FiringState = EFiringState::OutOfAmmo;
    }
    else if ((FPlatformTime::Seconds() - LastFireTime) < ReloadTimeInSeconds)
    {
        FiringState = EFiringState::Reloading;
    }
    else if (IsBarrelMoving())
    {
        FiringState = EFiringState::Aiming;
        
    }
    else
    {
        FiringState = EFiringState::Locked;
    }
}

int32 UTankAimingComponent::GetRoundsLeft() const
{
    return RoundsLeft;
}

EFiringState UTankAimingComponent::GetFiringState() const
{
    return FiringState;
}

bool UTankAimingComponent::IsBarrelMoving()
{
    if (!Barrel) { return false; }
    auto BarrelForward = Barrel->GetForwardVector();
    return !BarrelForward.Equals(AimDirection, 0.01); // vectors are equal
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
        AimDirection = OutLaunchVelocity.GetSafeNormal();
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
    
    // Always yaw the shortest way
    Barrel->Elevate(DeltaRotator.Pitch);
    if (FMath::Abs(DeltaRotator.Yaw) < 180)
    {
        Turret->Rotate(DeltaRotator.Yaw);
    }
    else // Avoid going the long-way round
    {
        Turret->Rotate(-DeltaRotator.Yaw);
        
    }
}

void UTankAimingComponent::Fire()
{
    if (FiringState == EFiringState::Locked || FiringState == EFiringState::Aiming)
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
        RoundsLeft--;
    }
}
