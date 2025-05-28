#pragma once

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FCubeIndirectCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FCubeIndirectCS)
	SHADER_USE_PARAMETER_STRUCT(FCubeIndirectCS, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER(uint32, IndexCount)
		SHADER_PARAMETER(uint32, InstanceCount)
		SHADER_PARAMETER_UAV(Buffer<float4>, InstanceOriginBuffer)
		SHADER_PARAMETER_UAV(Buffer<float4>, InstanceTransformBuffer)
		SHADER_PARAMETER_UAV(Buffer<uint>, IndirectArgsBuffer)
	END_SHADER_PARAMETER_STRUCT()

	void SetParameters(FRHICommandList& RHICmdList, const FCubeIndirectCS::FParameters InParameters)
	{
		TShaderRef<FCubeIndirectCS> Shader = GetGlobalShaderMap(GMaxRHIFeatureLevel)->GetShader<FCubeIndirectCS>();
		const FShaderParametersMetadata* ParametersMetadata = FCubeIndirectCS::FParameters::FTypeInfo::GetStructMetadata();
		// 将参数结构体绑定到Shader的Uniform Buffer
		SetShaderParameters(
			RHICmdList,
			Shader.GetComputeShader(),
			Shader->Bindings,
			ParametersMetadata,
			&InParameters
		);
	}
};

IMPLEMENT_GLOBAL_SHADER(FCubeIndirectCS, "/CubeIndirectShaders/private/CubeIndirectCS.usf", "MainCS", SF_Compute);