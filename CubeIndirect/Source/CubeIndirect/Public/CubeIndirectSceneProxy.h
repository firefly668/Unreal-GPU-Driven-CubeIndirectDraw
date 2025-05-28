#pragma once

#include "PrimitiveSceneProxy.h"
#include "Engine/InstancedStaticMesh.h"
#include "CubeIndirectVertexFactory.h"
#include "RHIDiagnosticBuffer.h"
#include "CubeIndirectComputeShader.h"
//class FStaticMeshInstanceBuffer;

struct FCubeIndirectBuffer
{
	int32 VertexCount;
	int32 IndexCount;
	int32 InstanceCount;

	FIndexBuffer IndexBuffer;

	FVertexBuffer PositionBuffer;

	FVertexBuffer TexCoordBuffer;

	FVertexBuffer NormalBuffer;

	FVertexBuffer TangentBuffer;

	FBufferRHIRef InstanceOriginBuffer;
	FUnorderedAccessViewRHIRef InstanceOriginBufferUAV;
	FRHIShaderResourceViewRef InstanceOriginBufferSRV;

	FBufferRHIRef InstanceTransformBuffer;
	FUnorderedAccessViewRHIRef InstanceTransformBufferUAV;
	FRHIShaderResourceViewRef InstanceTransformBufferSRV;

	FBufferRHIRef IndirectArgsBuffer;
	FUnorderedAccessViewRHIRef IndirectArgsBufferUAV;

	

	void InitRHI(FRHICommandList& RHICmdList, int32 InVertexCount, int32 InIndexCount, int32 InInstanceCount)
	{
		VertexCount = InVertexCount;
		IndexCount = InIndexCount;
		FRHIResourceCreateInfo IndexBufferCreateInfo(TEXT("CubeIndirectIndexBuffer"));
		IndexBuffer.IndexBufferRHI = RHICmdList.CreateIndexBuffer(sizeof(uint16), sizeof(uint16) * IndexCount, BUF_Static, IndexBufferCreateInfo);
		IndexBuffer.InitResource(RHICmdList);

		FRHIResourceCreateInfo PositionBufferCreateInfo(TEXT("CubeIndirectPositionBuffer"));
		PositionBuffer.VertexBufferRHI = RHICmdList.CreateBuffer(sizeof(FVector3f) * VertexCount,  BUF_Static | BUF_VertexBuffer, sizeof(FVector3f), ERHIAccess::VertexOrIndexBuffer | ERHIAccess::SRVMask, PositionBufferCreateInfo);
		PositionBuffer.InitResource(RHICmdList);

		FRHIResourceCreateInfo TexCoordBufferCreateInfo(TEXT("CubeIndirectTexCoordBuffer"));
		TexCoordBuffer.VertexBufferRHI = RHICmdList.CreateVertexBuffer(sizeof(FVector2f) * VertexCount, BUF_Static, TexCoordBufferCreateInfo);
		TexCoordBuffer.InitResource(RHICmdList);

		FRHIResourceCreateInfo NormalBufferCreateInfo(TEXT("CubeIndirectNormalBuffer"));
		//RHICmdList.CreateVertexBuffer有坑，内部将Stride定为0，而此情况下Alignment固定为16，FVector3f的属性就会Buffer报错，指定Stride后Alignment为Stride与16的最小公因数（还有其他条件，具体看源码）
		NormalBuffer.VertexBufferRHI = RHICmdList.CreateBuffer(sizeof(FVector3f) * VertexCount, BUF_Static | BUF_VertexBuffer, sizeof(FVector3f), ERHIAccess::VertexOrIndexBuffer | ERHIAccess::SRVMask, NormalBufferCreateInfo);
		NormalBuffer.InitResource(RHICmdList);

		FRHIResourceCreateInfo TangentBufferCreateInfo(TEXT("CubeIndirectTangentBuffer"));
		TangentBuffer.VertexBufferRHI = RHICmdList.CreateVertexBuffer(sizeof(FVector4f) * VertexCount, BUF_Static, TangentBufferCreateInfo);
		TangentBuffer.InitResource(RHICmdList);

		CreateInstanceBuffer(RHICmdList, InInstanceCount);

		FRHIResourceCreateInfo IndirectArgsBufferCreateInfo(TEXT("CubeIndirectIndirectArgsBuffer"));
		IndirectArgsBuffer = RHICmdList.CreateBuffer(sizeof(uint32) * 5, BUF_Static | BUF_UnorderedAccess | BUF_StructuredBuffer, sizeof(uint32), ERHIAccess::UAVMask, IndirectArgsBufferCreateInfo);
		IndirectArgsBufferUAV = RHICmdList.CreateUnorderedAccessView(IndirectArgsBuffer, FRHIViewDesc::CreateBufferUAV()
			.SetType(FRHIViewDesc::EBufferType::Structured)
			.SetAtomicCounter(false)
			.SetAppendBuffer(false)
		);
	}

