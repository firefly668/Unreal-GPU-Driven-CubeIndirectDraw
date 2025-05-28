// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RHI.h"
#include "RenderResource.h"
#include "CubeIndirectPrimitiveComponent.h"

#include "FireflyCubeIndirectManager.generated.h"

UCLASS()
class CUBEINDIRECT_API AFireflyCubeIndirectManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireflyCubeIndirectManager();

	UPROPERTY(VisibleAnywhere, Category="Components")
	UCubeIndirectPrimitiveComponent* CubeIndirectPC;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
