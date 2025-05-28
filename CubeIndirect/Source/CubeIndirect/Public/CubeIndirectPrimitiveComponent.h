// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "CubeIndirectPrimitiveComponent.generated.h"

/**
 * 
 */
UCLASS()
class CUBEINDIRECT_API UCubeIndirectPrimitiveComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="CubeIndirect")
	int32 InstanceCount = 4;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	virtual bool ShouldRecreateProxyOnUpdateTransform() const override;

	virtual void SendRenderDynamicData_Concurrent() override;

	virtual void OnRegister() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FPrimitiveSceneProxy* CreateSceneProxy() override;
};