	void CreateInstanceBuffer(FRHICommandList& RHICmdList, int32 InInstanceCount)
	{
		InstanceCount = InInstanceCount;

		FRHIResourceCreateInfo InstanceOriginBufferCreateInfo(TEXT("CubeIndirectInstanceOriginBuffer"));
		InstanceOriginBuffer = RHICmdList.CreateBuffer(sizeof(FVector4f) * InstanceCount, BUF_Static | BUF_UnorderedAccess | BUF_StructuredBuffer | BUF_ShaderResource, sizeof(FVector4f), ERHIAccess::UAVCompute, InstanceOriginBufferCreateInfo);
		InstanceOriginBufferSRV = RHICmdList.CreateShaderResourceView(InstanceOriginBuffer, FRHIViewDesc::CreateBufferSRV()
			.SetType(FRHIViewDesc::EBufferType::Structured)
			.SetStride(sizeof(FVector4f)));
		InstanceOriginBufferUAV = RHICmdList.CreateUnorderedAccessView(InstanceOriginBuffer, FRHIViewDesc::CreateBufferUAV()
			.SetType(FRHIViewDesc::EBufferType::Structured)
			.SetAtomicCounter(false)
			.SetAppendBuffer(false)
		);

		FRHIResourceCreateInfo InstanceTransformBufferCreateInfo(TEXT("CubeIndirectInstanceTransformBuffer"));
		InstanceTransformBuffer = RHICmdList.CreateBuffer(sizeof(FVector4f) * InstanceCount * 3, BUF_Static | BUF_UnorderedAccess | BUF_StructuredBuffer | BUF_ShaderResource, sizeof(FVector4f), ERHIAccess::UAVCompute, InstanceTransformBufferCreateInfo);
		InstanceTransformBufferSRV = RHICmdList.CreateShaderResourceView(InstanceTransformBuffer, FRHIViewDesc::CreateBufferSRV()
			.SetType(FRHIViewDesc::EBufferType::Structured)
			.SetStride(sizeof(FVector4f)));
		InstanceTransformBufferUAV = RHICmdList.CreateUnorderedAccessView(InstanceTransformBuffer, FRHIViewDesc::CreateBufferUAV()
			.SetType(FRHIViewDesc::EBufferType::Structured)
			.SetAtomicCounter(false)
			.SetAppendBuffer(false)
		);
	}

	void RecreateInstanceBuffer(FRHICommandList& RHICmdList, int32 InInstanceCount)
	{
		ReleaseInstanceBuffer();
		CreateInstanceBuffer(RHICmdList, InInstanceCount);
	}

	void ReleaseResources()
	{
		InstanceOriginBufferSRV.SafeRelease();
		InstanceTransformBufferSRV.SafeRelease();
		InstanceOriginBufferUAV.SafeRelease();
		InstanceTransformBufferUAV.SafeRelease();
		IndirectArgsBufferUAV.SafeRelease();
		IndexBuffer.ReleaseResource();
		PositionBuffer.ReleaseResource();
		TexCoordBuffer.ReleaseResource();
		NormalBuffer.ReleaseResource();
		TangentBuffer.ReleaseResource();
		InstanceOriginBuffer.SafeRelease();
		InstanceTransformBuffer.SafeRelease();
		IndirectArgsBuffer.SafeRelease();
	}

	void ReleaseInstanceBuffer()
	{
		InstanceOriginBuffer.SafeRelease();
		InstanceTransformBuffer.SafeRelease();
		InstanceOriginBufferSRV.SafeRelease();
		InstanceOriginBufferUAV.SafeRelease();
		InstanceTransformBufferSRV.SafeRelease();
		InstanceTransformBufferUAV.SafeRelease();
	}

	void SetIndex(FRHICommandListBase& RHICmdList, TArray<uint16> InIndexs) const
	{
		int32 Size = sizeof(uint16) * IndexCount;
		void* BufferData = RHICmdList.LockBuffer(IndexBuffer.IndexBufferRHI, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, InIndexs.GetData(), Size);
		RHICmdList.UnlockBuffer(IndexBuffer.IndexBufferRHI);
	}

	void SetPosition(FRHICommandListBase& RHICmdList, TArray<FVector3f> InPositions) const
	{
		int32 Size = sizeof(FVector3f) * VertexCount;
		void* BufferData = RHICmdList.LockBuffer(PositionBuffer.VertexBufferRHI, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, InPositions.GetData(), Size);
		RHICmdList.UnlockBuffer(PositionBuffer.VertexBufferRHI);
	}

	void SetTexCoord(FRHICommandListBase& RHICmdList, TArray<FVector2f> InTexCoords) const
	{
		int32 Size = sizeof(FVector2f) * VertexCount;
		void* BufferData = RHICmdList.LockBuffer(TexCoordBuffer.VertexBufferRHI, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, InTexCoords.GetData(), Size);
		RHICmdList.UnlockBuffer(TexCoordBuffer.VertexBufferRHI);
	}

	void SetNormal(FRHICommandListBase& RHICmdList, TArray<FVector3f> InNormals) const
	{
		int32 Size = sizeof(FVector3f) * VertexCount;
		void* BufferData = RHICmdList.LockBuffer(NormalBuffer.VertexBufferRHI, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, InNormals.GetData(), Size);
		RHICmdList.UnlockBuffer(NormalBuffer.VertexBufferRHI);
	}

