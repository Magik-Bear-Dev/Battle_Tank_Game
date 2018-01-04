// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "Tank.h"
#include "TankAimingComponent.h"
#include "BattleTank.h"

void ATankPlayerController::BeginPlay()
{
    Super::BeginPlay();
    ATank *ctlTank = GetControlledTank();
    if(!ctlTank) return;
    auto AimingComponent = ctlTank->FindComponentByClass<UTankAimingComponent>();
    if (AimingComponent)
    FoundAimingComponent(AimingComponent);
}

void ATankPlayerController::Tick(float DeltaTime)
{
    Super::Tick( DeltaTime );
    AimTowardsCrosshair();
}


ATank* ATankPlayerController::GetControlledTank() const
{
    return Cast<ATank>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair()
{
    if (!GetControlledTank()) { return; }
    
    
    FVector HitLocation; // Out parameter
    if (GetSightRayHitLocation(HitLocation)) // Has "side-effect", is going to line trace
    {
        GetControlledTank()->AimAt(HitLocation);
    }
}

// Get world location of linetrace through crosshair, true if hits landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
    // Find the Cosshair position in pixel coordinates
    int32 ViewportSizeX, ViewportSizeY;
    GetViewportSize(ViewportSizeX, ViewportSizeY);
    auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);
    
    // "De-project" the screen position of the crosshair to a world direction
    FVector LookDirection;
    if (GetLookDirection(ScreenLocation, LookDirection))
    {
        // Line-trace along that LookDrection, and see what we hit (up to max range)
        GetVectorHitLocation(LookDirection, HitLocation);
    }
    
    
    return true;
}

bool ATankPlayerController::GetVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
    FHitResult HitResult;
    auto StartLocation = PlayerCameraManager->GetCameraLocation();
    auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
    if (GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartLocation,
            EndLocation,
            ECollisionChannel::ECC_Visibility)
        )
    {
        HitLocation = HitResult.Location;
        return true;
    }
        HitLocation = FVector (0);
        return false; // Line trace didn't succeed
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
    FVector CameraWorldLocation; // To be discarded
    return DeprojectScreenPositionToWorld(
      ScreenLocation.X,
      ScreenLocation.Y,
      CameraWorldLocation,
      LookDirection
    );
}
