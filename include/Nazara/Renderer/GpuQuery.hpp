// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GPUQUERY_HPP
#define NAZARA_GPUQUERY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>

class NAZARA_RENDERER_API NzGpuQuery
{
	public:
		NzGpuQuery();
		NzGpuQuery(const NzGpuQuery&) = delete;
		NzGpuQuery(NzGpuQuery&&) = delete; ///TODO
		~NzGpuQuery();

		void Begin(nzGpuQueryMode mode);
		void End();

		unsigned int GetResult() const;

		bool IsResultAvailable() const;

		// Fonctions OpenGL
		unsigned int GetOpenGLID() const;

		NzGpuQuery& operator=(const NzGpuQuery&) = delete;
		NzGpuQuery& operator=(NzGpuQuery&&) = delete; ///TODO

		static bool IsModeSupported(nzGpuQueryMode mode);
		static bool IsSupported();

	private:
		nzGpuQueryMode m_mode;
		unsigned int m_id;
};

#endif // NAZARA_GPUQUERY_HPP
