#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
	enum class BindFrequency : uint8_t
	{
		Infrequent,
		PerFrame
	};

	struct DescriptorSet
	{
		vk::UniqueDescriptorSet set;
		vk::UniqueDescriptorSetLayout layout;
		std::vector<vk::DescriptorSetLayoutBinding> bindings;
	};

	class ShaderDescriptorSets
	{
		public:
			ShaderDescriptorSets();

			uint32_t RegisterLayoutBinding(BindFrequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages);
			vk::DescriptorSetLayout* GetDescriptorSetLayout(BindFrequency bindFrequency);
			void Bind(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t setCount);

		private:
			DescriptorSet* GetSet(BindFrequency bindFrequency);
			std::unordered_map<BindFrequency, DescriptorSet> m_descriptorSets;
	};
}