# Vulkan Hardware Requirements
Credit goes to [vulkan.gpuinfo.org](https://vulkan.gpuinfo.org/listdevices.php).

Per Sascha Willems: Data is based on reports submitted or updated with version 3.0 or newer of the Hardware Capability Viewer (January 2021) and does not contain reports from earlier versions.

Table data gathered March 2024.

## Feature Support - Windows and Linux

| Feature | Windows Adoption % |Linux Adoption % |
|----------------------------------------------|--------|-------|
| fillModeNonSolid |99.76% |99.72% |
| samplerAnisotropy |99.18% |99.72% |
| sampleRateShading |99.06% |99.72% |
| descriptorBindingPartiallyBound |94.44% |91.11% |
| descriptorBindingSampledImageUpdateAfterBind |96.82% |91.11% |
| descriptorBindingStorageBufferUpdateAfterBind|96.82% |91.11% |
| descriptorBindingUniformBufferUpdateAfterBind|70.27% |72.82% |
| descriptorIndexing |96.82% |90.10% |
| runtimeDescriptorArray |96.82% |90.10% |
| shaderSampledImageArrayNonUniformIndexing |96.82% |90.10% |
| shaderStorageBufferArrayNonUniformIndexing |96.82% |90.94% |
| shaderUniformBufferArrayNonUniformIndexing |96.18% |89.43% |

## Property Maximums - Windows and Linux
| Property | Maximum | Windows Adoption % | Linux Adoption % |
|--|--|--|--|
| MaxBoundDescriptorSets | 8 | 96.9% | 99.4% |
| MaxDescriptorSetSampledImages | 256 | 99.8% | 99.1% |
| MaxDescriptorSetSamplers | 576 | 97.6% | 94.8% |
| MaxDescriptorSetStorageBuffers | 96 | 99.8% | 99.7% |
| MaxDescriptorSetUniformBuffers | 72 | 100.0% | 100.0% |
| MaxDescriptorSetUpdateAfterBindSampledImages | 65536 | 100.0% | 99.4% |
| MaxDescriptorSetUpdateAfterBindSamplers | 65536 | 100.0% | 99.4% |
| MaxDescriptorSetUpdateAfterBindStorageBuffers | 65536 | 100.0% | 99.4% |
| MaxDescriptorSetUpdateAfterBindUniformBuffers | 90 | 100.0% | 99.7% |
| MaxPerSetDescriptors | 1024 | 100.0% | 100.0% |
| MaxPerStageDescriptorSampledImages | 128 | 100.0% | 97.0% |
| MaxPerStageDescriptorSamplers | 32 | 99.3% | 97.7% |
| MaxPerStageDescriptorStorageBuffers | 16 | 99.9% | 99.8% |
| MaxPerStageDescriptorUniformBuffers | 12 | 100.0% | 100.0% |
| MaxPerStageDescriptorUpdateAfterBindSampledImages | 65536 | 100.0% | 99.4% |
| MaxPerStageDescriptorUpdateAfterBindSamplers | 65536 | 100.0% | 99.4% |
| MaxPerStageDescriptorUpdateAfterBindStorageBuffers | 65536 | 100.0% | 99.4% |
| MaxPerStageDescriptorUpdateAfterBindUniformBuffers | 15 | 100.0% | 99.7% |
| MaxPerStageUpdateAfterBindResources | 65536 | 100.0% | 99.4% |
| MaxPerStageResources | 128 | 100.0% | 100.0% |
| MaxPushConstantsSize | 128 | 100.0% | 100.0% |
| MaxVertexInputBindings | 28 | 95.8% | 99.1% |
| MaxUpdateAfterBindDescriptorsInAllPools | 1000000 | 97.9% | 99.0% |