﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/C_ZombieBullet.h"

// Sets default values
AC_ZombieBullet::AC_ZombieBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(15.0f);
    SetRootComponent(CollisionComponent);

    SMComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMComponent"));
    SMComponent->SetupAttachment(CollisionComponent);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = 2000.0f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
}

// Called when the game starts or when spawned
void AC_ZombieBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_ZombieBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AC_ZombieBullet::GetInitialSpeed() const
{
    return InitialSpeed;
}

void AC_ZombieBullet::SetDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

