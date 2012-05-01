// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERIMPL_HPP
#define NAZARA_SHADERIMPL_HPP

#include <Nazara/Renderer/Shader.hpp>

class NzShaderImpl
{
	public:
		NzShaderImpl() = default;
		virtual ~NzShaderImpl();

		virtual bool Bind() = 0;

		virtual bool Compile() = 0;
		virtual bool Create() = 0;

		virtual void Destroy() = 0;

		virtual NzString GetLog() const = 0;
		virtual nzShaderLanguage GetLanguage() const = 0;
		virtual NzString GetSourceCode(nzShaderType type) const = 0;

		virtual bool IsLoaded(nzShaderType type) const = 0;

		virtual bool Load(nzShaderType type, const NzString& source) = 0;

		virtual bool SendBoolean(const NzString& name, bool value) = 0;
		virtual bool SendDouble(const NzString& name, double value) = 0;
		virtual bool SendFloat(const NzString& name, float value) = 0;
		virtual bool SendInteger(const NzString& name, int value) = 0;
		virtual bool SendMatrix(const NzString& name, const NzMatrix4d& matrix) = 0;
		virtual bool SendMatrix(const NzString& name, const NzMatrix4f& matrix) = 0;

		virtual void Unbind() = 0;
};

#endif // NAZARA_SHADERIMPL_HPP
