// Fill out your copyright notice in the Description page of Project Settings.


#include "ComputeShaderTestComponent.h"

#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

// Some useful links
// -----------------
// [Enqueue render commands using lambdas](https://github.com/EpicGames/UnrealEngine/commit/41f6b93892dcf626a5acc155f7d71c756a5624b0)
//



// Sets default values for this component's properties
UComputeShaderTestComponent::UComputeShaderTestComponent() 
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UComputeShaderTestComponent::BeginPlay()
{
	Super::BeginPlay();

    FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();

	FRandomStream rng;

	{
		TResourceArray<FVector> positionResourceArray;
		positionResourceArray.Init(FVector::ZeroVector, numBoids);


		for (FVector& position : positionResourceArray)
		{
			position = rng.GetUnitVector() * rng.GetFraction() * spawnRadius;
		}

		FRHIResourceCreateInfo createInfo;
		createInfo.ResourceArray = &positionResourceArray;

		_positionBuffer = RHICreateStructuredBuffer(sizeof(FVector), sizeof(FVector) * numBoids, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_positionBufferUAV = RHICreateUnorderedAccessView(_positionBuffer, false, false);
	}
    
	{
		TResourceArray<float> timesResourceArray;
		timesResourceArray.Init(0.0f, numBoids);

		for (float& time : timesResourceArray)
			time = rng.GetFraction();

		FRHIResourceCreateInfo createInfo;
		createInfo.ResourceArray = &timesResourceArray;

		_timesBuffer = RHICreateStructuredBuffer(sizeof(float), sizeof(float) * numBoids, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_timesBufferUAV = RHICreateUnorderedAccessView(_timesBuffer, false, false);
	}

	if (outputPositions.Num() != numBoids)
	{
		const FVector zero(0.0f);
		outputPositions.Init(zero, numBoids);
	}
}

// Called every frame
void UComputeShaderTestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
	[&](FRHICommandListImmediate& RHICommands)
	{
		TShaderMapRef<FComputeShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

		FRHIComputeShader * rhiComputeShader = cs->GetComputeShader();

		RHICommands.SetUAVParameter(rhiComputeShader, cs->positions.GetBaseIndex(), _positionBufferUAV);
		RHICommands.SetUAVParameter(rhiComputeShader, cs->times.GetBaseIndex(), _timesBufferUAV);

		RHICommands.SetComputeShader(rhiComputeShader);

		DispatchComputeShader(RHICommands, *cs, 256, 1, 1);

		// read back the data
		uint8* data = (uint8*)RHILockStructuredBuffer(_positionBuffer, 0, numBoids * sizeof(FVector), RLM_ReadOnly);
		FMemory::Memcpy(outputPositions.GetData(), data, numBoids * sizeof(FVector));		

		RHIUnlockStructuredBuffer(_positionBuffer);
	});
}

FComputeShaderDeclaration::FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
	positions.Bind(Initializer.ParameterMap, TEXT("positions"));
	times.Bind(Initializer.ParameterMap, TEXT("times"));
}

void FComputeShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

IMPLEMENT_SHADER_TYPE(, FComputeShaderDeclaration, TEXT("/ComputeShaderPlugin/Boid.usf"), TEXT("MainComputeShader"), SF_Compute);
