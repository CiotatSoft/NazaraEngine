// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_DEVICEOBJECT_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_DEVICEOBJECT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/Utils/MovablePtr.hpp>
#include <vulkan/vulkan_core.h>
#include <string>

namespace Nz::Vk
{
	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	class DeviceObject
	{
		public:
			DeviceObject();
			DeviceObject(const DeviceObject&) = delete;
			DeviceObject(DeviceObject&& object) noexcept;
			~DeviceObject();

			bool Create(Device& device, const CreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
			void Destroy();

			bool IsValid() const;

			Device* GetDevice() const;
			VkResult GetLastErrorCode() const;

			void SetDebugName(const char* name);
			void SetDebugName(const std::string& name);

			DeviceObject& operator=(const DeviceObject&) = delete;
			DeviceObject& operator=(DeviceObject&& object) noexcept;

			operator VkType() const;

		protected:
			MovablePtr<Device> m_device;
			VkAllocationCallbacks m_allocator;
			VkType m_handle;
			mutable VkResult m_lastErrorCode;
	};
}

#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_DEVICEOBJECT_HPP
