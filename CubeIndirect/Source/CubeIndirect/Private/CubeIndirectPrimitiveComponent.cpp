 //Fill out your copyright notice in the Description page of Project Settings.


#include "CubeIndirectPrimitiveComponent.h"
#include "CubeIndirectSceneProxy.h"
#include "MaterialDomain.h"
#include "PrimitiveSceneInfo.h"

 FCubeIndirectSceneProxy::FCubeIndirectSceneProxy(const UPrimitiveComponent* Component, int32 InInstanceCount):
	FPrimitiveSceneProxy(Component)
	,VertexFactory(GetScene().GetFeatureLevel(), "FCubeIndirectVertexFactory")
	,InstanceCount(InInstanceCount)
{
	bCastDynamicShadow = true;
	bCastStaticShadow = true;
	// 获取默认材质
	MaterialProxy = UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy();
	//初始化顶点 / 索引 / 顶点工厂
	InitResources();
}

FCubeIndirectSceneProxy::~FCubeIndirectSceneProxy()
{
	 //释放资源
	VertexFactory.ReleaseResource();
	Buffers.ReleaseResources();
}

void FCubeIndirectSceneProxy::InitResources()
{
	ENQUEUE_RENDER_COMMAND(CubeIndirectInitResources)([this](FRHICommandListImmediate& RHICmdList)
	{
		//1) 准备死数据
		const TArray<FVector3f> CubePositions = {
			// Front (+X)
			{ 50, -50, -50 }, { 50, -50, 50 }, { 50, 50, 50 }, { 50, 50, -50 },
			// Back (-X)
			{ -50, -50, 50 }, { -50, -50, -50 }, { -50, 50, -50 }, { -50, 50, 50 },
			// Top (+Z)
			{ -50, 50, 50 }, { 50, 50, 50 }, { 50, -50, 50 }, { -50, -50, 50 },
			// Bottom (-Z)
			{ -50, -50, -50 }, { 50, -50, -50 }, { 50, 50, -50 }, { -50, 50, -50 },
			// Right (+Y)
			{ 50, 50, -50 }, { 50, 50, 50 }, { -50, 50, 50 }, { -50, 50, -50 },
			 //Left (-Y)
			{ -50, -50, -50 }, { -50, -50, 50 }, { 50, -50, 50 }, { 50, -50, -50 },

		};

		 //索引数据：12 个三角形，每三项为一个面
		const TArray<uint16> CubeIndices = {
			0, 1, 2,  0, 2, 3,      // Front
			4, 5, 6,  4, 6, 7,      // Back
			8, 9,10,  8,10,11,      // Top
			12,13,14, 12,14,15,     // Bottom
			16,17,18, 16,18,19,     // Right
			20,21,22, 20,22,23      // Left
		};

		const TArray<FVector3f> CubeNormals = {
			// Front (+X)
		   {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
		   // Back (-X)
		   {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
		   // Top (+Z)
		   {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
		   // Bottom (-Z)
		   {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
		   // Right (+Y)
		   {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0},
		   // Left (-Y)
		   {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0},
		};

		const TArray<FVector4f> CubeTangents = {
			// Front (+X)
			FVector4f{0, 1, 0, 1}, FVector4f{0, 1, 0, 1}, FVector4f{0, 1, 0, 1}, FVector4f{0, 1, 0, 1},
			// Back (-X)
			FVector4f{0, -1, 0, 1}, FVector4f{0, -1, 0, 1}, FVector4f{0, -1, 0, 1}, FVector4f{0, -1, 0, 1},
			 //Top (+Z)
			FVector4f{1, 0, 0, 1}, FVector4f{1, 0, 0, 1}, FVector4f{1, 0, 0, 1}, FVector4f{1, 0, 0, 1},
			 //Bottom (-Z)
			FVector4f{1, 0, 0, 1}, FVector4f{1, 0, 0, 1}, FVector4f{1, 0, 0, 1}, FVector4f{1, 0, 0, 1},
			 //Right (+Y)
			FVector4f{0, 0, 1, 1}, FVector4f{0, 0, 1, 1}, FVector4f{0, 0, 1, 1}, FVector4f{0, 0, 1, 1},
			 //Left (-Y)
			FVector4f{0, 0, -1, 1}, FVector4f{0, 0, -1, 1}, FVector4f{0, 0, -1, 1}, FVector4f{0, 0, -1, 1},
		};

		const TArray < FVector2f> CubeUVs = {
			// Front
		   {1, 1}, {0, 1}, {0, 0}, {1, 0},
		   // Back
		   {1, 1}, {0, 1}, {0, 0}, {1, 0},
		   // Top
		   {0, 1}, {1, 1}, {1, 0}, {0, 0},
		   // Bottom
		   {0, 1}, {1, 1}, {1, 0}, {0, 0},
		    //Right
		   {0, 1}, {1, 1}, {1, 0}, {0, 0},
		   // Left
		   {0, 1}, {1, 1}, {1, 0}, {0, 0},
		};

		// 2) 数据放入buffer
		Buffers.InitRHI(RHICmdList, 24, 36, InstanceCount);
		Buffers.SetIndex(RHICmdList, CubeIndices);
		Buffers.SetPosition(RHICmdList, CubePositions);
		Buffers.SetTexCoord(RHICmdList, CubeUVs);
		Buffers.SetNormal(RHICmdList, CubeNormals);
		Buffers.SetTangent(RHICmdList, CubeTangents);

		// 3) VertexFactory绑定Buffer
		//数据为空但仍然需要绑定，没在Play状态也会执行完整流程，只不过数据不对画不出来
		{
			BindVertexFactoryBuffer(RHICmdList);
		}
	});
}

FPrimitiveViewRelevance FCubeIndirectSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bRenderInMainPass = true;
	Result.bDynamicRelevance = true;
	Result.bShadowRelevance = true;
	Result.bUsesLightingChannels = false;
	Result.bRenderCustomDepth = false;
	return Result;
}

void FCubeIndirectSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	FMeshBatch& Mesh = Collector.AllocateMesh();
	Mesh.MaterialRenderProxy = MaterialProxy;
	Mesh.VertexFactory = &VertexFactory;
	Mesh.Type = PT_TriangleList;
	Mesh.CastShadow = true;
	FMeshBatchElement& BatchElement = Mesh.Elements[0];

	BatchElement.NumPrimitives = 12;
	BatchElement.NumInstances = InstanceCount;
	BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
	BatchElement.MaxVertexIndex = 23;
	BatchElement.MinVertexIndex = 0;
	BatchElement.FirstIndex = 0;
	BatchElement.IndexBuffer = &Buffers.IndexBuffer;
	BatchElement.IndirectArgsBuffer = Buffers.IndirectArgsBuffer;
	BatchElement.NumPrimitives = 0;

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		Collector.AddMesh(ViewIndex, Mesh);
	}
}

void FCubeIndirectSceneProxy::BindVertexFactoryBuffer(FRHICommandList& RHICmdList)
{
	RHICmdList.Transition(FRHITransitionInfo(Buffers.InstanceOriginBuffer, ERHIAccess::UAVCompute, ERHIAccess::SRVGraphics));
	RHICmdList.Transition(FRHITransitionInfo(Buffers.InstanceTransformBuffer, ERHIAccess::UAVCompute, ERHIAccess::SRVGraphics));
	FCubeIndirectVertexFactory::FCubeIndirectDataType Data;
	Buffers.BindPositionVertexBuffer(Data);
	Buffers.BindTexCoordVertexBuffer(Data);
	Buffers.BindNormalVertexBuffer(Data);
	Buffers.BindTangentVertexBuffer(Data);
	Buffers.BindInstanceOriginUAV(Data);
	Buffers.BindInstanceTransformUAV(Data);
	Buffers.BindInstanceOriginSRV(Data);
	Buffers.BindInstanceTransformSRV(Data);
	VertexFactory.SetData(RHICmdList, Data);
	VertexFactory.InitResource(RHICmdList);
}

void FCubeIndirectSceneProxy::DispatchCubeIndirectCS(FRHICommandList& RHICmdList) const
{
	RHICmdList.Transition(FRHITransitionInfo(Buffers.InstanceOriginBuffer, ERHIAccess::Unknown, ERHIAccess::UAVCompute));
	RHICmdList.Transition(FRHITransitionInfo(Buffers.InstanceTransformBuffer, ERHIAccess::Unknown, ERHIAccess::UAVCompute));

	ERHIFeatureLevel::Type FeatureLevel = GetScene().GetFeatureLevel();
	TShaderMapRef<FCubeIndirectCS> CubeIndirectCS(GetGlobalShaderMap(FeatureLevel));
	SetComputePipelineState(RHICmdList, CubeIndirectCS.GetComputeShader());
	FCubeIndirectCS::FParameters Parameters;
	Parameters.IndexCount = 36;
	Parameters.InstanceCount = InstanceCount;
	Parameters.IndirectArgsBuffer = Buffers.IndirectArgsBufferUAV;
	Parameters.InstanceOriginBuffer = Buffers.InstanceOriginBufferUAV;
	Parameters.InstanceTransformBuffer = Buffers.InstanceTransformBufferUAV;
	CubeIndirectCS->SetParameters(RHICmdList, Parameters);
	DispatchComputeShader(RHICmdList, CubeIndirectCS.GetShader(), InstanceCount / 32 + 1, 1, 1);

	//UnBind Resource
	Parameters.IndirectArgsBuffer = nullptr;
	Parameters.InstanceOriginBuffer = nullptr;
	Parameters.InstanceTransformBuffer = nullptr;
	CubeIndirectCS->SetParameters(RHICmdList, Parameters);
}


SIZE_T FCubeIndirectSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

uint32 FCubeIndirectSceneProxy::GetMemoryFootprint() const
{
	return (sizeof(*this) /*+ GetAllocatedSize()*/);
}

void FCubeIndirectSceneProxy::RecreateInstanceBuffer(FRHICommandList& RHICmdList,int32 InInstanceCount)
{
	InstanceCount = InInstanceCount;
	Buffers.RecreateInstanceBuffer(RHICmdList, InstanceCount);
}

FBoxSphereBounds UCubeIndirectPrimitiveComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds NewBounds;
	NewBounds.Origin = LocalToWorld.GetLocation();
	NewBounds.BoxExtent = FVector(5000,5000,5000);
	//FrustumCull中有一步是直接看这个值是否大于0来判定是否被裁掉，所以即使有BoxExtent大于0，这里也要大于0才行
	NewBounds.SphereRadius = 5000.0f;
	return NewBounds;
}

