#include "ShaderDescriptorSets.h"

namespace nc::graphics
{
	ShaderDescriptorSets::ShaderDescriptorSets()
		: m_descriptorSets{}
	{
		m_descriptorSets.reserve(2);
		m_descriptorSets.emplace(BindFrequency::Infrequent, DescriptorSet{});
		m_descriptorSets.emplace(BindFrequency::PerFrame, DescriptorSet{});
	}

	uint32_t ShaderDescriptorSets::RegisterLayoutBinding(BindFrequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages)
	{

	}

	vk::DescriptorSetLayout* ShaderDescriptorSets::GetDescriptorSetLayout(BindFrequency bindFrequency)
	{
		return &GetSet(bindFrequency)->layout.get();
	}


	void ShaderDescriptorSets::Bind(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t setCount)
	{
		cmd->bindDescriptorSets(bindPoint, pipelineLayout, firstSet, setCount, &GetSet(bindFrequency)->set.get(), 0, 0);
	}

	DescriptorSet* ShaderDescriptorSets::GetSet(BindFrequency bindFrequency)
	{
		return &m_descriptorSets.at(bindFrequency);
	}
}