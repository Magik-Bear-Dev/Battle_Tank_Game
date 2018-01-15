// Copyright marksappstudio

#include "TankAIController.h"
#include "TankAimingComponent.h"
#include "BattleTank.h"
// Depends on movement component via pathfinding system.

void ATankAIController::BeginPlay()
{
    Super::BeginPlay();
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
        
    // Aim towards the player
    auto AimingComponent = ControlledTank->FindComponentByClass<UTankAimingComponent>();
    AimingComponent->AimAt(PlayerTank->GetActorLocation());

    AimingComponent->Fire(); // TODO limit firing rate
}
