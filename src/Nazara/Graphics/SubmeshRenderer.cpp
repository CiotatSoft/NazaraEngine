// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SubmeshRenderer.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderFrame.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	RenderElementPool<RenderSubmesh>& SubmeshRenderer::GetPool()
	{
		return m_submeshPool;
	}

	std::unique_ptr<ElementRendererData> SubmeshRenderer::InstanciateData()
	{
		return std::make_unique<SubmeshRendererData>();
	}

	void SubmeshRenderer::Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderFrame& /*currentFrame*/, std::size_t elementCount, const Pointer<const RenderElement>* elements, const RenderStates* renderStates)
	{
		Graphics* graphics = Graphics::Instance();

		auto& data = static_cast<SubmeshRendererData&>(rendererData);

		Recti invalidScissorBox(-1, -1, -1, -1);

		const RenderBuffer* currentIndexBuffer = nullptr;
		const RenderBuffer* currentVertexBuffer = nullptr;
		const MaterialInstance* currentMaterialInstance = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentShaderBinding = nullptr;
		const SkeletonInstance* currentSkeletonInstance = nullptr;
		const WorldInstance* currentWorldInstance = nullptr;
		Recti currentScissorBox = invalidScissorBox;
		RenderBufferView currentLightData;

		auto FlushDrawCall = [&]()
		{
			// Does nothing for now (but will serve once instancing is implemented)
		};

		auto FlushDrawData = [&]()
		{
			FlushDrawCall();

			currentShaderBinding = nullptr;
		};

		const auto& whiteTexture = Graphics::Instance()->GetDefaultTextures().whiteTextures[UnderlyingCast(ImageType::E2D)];
		const auto& defaultSampler = graphics->GetSamplerCache().Get({});

		std::size_t oldDrawCallCount = data.drawCalls.size();

		for (std::size_t i = 0; i < elementCount; ++i)
		{
			assert(elements[i]->GetElementType() == UnderlyingCast(BasicRenderElement::Submesh));
			const RenderSubmesh& submesh = static_cast<const RenderSubmesh&>(*elements[i]);
			const RenderStates& renderState = renderStates[i];

			if (const RenderPipeline* pipeline = submesh.GetRenderPipeline(); currentPipeline != pipeline)
			{
				FlushDrawCall();
				currentPipeline = pipeline;
			}

			if (const MaterialInstance* materialInstance = &submesh.GetMaterialInstance(); currentMaterialInstance != materialInstance)
			{
				FlushDrawData();
				currentMaterialInstance = materialInstance;
			}

			if (const RenderBuffer* indexBuffer = submesh.GetIndexBuffer(); currentIndexBuffer != indexBuffer)
			{
				FlushDrawCall();
				currentIndexBuffer = indexBuffer;
			}

			if (const RenderBuffer* vertexBuffer = submesh.GetVertexBuffer(); currentVertexBuffer != vertexBuffer)
			{
				FlushDrawCall();
				currentVertexBuffer = vertexBuffer;
			}

			if (const SkeletonInstance* skeletonInstance = submesh.GetSkeletonInstance(); currentSkeletonInstance != skeletonInstance)
			{
				FlushDrawData();
				currentSkeletonInstance = skeletonInstance;
			}

			if (const WorldInstance* worldInstance = &submesh.GetWorldInstance(); currentWorldInstance != worldInstance)
			{
				// TODO: Flushing draw calls on instance binding means we can have e.g. 1000 sprites rendered using a draw call for each one
				// which is far from being efficient, using some bindless could help (or at least instancing?)
				FlushDrawData();
				currentWorldInstance = worldInstance;
			}

			if (currentLightData != renderState.lightData)
			{
				FlushDrawData();
				currentLightData = renderState.lightData;
			}

			const Recti& scissorBox = submesh.GetScissorBox();
			const Recti& targetScissorBox = (scissorBox.width >= 0) ? scissorBox : invalidScissorBox;
			if (currentScissorBox != targetScissorBox)
			{
				FlushDrawCall();
				currentScissorBox = targetScissorBox;
			}

			if (!currentShaderBinding)
			{
				assert(currentMaterialInstance);

				m_bindingCache.clear();
				currentMaterialInstance->FillShaderBinding(m_bindingCache);

				const Material& material = *currentMaterialInstance->GetParentMaterial();

				// Predefined shader bindings
				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::InstanceDataUbo); bindingIndex != Material::InvalidBindingIndex)
				{
					assert(currentWorldInstance);
					const auto& instanceBuffer = currentWorldInstance->GetInstanceBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						instanceBuffer.get(),
						0, instanceBuffer->GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::LightDataUbo); bindingIndex != Material::InvalidBindingIndex && currentLightData)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						currentLightData.GetBuffer(),
						currentLightData.GetOffset(), currentLightData.GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::SkeletalDataUbo); bindingIndex != Material::InvalidBindingIndex && currentSkeletonInstance)
				{
					const auto& skeletalBuffer = currentSkeletonInstance->GetSkeletalBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						skeletalBuffer.get(),
						0, skeletalBuffer->GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::ViewerDataUbo); bindingIndex != Material::InvalidBindingIndex)
				{
					const auto& viewerBuffer = viewerInstance.GetViewerBuffer();

					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::UniformBufferBinding{
						viewerBuffer.get(),
						0, viewerBuffer->GetSize()
					};
				}

				if (UInt32 bindingIndex = material.GetEngineBindingIndex(EngineShaderBinding::OverlayTexture); bindingIndex != Material::InvalidBindingIndex)
				{
					auto& bindingEntry = m_bindingCache.emplace_back();
					bindingEntry.bindingIndex = bindingIndex;
					bindingEntry.content = ShaderBinding::TextureBinding{
						whiteTexture.get(), defaultSampler.get()
					};
				}

				assert(currentPipeline);
				ShaderBindingPtr drawDataBinding = currentPipeline->GetPipelineInfo().pipelineLayout->AllocateShaderBinding(0);
				drawDataBinding->Update(m_bindingCache.data(), m_bindingCache.size());

				currentShaderBinding = drawDataBinding.get();

				data.shaderBindings.emplace_back(std::move(drawDataBinding));
			}

			auto& drawCall = data.drawCalls.emplace_back();
			drawCall.firstIndex = 0;
			drawCall.indexBuffer = currentIndexBuffer;
			drawCall.indexCount = submesh.GetIndexCount();
			drawCall.indexType = submesh.GetIndexType();
			drawCall.renderPipeline = currentPipeline;
			drawCall.scissorBox = currentScissorBox;
			drawCall.shaderBinding = currentShaderBinding;
			drawCall.vertexBuffer = currentVertexBuffer;
		}

		const RenderSubmesh* firstSubmesh = static_cast<const RenderSubmesh*>(elements[0]);
		std::size_t drawCallCount = data.drawCalls.size() - oldDrawCallCount;
		data.drawCallPerElement[firstSubmesh] = SubmeshRendererData::DrawCallIndices{ oldDrawCallCount, drawCallCount };
	}

	void SubmeshRenderer::Render(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, std::size_t /*elementCount*/, const Pointer<const RenderElement>* elements)
	{
		auto& data = static_cast<SubmeshRendererData&>(rendererData);

		Vector2f targetSize = viewerInstance.GetTargetSize();
		Recti fullscreenScissorBox(0, 0, SafeCast<int>(std::floor(targetSize.x)), SafeCast<int>(std::floor(targetSize.y)));

		const RenderBuffer* currentIndexBuffer = nullptr;
		const RenderBuffer* currentVertexBuffer = nullptr;
		const RenderPipeline* currentPipeline = nullptr;
		const ShaderBinding* currentShaderBinding = nullptr;
		Recti currentScissorBox(-1, -1, -1, -1);

		const RenderSubmesh* firstSubmesh = static_cast<const RenderSubmesh*>(elements[0]);
		auto it = data.drawCallPerElement.find(firstSubmesh);
		assert(it != data.drawCallPerElement.end());

		const auto& indices = it->second;

		for (std::size_t i = 0; i < indices.count; ++i)
		{
			const auto& drawData = data.drawCalls[indices.start + i];

			if (currentPipeline != drawData.renderPipeline)
			{
				commandBuffer.BindPipeline(*drawData.renderPipeline);
				currentPipeline = drawData.renderPipeline;
			}

			if (currentShaderBinding != drawData.shaderBinding)
			{
				commandBuffer.BindShaderBinding(0, *drawData.shaderBinding);
				currentShaderBinding = drawData.shaderBinding;
			}

			if (currentIndexBuffer != drawData.indexBuffer)
			{
				if (drawData.indexBuffer)
					commandBuffer.BindIndexBuffer(*drawData.indexBuffer, drawData.indexType);

				currentIndexBuffer = drawData.indexBuffer;
			}

			if (currentVertexBuffer != drawData.vertexBuffer)
			{
				commandBuffer.BindVertexBuffer(0, *drawData.vertexBuffer);
				currentVertexBuffer = drawData.vertexBuffer;
			}

			const Recti& targetScissorBox = (drawData.scissorBox.width >= 0) ? drawData.scissorBox : fullscreenScissorBox;
			if (currentScissorBox != targetScissorBox)
			{
				commandBuffer.SetScissor(targetScissorBox);
				currentScissorBox = targetScissorBox;
			}

			if (currentIndexBuffer)
				commandBuffer.DrawIndexed(SafeCast<UInt32>(drawData.indexCount), 1U, SafeCast<UInt32>(drawData.firstIndex));
			else
				commandBuffer.Draw(SafeCast<UInt32>(drawData.indexCount), 1U, SafeCast<UInt32>(drawData.firstIndex));
		}
	}

	void SubmeshRenderer::Reset(ElementRendererData& rendererData, RenderFrame& currentFrame)
	{
		auto& data = static_cast<SubmeshRendererData&>(rendererData);

		for (auto& shaderBinding : data.shaderBindings)
			currentFrame.PushForRelease(std::move(shaderBinding));
		data.shaderBindings.clear();

		data.drawCalls.clear();
	}
}
