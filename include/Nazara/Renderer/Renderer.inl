// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RendererImpl* Renderer::GetRendererImpl()
	{
		return m_rendererImpl.get();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
