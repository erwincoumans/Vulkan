/**
* VKTS Examples - Examples for Vulkan using VulKan ToolS.
*
* The MIT License (MIT)
*
* Copyright (c) since 2014 Norbert Nopper
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/


#include "Example.hpp"

Example::Example(const vkts::IContextObjectSP& contextObject, const int32_t windowIndex, const vkts::IVisualContextSP& visualContext, const vkts::ISurfaceSP& surface) :
	IUpdateThread(), contextObject(contextObject), windowIndex(windowIndex), visualContext(visualContext), surface(surface), commandPool(nullptr), imageAcquiredSemaphore(nullptr), renderingCompleteSemaphore(nullptr), descriptorSetLayout(nullptr), vertexViewProjectionUniformBuffer(nullptr), fragmentUniformBuffer(nullptr), vertexShaderModule(nullptr), fragmentShaderModule(nullptr), pipelineLayout(nullptr), renderFactory(nullptr), sceneManager(nullptr), sceneFactory(nullptr), scene(nullptr), swapchain(nullptr), renderPass(nullptr), allGraphicsPipelines(), depthTexture(nullptr), depthStencilImageView(nullptr), swapchainImagesCount(0), swapchainImageView(), framebuffer(), cmdBuffer(), cmdBufferFence()
{
}

Example::~Example()
{
}

VkBool32 Example::buildCmdBuffer(const int32_t usedBuffer)
{
	VkResult result;

	cmdBuffer[usedBuffer] = vkts::commandBuffersCreate(contextObject->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
	VKTS_VALIDATE_INSTANCE(cmdBuffer[usedBuffer], "Could not create command buffer.");

	result = cmdBuffer[usedBuffer]->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
	VKTS_VALIDATE_SUCCESS(result, "Could not begin command buffer.");

	//

	swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, usedBuffer);

	//

	VkClearColorValue clearColorValue{};

	clearColorValue.float32[0] = 0.2f;
	clearColorValue.float32[1] = 0.2f;
	clearColorValue.float32[2] = 0.2f;
	clearColorValue.float32[3] = 1.0f;

	VkClearDepthStencilValue clearDepthStencilValue{};

	clearDepthStencilValue.depth   = 1.0f;
	clearDepthStencilValue.stencil = 0;

	VkClearValue clearValues[2]{};

	clearValues[0].color        = clearColorValue;
	clearValues[1].depthStencil = clearDepthStencilValue;

	VkRenderPassBeginInfo renderPassBeginInfo{};

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderPassBeginInfo.renderPass          = renderPass->getRenderPass();
	renderPassBeginInfo.framebuffer         = framebuffer[usedBuffer]->getFramebuffer();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent   = swapchain->getImageExtent();
	renderPassBeginInfo.clearValueCount     = 2;
	renderPassBeginInfo.pClearValues        = clearValues;

	cmdBuffer[usedBuffer]->cmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};

	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = (float)swapchain->getImageExtent().width;
	viewport.height   = (float)swapchain->getImageExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vkCmdSetViewport(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor{};

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent   = swapchain->getImageExtent();

	vkCmdSetScissor(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &scissor);

	if (scene.get())
	{
		scene->drawRecursive(cmdBuffer[usedBuffer], allGraphicsPipelines, usedBuffer, dynamicOffsets);
	}

	cmdBuffer[usedBuffer]->cmdEndRenderPass();

	//

	swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, usedBuffer);

	//

	result = cmdBuffer[usedBuffer]->endCommandBuffer();
	VKTS_VALIDATE_SUCCESS(result, "Could not end command buffer.");

	return VK_TRUE;
}

VkBool32 Example::buildFramebuffer(const int32_t usedBuffer)
{
	VkImageView imageViews[2];

	imageViews[0] = swapchainImageView[usedBuffer]->getImageView();
	imageViews[1] = depthStencilImageView->getImageView();

	framebuffer[usedBuffer] = vkts::framebufferCreate(contextObject->getDevice()->getDevice(), 0, renderPass->getRenderPass(), 2, imageViews, swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1);
	VKTS_VALIDATE_INSTANCE(framebuffer[usedBuffer], "Could not create frame buffer.");

	return VK_TRUE;
}

VkBool32 Example::buildSwapchainImageView(const int32_t usedBuffer)
{
	VkComponentMapping componentMapping           = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	swapchainImageView[usedBuffer] = vkts::imageViewCreate(contextObject->getDevice()->getDevice(), 0, swapchain->getAllSwapchainImages()[usedBuffer], VK_IMAGE_VIEW_TYPE_2D, swapchain->getImageFormat(), componentMapping, imageSubresourceRange);
	VKTS_VALIDATE_INSTANCE(swapchainImageView[usedBuffer], "Could not create color attachment view.");

	return VK_TRUE;
}

VkBool32 Example::updateDescriptorSets()
{
	memset(descriptorBufferInfos, 0, sizeof(descriptorBufferInfos));

	descriptorBufferInfos[0].buffer = vertexViewProjectionUniformBuffer->getBuffer()->getBuffer();
	descriptorBufferInfos[0].offset = 0;
	descriptorBufferInfos[0].range  = vertexViewProjectionUniformBuffer->getBuffer()->getSize() / vertexViewProjectionUniformBuffer->getBufferCount();

	descriptorBufferInfos[1].buffer = fragmentUniformBuffer->getBuffer()->getBuffer();
	descriptorBufferInfos[1].offset = 0;
	descriptorBufferInfos[1].range  = fragmentUniformBuffer->getBuffer()->getSize() / fragmentUniformBuffer->getBufferCount();

	memset(writeDescriptorSets, 0, sizeof(writeDescriptorSets));

	writeDescriptorSets[0].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[0].dstSet           = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[0].dstBinding       = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	writeDescriptorSets[0].dstArrayElement  = 0;
	writeDescriptorSets[0].descriptorCount  = 1;
	writeDescriptorSets[0].descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSets[0].pImageInfo       = nullptr;
	writeDescriptorSets[0].pBufferInfo      = &descriptorBufferInfos[0];
	writeDescriptorSets[0].pTexelBufferView = nullptr;

	writeDescriptorSets[1].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSets[1].dstSet           = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[1].dstBinding       = VKTS_BINDING_UNIFORM_BUFFER_LIGHT;
	writeDescriptorSets[1].dstArrayElement  = 0;
	writeDescriptorSets[1].descriptorCount  = 1;
	writeDescriptorSets[1].descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSets[1].pImageInfo       = nullptr;
	writeDescriptorSets[1].pBufferInfo      = &descriptorBufferInfos[1];
	writeDescriptorSets[1].pTexelBufferView = nullptr;

	//

	writeDescriptorSets[2].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;

	for (uint32_t i = VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST; i <= VKTS_BINDING_UNIFORM_SAMPLER_PHONG_LAST; i++)
	{
		writeDescriptorSets[3 + i - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST].dstBinding = i;
	}

	return VK_TRUE;
}

VkBool32 Example::buildScene(const vkts::ICommandObjectSP& commandObject)
{
	renderFactory = vkts::sceneRenderFactoryCreate(descriptorSetLayout, vkts::IRenderPassSP(), vkts::IPipelineCacheSP(), VKTS_MAX_NUMBER_BUFFERS);
	VKTS_VALIDATE_INSTANCE(renderFactory, "Could not create data factory.");

	//

	sceneFactory = vkts::sceneFactoryCreate(renderFactory);
	VKTS_VALIDATE_INSTANCE(sceneFactory, "Could not create factory.");

	//

	sceneManager = vkts::sceneManagerCreate(VK_FALSE, contextObject, commandObject);
	VKTS_VALIDATE_INSTANCE(sceneManager, "Could not create cache.");

	//

	scene = vkts::sceneLoad(VKTS_SCENE_NAME, sceneManager, sceneFactory);
	VKTS_VALIDATE_INSTANCE(scene, "Could not load scene.");

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Number objects: %d", scene->getNumberObjects());

	//

	// Sorted by binding
	dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION] = VkTsDynamicOffset{ 0, (uint32_t)contextObject->getPhysicalDevice()->getUniformBufferAlignmentSizeInBytes(vkts::alignmentGetSizeInBytes(16 * sizeof(float) * 2, 16)) };
	dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM]      = VkTsDynamicOffset{ 0, (uint32_t)sceneFactory->getSceneRenderFactory()->getTransformUniformBufferAlignmentSize(sceneManager) };
	dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_LIGHT]          = VkTsDynamicOffset{ 0, (uint32_t)contextObject->getPhysicalDevice()->getUniformBufferAlignmentSizeInBytes(vkts::alignmentGetSizeInBytes(3 * sizeof(float), 16)) };

	//
	// Free resources.
	//

	if (sceneFactory.get())
	{
		sceneFactory.reset();
	}

	if (sceneManager.get())
	{
		sceneManager->destroy();

		sceneManager.reset();
	}

	if (renderFactory.get())
	{
		renderFactory.reset();
	}

	return VK_TRUE;
}

VkBool32 Example::buildDepthStencilImageView()
{
	VkComponentMapping componentMapping           = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	depthStencilImageView = vkts::imageViewCreate(contextObject->getDevice()->getDevice(), 0, depthTexture->getImage()->getImage(), VK_IMAGE_VIEW_TYPE_2D, depthTexture->getImage()->getFormat(), componentMapping, imageSubresourceRange);
	VKTS_VALIDATE_INSTANCE(depthStencilImageView, "Could not create depth attachment view.");

	return VK_TRUE;
}

VkBool32 Example::buildDepthTexture(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkImageCreateInfo imageCreateInfo{};

	imageCreateInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.flags                 = 0;
	imageCreateInfo.imageType             = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format                = VK_FORMAT_D16_UNORM;
	imageCreateInfo.extent                = { swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1 };
	imageCreateInfo.mipLevels             = 1;
	imageCreateInfo.arrayLayers           = 1;
	imageCreateInfo.samples               = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling                = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage                 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices   = nullptr;
	imageCreateInfo.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	depthTexture = vkts::imageObjectCreate(contextObject, cmdBuffer, "DepthTexture", imageCreateInfo, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VKTS_VALIDATE_INSTANCE(depthTexture, "Could not create depth texture.");

	return VK_TRUE;
}

VkBool32 Example::buildPipeline()
{
	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[VKTS_SHADER_STAGE_COUNT]{};

	pipelineShaderStageCreateInfo[0].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[0].flags               = 0;
	pipelineShaderStageCreateInfo[0].stage               = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineShaderStageCreateInfo[0].module              = vertexShaderModule->getShaderModule();
	pipelineShaderStageCreateInfo[0].pName               = "main";
	pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

	pipelineShaderStageCreateInfo[1].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[1].flags               = 0;
	pipelineShaderStageCreateInfo[1].stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelineShaderStageCreateInfo[1].module              = fragmentShaderModule->getShaderModule();
	pipelineShaderStageCreateInfo[1].pName               = "main";
	pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

	//

	VkTsVertexBufferType vertexBufferType = VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TANGENTS | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0;

	VkVertexInputBindingDescription vertexInputBindingDescription{};

	vertexInputBindingDescription.binding   = VKTS_BINDING_VERTEX_BUFFER;
	vertexInputBindingDescription.stride    = vkts::alignmentGetStrideInBytes(vertexBufferType);
	vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vertexInputAttributeDescription[3]{};

	vertexInputAttributeDescription[0].location = 0;
	vertexInputAttributeDescription[0].binding  = VKTS_BINDING_VERTEX_BUFFER;
	vertexInputAttributeDescription[0].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexInputAttributeDescription[0].offset   = vkts::alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_VERTEX, vertexBufferType);

	vertexInputAttributeDescription[1].location = 1;
	vertexInputAttributeDescription[1].binding  = VKTS_BINDING_VERTEX_BUFFER;
	vertexInputAttributeDescription[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
	vertexInputAttributeDescription[1].offset   = vkts::alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_NORMAL, vertexBufferType);

	vertexInputAttributeDescription[2].location = 2;
	vertexInputAttributeDescription[2].binding  = VKTS_BINDING_VERTEX_BUFFER;
	vertexInputAttributeDescription[2].format   = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributeDescription[2].offset   = vkts::alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0, vertexBufferType);

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo{};

	pipelineVertexInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputCreateInfo.flags                           = 0;
	pipelineVertexInputCreateInfo.vertexBindingDescriptionCount   = 1;
	pipelineVertexInputCreateInfo.pVertexBindingDescriptions      = &vertexInputBindingDescription;
	pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = 3;
	pipelineVertexInputCreateInfo.pVertexAttributeDescriptions    = vertexInputAttributeDescription;

	//

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};

	pipelineInputAssemblyStateCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.flags                  = 0;
	pipelineInputAssemblyStateCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	//

	VkViewport viewport{};

	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = (float)swapchain->getImageExtent().width;
	viewport.height   = (float)swapchain->getImageExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent   = swapchain->getImageExtent();

	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};

	pipelineViewportStateCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.flags         = 0;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports    = &viewport;
	pipelineViewportStateCreateInfo.scissorCount  = 1;
	pipelineViewportStateCreateInfo.pScissors     = &scissor;

	//

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};

	pipelineRasterizationStateCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.flags                   = 0;
	pipelineRasterizationStateCreateInfo.depthClampEnable        = VK_FALSE;
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
	pipelineRasterizationStateCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
	pipelineRasterizationStateCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.depthBiasEnable         = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasClamp          = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor    = 0.0f;
	pipelineRasterizationStateCreateInfo.lineWidth               = 1.0f;

	//

	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};

	pipelineMultisampleStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.flags                 = 0;
	pipelineMultisampleStateCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	pipelineMultisampleStateCreateInfo.sampleShadingEnable   = VK_FALSE;
	pipelineMultisampleStateCreateInfo.minSampleShading      = 0.0f;
	pipelineMultisampleStateCreateInfo.pSampleMask           = nullptr;
	pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.alphaToOneEnable      = VK_FALSE;

	//

	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};

	pipelineDepthStencilStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.flags                 = 0;
	pipelineDepthStencilStateCreateInfo.depthTestEnable       = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable      = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.stencilTestEnable     = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.front.failOp          = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.front.passOp          = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.front.depthFailOp     = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.front.compareOp       = VK_COMPARE_OP_NEVER;
	pipelineDepthStencilStateCreateInfo.front.compareMask     = 0;
	pipelineDepthStencilStateCreateInfo.front.writeMask       = 0;
	pipelineDepthStencilStateCreateInfo.front.reference       = 0;
	pipelineDepthStencilStateCreateInfo.back.failOp           = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.back.passOp           = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.back.depthFailOp      = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.back.compareOp        = VK_COMPARE_OP_NEVER;
	pipelineDepthStencilStateCreateInfo.back.compareMask      = 0;
	pipelineDepthStencilStateCreateInfo.back.writeMask        = 0;
	pipelineDepthStencilStateCreateInfo.back.reference        = 0;
	pipelineDepthStencilStateCreateInfo.minDepthBounds        = 0.0f;

	//

	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};

	pipelineColorBlendAttachmentState.blendEnable         = VK_FALSE;
	pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.colorBlendOp        = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.colorWriteMask      = 0xf;

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};

	pipelineColorBlendStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.flags             = 0;
	pipelineColorBlendStateCreateInfo.logicOpEnable     = VK_FALSE;
	pipelineColorBlendStateCreateInfo.logicOp           = VK_LOGIC_OP_COPY;
	pipelineColorBlendStateCreateInfo.attachmentCount   = 1;
	pipelineColorBlendStateCreateInfo.pAttachments      = &pipelineColorBlendAttachmentState;
	pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	//

	VkDynamicState dynamicState[VKTS_NUMBER_DYNAMIC_STATES] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};

	pipelineDynamicStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineDynamicStateCreateInfo.flags             = 0;
	pipelineDynamicStateCreateInfo.dynamicStateCount = VKTS_NUMBER_DYNAMIC_STATES;
	pipelineDynamicStateCreateInfo.pDynamicStates    = dynamicState;

	//

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};

	graphicsPipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.flags               = 0;
	graphicsPipelineCreateInfo.stageCount          = VKTS_SHADER_STAGE_COUNT;
	graphicsPipelineCreateInfo.pStages             = pipelineShaderStageCreateInfo;
	graphicsPipelineCreateInfo.pVertexInputState   = &pipelineVertexInputCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pTessellationState  = nullptr;
	graphicsPipelineCreateInfo.pViewportState      = &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState   = &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState  = &pipelineDepthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState    = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState       = &pipelineDynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout              = pipelineLayout->getPipelineLayout();
	graphicsPipelineCreateInfo.renderPass          = renderPass->getRenderPass();
	graphicsPipelineCreateInfo.subpass             = 0;
	graphicsPipelineCreateInfo.basePipelineHandle  = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex   = 0;

	auto pipeline = vkts::pipelineCreateGraphics(contextObject->getDevice()->getDevice(), VK_NULL_HANDLE, graphicsPipelineCreateInfo, vertexBufferType);
	VKTS_VALIDATE_INSTANCE(pipeline, "Could not create graphics pipeline.");

	allGraphicsPipelines.append(pipeline);

	return VK_TRUE;
}

VkBool32 Example::buildRenderPass()
{
	VkAttachmentDescription attachmentDescription[2]{};

	attachmentDescription[0].flags          = 0;
	attachmentDescription[0].format         = swapchain->getImageFormat();
	attachmentDescription[0].samples        = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[0].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[0].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDescription[0].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachmentDescription[1].flags          = 0;
	attachmentDescription[1].format         = VK_FORMAT_D16_UNORM;
	attachmentDescription[1].samples        = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[1].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription[1].storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[1].initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentDescription[1].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentReference;

	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference deptStencilAttachmentReference;

	deptStencilAttachmentReference.attachment = 1;
	deptStencilAttachmentReference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription{};

	subpassDescription.flags                   = 0;
	subpassDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount    = 0;
	subpassDescription.pInputAttachments       = nullptr;
	subpassDescription.colorAttachmentCount    = 1;
	subpassDescription.pColorAttachments       = &colorAttachmentReference;
	subpassDescription.pResolveAttachments     = nullptr;
	subpassDescription.pDepthStencilAttachment = &deptStencilAttachmentReference;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments    = nullptr;

	renderPass = vkts::renderPassCreate(contextObject->getDevice()->getDevice(), 0, 2, attachmentDescription, 1, &subpassDescription, 0, nullptr);
	VKTS_VALIDATE_INSTANCE(renderPass, "Could not create render pass.");

	return VK_TRUE;
}

VkBool32 Example::buildPipelineLayout()
{
	VkDescriptorSetLayout setLayouts[1];

	setLayouts[0] = descriptorSetLayout->getDescriptorSetLayout();

	pipelineLayout = vkts::pipelineCreateLayout(contextObject->getDevice()->getDevice(), 0, 1, setLayouts, 0, nullptr);
	VKTS_VALIDATE_INSTANCE(pipelineLayout, "Could not create pipeline layout.");

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[VKTS_DESCRIPTOR_SET_COUNT]{};

	descriptorSetLayoutBinding[0].binding            = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	descriptorSetLayoutBinding[0].descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetLayoutBinding[0].descriptorCount    = 1;
	descriptorSetLayoutBinding[0].stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[0].pImmutableSamplers = nullptr;

	descriptorSetLayoutBinding[1].binding            = VKTS_BINDING_UNIFORM_BUFFER_LIGHT;
	descriptorSetLayoutBinding[1].descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetLayoutBinding[1].descriptorCount    = 1;
	descriptorSetLayoutBinding[1].stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorSetLayoutBinding[1].pImmutableSamplers = nullptr;

	descriptorSetLayoutBinding[2].binding            = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
	descriptorSetLayoutBinding[2].descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetLayoutBinding[2].descriptorCount    = 1;
	descriptorSetLayoutBinding[2].stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[2].pImmutableSamplers = nullptr;

	for (int32_t i = VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST; i <= VKTS_BINDING_UNIFORM_SAMPLER_PHONG_LAST; i++)
	{
		descriptorSetLayoutBinding[3 + i - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST].binding            = i;
		descriptorSetLayoutBinding[3 + i - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST].descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[3 + i - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST].descriptorCount    = 1;
		descriptorSetLayoutBinding[3 + i - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST].stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[3 + i - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST].pImmutableSamplers = nullptr;
	}

	//

	descriptorSetLayout = vkts::descriptorSetLayoutCreate(contextObject->getDevice()->getDevice(), 0, VKTS_DESCRIPTOR_SET_COUNT, descriptorSetLayoutBinding);
	VKTS_VALIDATE_INSTANCE(descriptorSetLayout, "Could not create descriptor set layout.");

	return VK_TRUE;
}

VkBool32 Example::buildShader()
{
	auto vertexShaderBinary = vkts::fileLoadBinary(VKTS_VERTEX_SHADER_NAME);
	VKTS_VALIDATE_INSTANCE(vertexShaderBinary, "Could not load vertex shader: '" + VKTS_VERTEX_SHADER_NAME + "'");

	auto fragmentShaderBinary = vkts::fileLoadBinary(VKTS_FRAGMENT_SHADER_NAME);
	VKTS_VALIDATE_INSTANCE(fragmentShaderBinary, "Could not load fragment shader: '" + VKTS_FRAGMENT_SHADER_NAME + "'");

	//

	vertexShaderModule = vkts::shaderModuleCreate(VKTS_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());
	VKTS_VALIDATE_INSTANCE(vertexShaderModule, "Could not create vertex shader module.");

	fragmentShaderModule = vkts::shaderModuleCreate(VKTS_FRAGMENT_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, fragmentShaderBinary->getSize(), (uint32_t*)fragmentShaderBinary->getData());
	VKTS_VALIDATE_INSTANCE(fragmentShaderModule, "Could not create fragment shader module.");

	return VK_TRUE;
}

VkBool32 Example::buildUniformBuffers()
{
	VkBufferCreateInfo bufferCreateInfo{};

	bufferCreateInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.flags                 = 0;
	bufferCreateInfo.size                  = vkts::alignmentGetSizeInBytes(16 * sizeof(float) * 2, 16);
	bufferCreateInfo.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferCreateInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices   = nullptr;

	vertexViewProjectionUniformBuffer = vkts::bufferObjectCreate(contextObject, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VKTS_MAX_NUMBER_BUFFERS);
	VKTS_VALIDATE_INSTANCE(vertexViewProjectionUniformBuffer, "Could not create vertex uniform buffer.");

	// Only change the size of it and then reuse the bufferCreateInfo.
	bufferCreateInfo.size = vkts::alignmentGetSizeInBytes(3 * sizeof(float), 16);

	fragmentUniformBuffer = vkts::bufferObjectCreate(contextObject, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VKTS_MAX_NUMBER_BUFFERS);
	VKTS_VALIDATE_INSTANCE(fragmentUniformBuffer, "Could not create fragment uniform buffer.");

	return VK_TRUE;
}

VkBool32 Example::buildResources(const vkts::IUpdateThreadContext& updateContext)
{
	VkResult result;

	//

	auto lastSwapchain = swapchain;

	VkSwapchainKHR oldSwapchain = lastSwapchain.get() ? lastSwapchain->getSwapchain() : VK_NULL_HANDLE;

	swapchain = vkts::wsiSwapchainCreate(contextObject->getPhysicalDevice()->getPhysicalDevice(), contextObject->getDevice()->getDevice(), 0, surface->getSurface(), VKTS_NUMBER_BUFFERS, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_PRESENT_MODE_FIFO_KHR, VK_TRUE, oldSwapchain);
	VKTS_VALIDATE_INSTANCE(swapchain, "Could not create swap chain.");

	//

	swapchainImagesCount = (uint32_t)swapchain->getAllSwapchainImages().size();
	VKTS_VALIDATE_CONDITION(swapchainImagesCount == 0, "Could not get swap chain images count.");

	if (swapchainImagesCount > VKTS_MAX_NUMBER_BUFFERS)
	{
		return VK_FALSE;
	}

	swapchainImageView = vkts::SmartPointerVector<vkts::IImageViewSP>(swapchainImagesCount);
	framebuffer        = vkts::SmartPointerVector<vkts::IFramebufferSP>(swapchainImagesCount);
	cmdBuffer          = vkts::SmartPointerVector<vkts::ICommandBuffersSP>(swapchainImagesCount);
	cmdBufferFence     = vkts::SmartPointerVector<vkts::IFenceSP>(swapchainImagesCount);

	for (uint32_t i = 0; i < swapchainImagesCount; i++)
	{
		cmdBufferFence[i] = vkts::fenceCreate(contextObject->getDevice()->getDevice(), VK_FENCE_CREATE_SIGNALED_BIT);
		VKTS_VALIDATE_INSTANCE(cmdBufferFence[i], "Could not create fence.");
	}

	//

	if (lastSwapchain.get())
	{
		lastSwapchain->destroy();
	}

	//

	if (!buildRenderPass())
	{
		return VK_FALSE;
	}

	if (!buildPipeline())
	{
		return VK_FALSE;
	}

	//

	vkts::ICommandBuffersSP updateCmdBuffer = vkts::commandBuffersCreate(contextObject->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
	VKTS_VALIDATE_INSTANCE(updateCmdBuffer, "Could not create command buffer.");

	//

	auto commandObject = vkts::commandObjectCreate(updateCmdBuffer);
	VKTS_VALIDATE_INSTANCE(commandObject, "Could not create command object.");

	//

	result = updateCmdBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);
	VKTS_VALIDATE_SUCCESS(result, "Could not begin command buffer.");

	if (!buildDepthTexture(updateCmdBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build texture.");

		return VK_FALSE;
	}

	VkBool32 doUpdateDescriptorSets = VK_FALSE;

	if (!scene.get())
	{
		if (!buildScene(commandObject))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build scene.");

			return VK_FALSE;
		}

		doUpdateDescriptorSets = VK_TRUE;
	}

	result = updateCmdBuffer->endCommandBuffer();
	VKTS_VALIDATE_SUCCESS(result, "Could not end command buffer.");


	VkSubmitInfo submitInfo{};

	submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount   = 0;
	submitInfo.pWaitSemaphores      = nullptr;
	submitInfo.commandBufferCount   = updateCmdBuffer->getCommandBufferCount();
	submitInfo.pCommandBuffers      = updateCmdBuffer->getCommandBuffers();
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores    = nullptr;

	result = contextObject->getQueue()->submit(1, &submitInfo, VK_NULL_HANDLE);
	VKTS_VALIDATE_SUCCESS(result, "Could not submit queue.");

	result = contextObject->getQueue()->waitIdle();
	VKTS_VALIDATE_SUCCESS(result, "Could not wait for idle queue.");

	commandObject->destroy();

	updateCmdBuffer->destroy();

	//

	if (doUpdateDescriptorSets)
	{
		if (!updateDescriptorSets())
		{
			return VK_FALSE;
		}

		if (scene.get())
		{
			for (uint32_t i = 0; i < swapchainImagesCount; i++)
			{
				scene->updateDescriptorSetsRecursive(VKTS_DESCRIPTOR_SET_COUNT, writeDescriptorSets, i);
			}
		}
	}

	if (!buildDepthStencilImageView())
	{
		return VK_FALSE;
	}

	//

	for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
	{
		if (!buildSwapchainImageView(i))
		{
			return VK_FALSE;
		}

		if (!buildFramebuffer(i))
		{
			return VK_FALSE;
		}

		if (!buildCmdBuffer(i))
		{
			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

void Example::terminateResources(const vkts::IUpdateThreadContext& updateContext)
{
	if (contextObject.get())
	{
		if (contextObject->getDevice().get())
		{
			contextObject->getDevice()->waitIdle();

			for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
			{
				if (cmdBufferFence[i].get())
				{
					cmdBufferFence[i]->destroy();
				}

				if (cmdBuffer[i].get())
				{
					cmdBuffer[i]->destroy();
				}

				if (framebuffer[i].get())
				{
					framebuffer[i]->destroy();
				}

				if (swapchainImageView[i].get())
				{
					swapchainImageView[i]->destroy();
				}
			}

			if (depthStencilImageView.get())
			{
				depthStencilImageView->destroy();
			}

			if (depthTexture.get())
			{
				depthTexture->destroy();
			}

			for (uint32_t i = 0; i < allGraphicsPipelines.size(); i++)
			{
				allGraphicsPipelines[i]->destroy();
			}
			allGraphicsPipelines.clear();

			if (renderPass.get())
			{
				renderPass->destroy();
			}
		}
	}
}

//
// Vulkan initialization.
//
VkBool32 Example::init(const vkts::IUpdateThreadContext& updateContext)
{
	if (!visualContext->isWindowAttached(windowIndex))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get window.");

		return VK_FALSE;
	}
	//

	surface->hasCurrentExtentChanged(contextObject->getPhysicalDevice()->getPhysicalDevice());

	//

	commandPool = vkts::commandPoolCreate(contextObject->getDevice()->getDevice(), 0, contextObject->getQueue()->getQueueFamilyIndex());
	VKTS_VALIDATE_INSTANCE(commandPool, "Could not get command pool.");

	//

	imageAcquiredSemaphore = vkts::semaphoreCreate(contextObject->getDevice()->getDevice(), 0);
	VKTS_VALIDATE_INSTANCE(imageAcquiredSemaphore, "Could not create semaphore.");

	renderingCompleteSemaphore = vkts::semaphoreCreate(contextObject->getDevice()->getDevice(), 0);
	VKTS_VALIDATE_INSTANCE(renderingCompleteSemaphore, "Could not create semaphore.");

	//

	if (!buildUniformBuffers())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build uniform buffers.");

		return VK_FALSE;
	}

	if (!buildShader())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build shader.");

		return VK_FALSE;
	}

	if (!buildDescriptorSetLayout())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build descriptor set layout.");

		return VK_FALSE;
	}

	if (!buildPipelineLayout())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build pipeline cache.");

		return VK_FALSE;
	}

	//

	if (!buildResources(updateContext))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

//
// Vulkan update.
//
VkBool32 Example::update(const vkts::IUpdateThreadContext& updateContext)
{

	VkResult result = VK_SUCCESS;

	//

	if (surface->hasCurrentExtentChanged(contextObject->getPhysicalDevice()->getPhysicalDevice()))
	{
		const auto& currentExtent = surface->getCurrentExtent(contextObject->getPhysicalDevice()->getPhysicalDevice(), VK_FALSE);

		if (currentExtent.width == 0 || currentExtent.height == 0)
		{
			return VK_TRUE;
		}

		result = VK_ERROR_OUT_OF_DATE_KHR;
	}

	//

	uint32_t currentBuffer;

	if (result == VK_SUCCESS)
	{
		result = swapchain->acquireNextImage(UINT64_MAX, imageAcquiredSemaphore->getSemaphore(), VK_NULL_HANDLE, currentBuffer);
	}

	if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
	{
		// Wait until complete, before to commit again.
		result = cmdBufferFence[currentBuffer]->waitForFence(UINT64_MAX);
		VKTS_VALIDATE_SUCCESS(result, "Could not wait for fence.");

		result = cmdBufferFence[currentBuffer]->reset();
		VKTS_VALIDATE_SUCCESS(result, "Could not reset fence.");

		//

		glm::mat4 projectionMatrix(1.0f);
		glm::mat4 viewMatrix(1.0f);

		const auto& currentExtent = surface->getCurrentExtent(contextObject->getPhysicalDevice()->getPhysicalDevice(), VK_FALSE);

		projectionMatrix = vkts::perspectiveMat4(45.0f, (float)currentExtent.width / (float)currentExtent.height, 1.0f, 100.0f);
		viewMatrix       = vkts::lookAtMat4(0.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		if (!vertexViewProjectionUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION].stride * (VkDeviceSize)currentBuffer + 0 * sizeof(float) * 16, 0, projectionMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}
		if (!vertexViewProjectionUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION].stride * (VkDeviceSize)currentBuffer + 1 * sizeof(float) * 16, 0, viewMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}

		//

		glm::vec3 lightDirection = glm::vec3(0.0f, 2.0f, 1.0f);

		lightDirection = glm::normalize(lightDirection);

		if (!fragmentUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_LIGHT].stride * (VkDeviceSize)currentBuffer + 0, 0, lightDirection))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload light direction.");

			return VK_FALSE;
		}

		if (scene.get())
		{
			scene->updateTransformRecursive(updateContext.getDeltaTime(), updateContext.getDeltaTicks(), updateContext.getTickTime(), currentBuffer);
		}

		//

		VkSemaphore waitSemaphores = imageAcquiredSemaphore->getSemaphore();
		VkSemaphore signalSemaphores = renderingCompleteSemaphore->getSemaphore();


		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		VkSubmitInfo submitInfo{};

		submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount   = 1;
		submitInfo.pWaitSemaphores      = &waitSemaphores;
		submitInfo.pWaitDstStageMask    = &waitDstStageMask;
		submitInfo.commandBufferCount   = 1;
		submitInfo.pCommandBuffers      = cmdBuffer[currentBuffer]->getCommandBuffers();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores    = &signalSemaphores;

		result = contextObject->getQueue()->submit(1, &submitInfo, cmdBufferFence[currentBuffer]->getFence());
		VKTS_VALIDATE_SUCCESS(result, "Could not submit queue.");

		waitSemaphores = renderingCompleteSemaphore->getSemaphore();

		VkSwapchainKHR swapchains = swapchain->getSwapchain();

		result = swapchain->queuePresent(contextObject->getQueue()->getQueue(), 1, &waitSemaphores, 1, &swapchains, &currentBuffer, nullptr);

		if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		{
			// Do nothing, as everything is buffered and synchronized.
		}
		else
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				terminateResources(updateContext);

				if (!buildResources(updateContext))
				{
					vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

					return VK_FALSE;
				}
			}
			else
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not present queue.");

				return VK_FALSE;
			}
		}
	}
	else
	{
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			terminateResources(updateContext);

			if (!buildResources(updateContext))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

				return VK_FALSE;
			}
		}
		else
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not acquire next image.");

			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

//
// Vulkan termination.
//
void Example::terminate(const vkts::IUpdateThreadContext& updateContext)
{
	if (contextObject.get())
	{
		if (contextObject->getDevice().get())
		{
			terminateResources(updateContext);

			//

			if (sceneFactory.get())
			{
				sceneFactory.reset();
			}

			if (sceneManager.get())
			{
				sceneManager->destroy();

				sceneManager.reset();
			}

			if (renderFactory.get())
			{
				renderFactory.reset();
			}

			if (scene.get())
			{
				scene->destroy();
			}

			if (swapchain.get())
			{
				swapchain->destroy();
			}

			if (pipelineLayout.get())
			{
				pipelineLayout->destroy();
			}

			if (vertexShaderModule.get())
			{
				vertexShaderModule->destroy();
			}

			if (fragmentShaderModule.get())
			{
				fragmentShaderModule->destroy();
			}

			if (vertexViewProjectionUniformBuffer.get())
			{
				vertexViewProjectionUniformBuffer->destroy();
			}

			if (fragmentUniformBuffer.get())
			{
				fragmentUniformBuffer->destroy();
			}

			if (descriptorSetLayout.get())
			{
				descriptorSetLayout->destroy();
			}

			if (renderingCompleteSemaphore.get())
			{
				renderingCompleteSemaphore->destroy();
			}

			if (imageAcquiredSemaphore.get())
			{
				imageAcquiredSemaphore->destroy();
			}

			if (commandPool.get())
			{
				commandPool->destroy();
			}
		}
	}
}