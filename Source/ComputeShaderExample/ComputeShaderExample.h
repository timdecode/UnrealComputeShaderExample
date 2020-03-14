// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FComputeShaderExample : public IModuleInterface
{
	virtual bool IsGameModule() const override
	{
		return true;
	}

	void StartupModule() override {
		FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
		AddShaderSourceDirectoryMapping("/ComputeShaderPlugin", ShaderDirectory);
	}
};