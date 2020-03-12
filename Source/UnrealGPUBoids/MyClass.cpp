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

	_outputPositions.SetNum(numBoids);

	_updateInstanceTransforms();

	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
	[&](FRHICommandListImmediate& RHICommands)
	{
		TShaderMapRef<FComputeShaderDeclaration> cs(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

		FRHIComputeShader * rhiComputeShader = cs->GetComputeShader();

		if (cs->positions.IsBound())
			RHICommands.SetUAVParameter(rhiComputeShader, cs->positions.GetBaseIndex(), _positionBufferUAV);

		RHICommands.SetComputeShader(rhiComputeShader);

		DispatchComputeShader(RHICommands, *cs, 64, 1, 1);

		// read back the data
		uint8* data = (uint8*)RHILockStructuredBuffer(_positionBuffer, 0, numBoids * sizeof(FVector), RLM_ReadOnly);
		FMemory::Memcpy(_outputPositions.GetData(), data, numBoids * sizeof(FVector));		

		RHIUnlockStructuredBuffer(_positionBuffer);
	});
}

void UComputeShaderBoidsComponent::_updateInstanceTransforms()
{
	UInstancedStaticMeshComponent * ismc = GetOwner()->FindComponentByClass<UInstancedStaticMeshComponent>();

	if (!ismc) return;

	ismc->SetSimulatePhysics(false);

	// resize up/down the ismc
	int toAdd = FMath::Max(0, numBoids - ismc->GetInstanceCount());
	int toRemove = FMath::Max(0, ismc->GetInstanceCount() - numBoids);

	for (int i = 0; i < toAdd; ++i)
		ismc->AddInstance(FTransform::Identity);
	for (int i = 0; i < toRemove; ++i)
		ismc->RemoveInstance(ismc->GetInstanceCount() - 1);
	
	// update the transforms
	_instanceTransforms.SetNum(_outputPositions.Num());

	for (int i = 0; i < _outputPositions.Num(); ++i)
	{
		FTransform& transform = _instanceTransforms[i];

		transform.SetTranslation(_outputPositions[i]);
	}

	ismc->BatchUpdateInstancesTransforms(0, _instanceTransforms, false, true, true);
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
