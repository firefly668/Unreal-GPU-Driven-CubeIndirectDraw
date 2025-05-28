#pragma once

#include "MeshDrawShaderBindings.h"
#include "MeshMaterialShader.h"
#include "VertexFactory.h"

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FCubeIndirectVertexFactoryUniformShaderParameters, CUBEINDIRECT_API)
	SHADER_PARAMETER_SRV(StructuredBuffer<float4>, InstanceOriginBuffer)
	SHADER_PARAMETER_SRV(StructuredBuffer<float4>, InstanceTransformBuffer)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

//FLocalVertexFactory没有Instance数据，官方给出了Instance的版本FInstancedStaticMeshVertexFactory，但接口没有从Engine模块中开放，所以我们只能仿照其写一个。如果只想显示一个Instance，那FLocalVertexFactory足矣
class FCubeIndirectVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE_API(FCubeIndirectVertexFactory, CUBEINDIRECT_API)
public:
	FCubeIndirectVertexFactory(ERHIFeatureLevel::Type FeatureLevel, const char* InDebugName)
		:FVertexFactory(FeatureLevel)
		,DebugName(InDebugName)

	{
		
	}

	struct FCubeIndirectDataType
	{
		/** The stream to read the vertex position from. */
		FVertexStreamComponent PositionComponent;

		/** The stream to read the vertex position from. */
		FVertexStreamComponent TextureCoordinateComponent;

		/** The streams to read the tangent basis from. */
		FVertexStreamComponent NormalComponent;

		/** The streams to read the tangent basis from. */
		FVertexStreamComponent TangentComponent;

		FRHIUnorderedAccessView* InstanceOriginUAV= nullptr;

		FRHIUnorderedAccessView* InstanceTransformUAV = nullptr;

		FRHIShaderResourceView* InstanceOriginSRV = nullptr;

		FRHIShaderResourceView* InstanceTransformSRV = nullptr;
	};

	static CUBEINDIRECT_API bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters);

	CUBEINDIRECT_API void SetData(FRHICommandListBase& RHICmdList, const FCubeIndirectDataType& InData);

	CUBEINDIRECT_API virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

	static CUBEINDIRECT_API void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
	static CUBEINDIRECT_API void ValidateCompiledResult(const FVertexFactoryType* Type, EShaderPlatform Platform, const FShaderParameterMap& ParameterMap, TArray<FString>& OutErrors);
	virtual void ReleaseRHI() override
	{
		UniformBuffer.SafeRelease();
		FVertexFactory::ReleaseRHI();
	}

	FORCEINLINE_DEBUGGABLE FUniformBufferRHIRef GetUniformBuffer() const
	{
		return UniformBuffer;
	}

protected:

	FCubeIndirectDataType Data;
	TUniformBufferRef<FCubeIndirectVertexFactoryUniformShaderParameters> UniformBuffer;

	struct FDebugName
	{
		FDebugName(const char* InDebugName)
#if !UE_BUILD_SHIPPING
			: DebugName(InDebugName)
#endif
		{}
	private:
#if !UE_BUILD_SHIPPING
		const char* DebugName;
#endif
	} DebugName;
};

class FCubeIndirectVFShaderParameters : public FVertexFactoryShaderParameters
{
	DECLARE_TYPE_LAYOUT(FCubeIndirectVFShaderParameters, NonVirtual);
public:
	void Bind(const FShaderParameterMap& ParameterMap)
	{
		int isss = 0;
	}

	void GetElementShaderBindings(
		const FSceneInterface* Scene,
		const FSceneView* View,
		const FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const FVertexFactory* InVertexFactory,
		const FMeshBatchElement& BatchElement,
		FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams
	) const
	{
		FCubeIndirectVertexFactory* VertexFactory = (FCubeIndirectVertexFactory*)InVertexFactory;
		ShaderBindings.Add(Shader->GetUniformBufferParameter<FCubeIndirectVertexFactoryUniformShaderParameters>(), VertexFactory->GetUniformBuffer());
	}
};