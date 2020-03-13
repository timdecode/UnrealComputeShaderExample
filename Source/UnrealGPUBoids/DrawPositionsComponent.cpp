// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawPositionsComponent.h"

#include "ComputeShaderBoidsComponent.h"

// Sets default values for this component's properties
UDrawPositionsComponent::UDrawPositionsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDrawPositionsComponent::BeginPlay()
{
	Super::BeginPlay();

	_initISMC();
}


// Called every frame
void UDrawPositionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	_updateInstanceTransforms();
}

void UDrawPositionsComponent::_initISMC()
{
	UInstancedStaticMeshComponent * ismc = GetOwner()->FindComponentByClass<UInstancedStaticMeshComponent>();

	if (!ismc) return;

	ismc->SetSimulatePhysics(false);

	ismc->SetMobility(EComponentMobility::Movable);
	ismc->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ismc->SetCanEverAffectNavigation(false);
	//ismc->UseDynamicInstanceBuffer = true;
	//ismc->KeepInstanceBufferCPUAccess = true;
	ismc->SetCollisionProfileName(TEXT("NoCollision"));
}

void UDrawPositionsComponent::_updateInstanceTransforms()
{
	UInstancedStaticMeshComponent * ismc = GetOwner()->FindComponentByClass<UInstancedStaticMeshComponent>();

	if (!ismc) return;

	UComputeShaderBoidsComponent * boidsComponent = GetOwner()->FindComponentByClass<UComputeShaderBoidsComponent>();

	if (!boidsComponent) return;

	TArray<FVector>& positions = boidsComponent->outputPositions;

	// resize up/down the ismc
	int toAdd = FMath::Max(0, positions.Num() - ismc->GetInstanceCount());
	int toRemove = FMath::Max(0, ismc->GetInstanceCount() - positions.Num());

	for (int i = 0; i < toAdd; ++i)
		ismc->AddInstance(FTransform::Identity);
	for (int i = 0; i < toRemove; ++i)
		ismc->RemoveInstance(ismc->GetInstanceCount() - 1);

	// update the transforms
	_instanceTransforms.SetNum(positions.Num());

	for (int i = 0; i < positions.Num(); ++i)
	{
		FTransform& transform = _instanceTransforms[i];

		transform.SetTranslation(positions[i]);
		transform.SetScale3D(FVector(0.05f));
		transform.SetRotation(FQuat::Identity);
	}

	ismc->BatchUpdateInstancesTransforms(0, _instanceTransforms, false, true, true);

	ismc->UpdateBounds();
}

