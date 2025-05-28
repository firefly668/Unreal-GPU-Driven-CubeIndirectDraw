#include "CubeIndirectVertexFactory.h"

#include "MeshMaterialShader.h"
#include <MaterialDomain.h>

bool FCubeIndirectVertexFactory::ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
{
	return RHISupportsManualVertexFetch(Parameters.Platform) && Parameters.MaterialParameters.MaterialDomain == MD_UI ? !! WITH_EDITOR : true;
}

void FCubeIndirectVertexFactory::SetData(FRHICommandListBase& RHICmdList, const FCubeIndirectDataType& InData)
{
	Data = InData;
	InitRHI(RHICmdList);
}

void FCubeIndirectVertexFactory::InitRHI(FRHICommandListBase& RHICmdList)
{
	FVertexDeclarationElementList Elements;
	Elements.Add(AccessStreamComponent(Data.PositionComponent, 0));
	Elements.Add(AccessStreamComponent(Data.TextureCoordinateComponent, 1));
	Elements.Add(AccessStreamComponent(Data.NormalComponent, 2));
	Elements.Add(AccessStreamComponent(Data.TangentComponent, 3));
	InitDeclaration(Elements);

	FCubeIndirectVertexFactoryUniformShaderParameters ShaderParameters;
	ShaderParameters.InstanceOriginBuffer = Data.InstanceOriginSRV;
	ShaderParameters.InstanceTransformBuffer = Data.InstanceTransformSRV;
	UniformBuffer = TUniformBufferRef<FCubeIndirectVertexFactoryUniformShaderParameters>::CreateUniformBufferImmediate(ShaderParameters, UniformBuffer_SingleFrame);
}

void FCubeIndirectVertexFactory::ValidateCompiledResult(const FVertexFactoryType* Type, EShaderPlatform Platform, const FShaderParameterMap& ParameterMap, TArray<FString>& OutErrors)
{

}

void FCubeIndirectVertexFactory::ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FVertexFactory::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	if (RHISupportsManualVertexFetch(Parameters.Platform))
	{
		OutEnvironment.SetDefineIfUnset(TEXT("MANUAL_VERTEX_FETCH"), TEXT("1"));
	}

	const bool bVFSupportsPrimtiveSceneData = Parameters.VertexFactoryType->SupportsPrimitiveIdStream() && UseGPUScene(Parameters.Platform, GetMaxSupportedFeatureLevel(Parameters.Platform));
	OutEnvironment.SetDefine(TEXT("VF_SUPPORTS_PRIMITIVE_SCENE_DATA"), bVFSupportsPrimtiveSceneData);
}

IMPLEMENT_TYPE_LAYOUT(FCubeIndirectVFShaderParameters)
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FCubeIndirectVertexFactory, SF_Vertex, FCubeIndirectVFShaderParameters);
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FCubeIndirectVertexFactoryUniformShaderParameters, "CubeIndirectInstanceVF")

IMPLEMENT_VERTEX_FACTORY_TYPE(FCubeIndirectVertexFactory, "/CubeIndirectShaders/public/CubeIndirectVertexFactory.ush",
	EVertexFactoryFlags::UsedWithMaterials
	| EVertexFactoryFlags::SupportsStaticLighting
	| EVertexFactoryFlags::SupportsDynamicLighting
	| EVertexFactoryFlags::SupportsPrecisePrevWorldPos
	| EVertexFactoryFlags::SupportsPositionOnly
	| EVertexFactoryFlags::SupportsCachingMeshDrawCommands
	//| EVertexFactoryFlags::SupportsPrimitiveIdStream
	| EVertexFactoryFlags::SupportsLightmapBaking
	| EVertexFactoryFlags::SupportsManualVertexFetch
);