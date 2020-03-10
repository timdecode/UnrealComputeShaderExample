// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyClass.generated.h"

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidPosition, )
SHADER_PARAMETER(FVector3, position)
END_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidPosition)

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidVelocity, )
SHADER_PARAMETER(FVector3, velocity)
END_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidVelocity)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALGPUBOIDS_API UComputeShaderBoidsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyClass();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
