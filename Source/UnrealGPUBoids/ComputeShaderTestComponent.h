// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

#include <atomic>

#include "ComputeShaderTestComponent.generated.h"



BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidPosition, )
SHADER_PARAMETER(FVector, position)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidVelocity, )
SHADER_PARAMETER(FVector, velocity)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

class FComputeShaderDeclaration : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FComputeShaderDeclaration, Global);

	FComputeShaderDeclaration() {}

	explicit FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
		return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
	};

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);

		Ar << positions;
		Ar << times;

		return bShaderHasOutdatedParams;
	}

public:
	FShaderResourceParameter positions;
	FShaderResourceParameter times;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPUTESHADEREXAMPLE_API UComputeShaderTestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UComputeShaderTestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int numBoids = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVector> outputPositions;

protected:
	// GPU side
	FStructuredBufferRHIRef _positionBuffer;
	FUnorderedAccessViewRHIRef _positionBufferUAV;     // we need a UAV for writing

	FStructuredBufferRHIRef _timesBuffer;
	FUnorderedAccessViewRHIRef _timesBufferUAV;
};
