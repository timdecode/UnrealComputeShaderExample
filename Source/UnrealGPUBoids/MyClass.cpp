// Fill out your copyright notice in the Description page of Project Settings.


#include "MyClass.h"

#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

// Some useful links
// -----------------
// [Enqueue render commands using lambdas](https://github.com/EpicGames/UnrealEngine/commit/41f6b93892dcf626a5acc155f7d71c756a5624b0)
//



// Sets default values for this component's properties
UComputeShaderBoidsComponent::UComputeShaderBoidsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UComputeShaderBoidsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
    FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();
    
    FRHIResourceCreateInfo createInfo;
    
    positionResourceArray.Init(FVector::ZeroVector, numBoids);
    
    
    createInfo.ResourceArray = &positionResourceArray;
    
    
    _positionBuffer = RHICreateStructuredBuffer(sizeof(FVector), sizeof(FVector) * numBoids, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
	_positionBufferUAV = RHICreateUnorderedAccessView(_positionBuffer, false, false);
}


// Called every frame
void UComputeShaderBoidsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FRHICommandListImmediate& RHICommands = GRHICommandList.GetImmediateCommandList();




	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
	[&](FRHICommandListImmediate& RHICommands)
	{
		TShaderMapRef<FComputeShaderDeclaration> computeShader(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

		FRHIComputeShader * rhiComputeShader = computeShader->GetComputeShader();

		if (computeShader->positions.IsBound())
			RHICommands.SetUAVParameter(rhiComputeShader, computeShader->positions.GetBaseIndex(), _positionBufferUAV);

		DispatchComputeShader(RHICommands, *computeShader, 64, 1, 1);
	});
}

FComputeShaderDeclaration::FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
	positions.Bind(Initializer.ParameterMap, TEXT("positions"));
}

void FComputeShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}




IMPLEMENT_SHADER_TYPE(, FComputeShaderDeclaration, TEXT("/ComputeShaderPlugin/Boid.usf"), TEXT("MainComputeShader"), SF_Compute);