	void SetTangent(FRHICommandListBase& RHICmdList, TArray<FVector4f> InTangents) const
	{
		int32 Size = sizeof(FVector4f) * VertexCount;
		void* BufferData = RHICmdList.LockBuffer(TangentBuffer.VertexBufferRHI, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, InTangents.GetData(), Size);
		RHICmdList.UnlockBuffer(TangentBuffer.VertexBufferRHI);
	}

	void SetInstanceOrigin(FRHICommandListBase& RHICmdList, TArray<FVector4f> InInstanceOrigins) const
	{
		int32 Size = sizeof(FVector4f) * InstanceCount;
		void* BufferData = RHICmdList.LockBuffer(InstanceOriginBuffer, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, InInstanceOrigins.GetData(), Size);
		RHICmdList.UnlockBuffer(InstanceOriginBuffer);
	}

	void SetInstanceTransform(FRHICommandListBase& RHICmdList, TArray<FVector4f> InInstanceTransforms) const
	{
		int32 Size = sizeof(FVector4f) * InstanceCount * 3;
		void* BufferData = RHICmdList.LockBuffer(InstanceTransformBuffer, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, InInstanceTransforms.GetData(), Size);
		RHICmdList.UnlockBuffer(InstanceTransformBuffer);
	}

	void SetIndirectArgs(FRHICommandListBase& RHICmdList, TArray<uint32> IndirectArgs) const
	{
		int32 Size = sizeof(uint32) * 5;
		void* BufferData = RHICmdList.LockBuffer(IndirectArgsBuffer, 0, Size, RLM_WriteOnly);
		FMemory::Memcpy(BufferData, IndirectArgs.GetData(), Size);
		RHICmdList.UnlockBuffer(IndirectArgsBuffer);
	}

	void BindPositionVertexBuffer(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data) const
	{
		Data.PositionComponent = FVertexStreamComponent(
			&PositionBuffer,
			0,
			sizeof(FVector3f),
			VET_Float3,
			//EVertexStreamUsage::ManualFetch会导致VertexStream不会传递到FMeshDrawCommand，毕竟你都想通过塞入buffer自己获取了，人家就不给你传了
			//EVertexStreamUsage::Instancing让你的VertexStream变成PER_INSTANCE，而不是PER_VERTEX。PER_INSTANCE指一个数据给一个实例用，用renderdoc能看到该类型，并且第一个实例所有顶点数据都是第一个顶点数据的复制；PER_VERTEX指一个数据给一个顶点用
			EVertexStreamUsage::Default
		);
	}

	void BindTexCoordVertexBuffer(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data) const
	{
		Data.TextureCoordinateComponent = FVertexStreamComponent(
			&TexCoordBuffer,
			0,
			sizeof(FVector2f),
			VET_Float2,
			EVertexStreamUsage::Default
		);
	}

	void BindNormalVertexBuffer(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data) const
	{
		Data.NormalComponent = FVertexStreamComponent(
			&NormalBuffer,
			0,
			sizeof(FVector3f),
			VET_Float3,
			EVertexStreamUsage::Default
		);
	}

	void BindTangentVertexBuffer(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data) const
	{
		Data.TangentComponent = FVertexStreamComponent(
			&TangentBuffer,
			0,
			sizeof(FVector4f),
			VET_Float4,
			EVertexStreamUsage::Default
		);
	}

	void BindInstanceOriginUAV(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data) const
	{
		Data.InstanceOriginUAV = InstanceOriginBufferUAV;
	}

	void BindInstanceTransformUAV(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data) const
	{
		Data.InstanceTransformUAV = InstanceTransformBufferUAV;
	}

	void BindInstanceOriginSRV(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data)
	{
		Data.InstanceOriginSRV = InstanceOriginBufferSRV;
	}

	void BindInstanceTransformSRV(FCubeIndirectVertexFactory::FCubeIndirectDataType& Data)
	{
		Data.InstanceTransformSRV = InstanceTransformBufferSRV;
	}
};


class FCubeIndirectSceneProxy : public FPrimitiveSceneProxy
{
public:
	CUBEINDIRECT_API virtual SIZE_T GetTypeHash() const override;
	CUBEINDIRECT_API virtual uint32 GetMemoryFootprint() const override;
	CUBEINDIRECT_API virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	CUBEINDIRECT_API FCubeIndirectSceneProxy(const UPrimitiveComponent* Component, int32 InstanceCount);
	CUBEINDIRECT_API ~FCubeIndirectSceneProxy();
	CUBEINDIRECT_API virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
	void DispatchCubeIndirectCS(FRHICommandList& RHICmdList) const;
	void BindVertexFactoryBuffer(FRHICommandList& RHICmdList);
	void RecreateInstanceBuffer(FRHICommandList& RHICmdList, int32 InInstanceCount);
private:

	int32 InstanceCount;

	FCubeIndirectBuffer Buffers;

	FCubeIndirectVertexFactory VertexFactory;
	
	FMaterialRenderProxy* MaterialProxy;

	void InitResources();
};
