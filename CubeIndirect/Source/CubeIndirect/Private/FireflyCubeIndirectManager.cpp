// Fill out your copyright notice in the Description page of Project Settings.


#include "FireflyCubeIndirectManager.h"

#include "CubeIndirectPrimitiveComponent.h"
// Sets default values
AFireflyCubeIndirectManager::AFireflyCubeIndirectManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    CubeIndirectPC = CreateDefaultSubobject<UCubeIndirectPrimitiveComponent>(TEXT("CubeIndirectPrimitiveComponent"));
    CubeIndirectPC->SetupAttachment(RootComponent);
    CubeIndirectPC->PrimaryComponentTick.bCanEverTick = true;
    CubeIndirectPC->SetComponentTickEnabled(true);
}

// Called when the game starts or when spawned
void AFireflyCubeIndirectManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFireflyCubeIndirectManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

