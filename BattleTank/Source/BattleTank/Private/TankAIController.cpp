// Copyright marksappstudio

#include "TankAIController.h"
#include "TankAimingComponent.h"
#include "Tank.h" // So we can implement OnDeath
#include "BattleTank.h"
// Depends on movement component via pathfinding system.

void ATankAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ATankAIController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);
    if (InPawn)
    {
        auto PossessedTank = Cast<ATank>(InPawn);
        if (!PossessedTank) { return; }
        
        // Subscribe our local method to the tank's death event
        PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankAIController::OnPossessedTankDeath);
    }
}

void ATankAIController::OnPossessedTankDeath()
{
    if (!GetPawn()) { return; }
    GetPawn()->DetachFromControllerPendingDestroy();
}

// Called every frame
void ATankAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    auto PlayerTank = GetWorld()->GetFirstPlayerController()->GetPawn();
    auto ControlledTank = GetPawn();
    
    if (!PlayerTank && !ControlledTank) { return; }

    //Move towards the player
    MoveToActor(PlayerTank, AcceptanceRadius); // Check radius is in cm
    
    if(!PlayerTank){ return; }
    
    // Aim towards the player
    auto AimingComponent = ControlledTank->FindComponentByClass<UTankAimingComponent>();
    AimingComponent->AimAt(PlayerTank->GetActorLocation());

    if (AimingComponent->GetFiringState() == EFiringState::Locked)
    {
        AimingComponent->Fire(); // TODO limit firing rate
    }
}
