// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "TankBarrel.generated.h"

/**
 *
 */
UCLASS(meta=(BlueprintSpawnableComponent)) //, hidecategories = ("Collision"))
class BATTLETANK_API UTankBarrel : public UStaticMeshComponent
{
    GENERATED_BODY()
    
public:
    // -1 downward speed, +1 is max up movement
    void Elevate(float RelativeSpeed);
    
private:
  UPROPERTY(EditDefaultsOnly, Category = "Setup")
  float MaxDegressPerSecond = 10;
    
  UPROPERTY(EditDefaultsOnly, Category = "Setup")
  float MaxElevationDegrees = 40;
    
  UPROPERTY(EditDefaultsOnly, Category = "Setup")
  float MinElevationDegrees= 0; 
    
};