bool UCubeIndirectPrimitiveComponent::ShouldRecreateProxyOnUpdateTransform() const
{
	return false;
}

FPrimitiveSceneProxy* UCubeIndirectPrimitiveComponent::CreateSceneProxy()
{
	if(RHISupportsManualVertexFetch(GMaxRHIShaderPlatform))
	{
		FPrimitiveSceneProxy* CubeSceneProxy = new FCubeIndirectSceneProxy(this, InstanceCount);
		return CubeSceneProxy;
	}
	return nullptr;
}

void UCubeIndirectPrimitiveComponent::OnRegister()
{
	Super::OnRegister();

	MarkRenderDynamicDataDirty();
}

void UCubeIndirectPrimitiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//MarkRenderStateDirty()会重新创建一个新的SceneProxy，只有Buffer数据改变时才用，重新计算只用MarkRenderDynamicDataDirty()，不会摧毁当前SceneProxy
	MarkRenderDynamicDataDirty();
}


void UCubeIndirectPrimitiveComponent::SendRenderDynamicData_Concurrent()
{
	if(SceneProxy)
	{
		FCubeIndirectSceneProxy* CubeIndirectSceneProxy = static_cast<FCubeIndirectSceneProxy*>(SceneProxy);
		ENQUEUE_RENDER_COMMAND(UpdateCubeIndirectSceneProxyData)([this, CubeIndirectSceneProxy](FRHICommandList& RHICmdList)
		{
			CubeIndirectSceneProxy->RecreateInstanceBuffer(RHICmdList, InstanceCount);
			CubeIndirectSceneProxy->DispatchCubeIndirectCS(RHICmdList);
			CubeIndirectSceneProxy->BindVertexFactoryBuffer(RHICmdList);
		});
	}
}

#if WITH_EDITOR
void UCubeIndirectPrimitiveComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UCubeIndirectPrimitiveComponent, InstanceCount))
	{
		// 通知渲染线程更新
		MarkRenderDynamicDataDirty();
	}
}
#endif

