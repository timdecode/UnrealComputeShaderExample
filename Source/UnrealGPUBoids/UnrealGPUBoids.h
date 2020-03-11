// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FGPUBoids : public IModuleInterface
{
	void StartupModule() override {
		FString ShaderDirectory = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("UnrealGPUBoids/Shaders"));
		AddShaderSourceDirectoryMapping("/ComputeShaderPlugin", ShaderDirectory);
	}
};