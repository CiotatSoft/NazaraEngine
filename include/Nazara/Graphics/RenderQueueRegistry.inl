// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderQueueRegistry.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void RenderQueueRegistry::Clear()
	{
		m_renderLayerRegistry.clear();
		m_pipelineRegistry.clear();
	}

	inline std::size_t RenderQueueRegistry::FetchLayerIndex(int renderLayer) const
	{
		auto it = m_renderLayerRegistry.find(renderLayer);
		assert(it != m_renderLayerRegistry.end());

		return it->second;
	}

	inline std::size_t RenderQueueRegistry::FetchPipelineIndex(const RenderPipeline* pipeline) const
	{
		auto it = m_pipelineRegistry.find(pipeline);
		assert(it != m_pipelineRegistry.end());

		return it->second;
	}

	inline std::size_t RenderQueueRegistry::FetchVertexBuffer(const AbstractBuffer* vertexBuffer) const
	{
		auto it = m_vertexBufferRegistry.find(vertexBuffer);
		assert(it != m_vertexBufferRegistry.end());

		return it->second;
	}

	inline std::size_t RenderQueueRegistry::FetchVertexDeclaration(const VertexDeclaration* vertexDeclaration) const
	{
		auto it = m_vertexDeclarationRegistry.find(vertexDeclaration);
		assert(it != m_vertexDeclarationRegistry.end());

		return it->second;
	}

	inline void RenderQueueRegistry::RegisterLayer(int renderLayer)
	{
		m_renderLayerRegistry.try_emplace(renderLayer, m_renderLayerRegistry.size());
	}

	inline void RenderQueueRegistry::RegisterPipeline(const RenderPipeline* pipeline)
	{
		m_pipelineRegistry.try_emplace(pipeline, m_pipelineRegistry.size());
	}

	inline void RenderQueueRegistry::RegisterVertexBuffer(const AbstractBuffer* vertexBuffer)
	{
		m_vertexBufferRegistry.try_emplace(vertexBuffer, m_vertexBufferRegistry.size());
	}

	inline void RenderQueueRegistry::RegisterVertexDeclaration(const VertexDeclaration* vertexDeclaration)
	{
		m_vertexDeclarationRegistry.try_emplace(vertexDeclaration, m_vertexDeclarationRegistry.size());
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
