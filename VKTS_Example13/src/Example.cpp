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

Example::Example(const vkts::IContextObjectSP& contextObject, const int32_t windowIndex, const vkts::IVisualContextSP& visualContext, const vkts::ISurfaceSP& surface, const std::string& sceneName, const std::string& outputSceneName, const std::string& environmentName) :
		IUpdateThread(), contextObject(contextObject), windowIndex(windowIndex), visualContext(visualContext), surface(surface), showStats(VK_FALSE), camera(nullptr), inputController(nullptr), allUpdateables(), commandPool(nullptr), pipelineCache(nullptr), imageAcquiredSemaphore(nullptr), renderingCompleteSemaphore(nullptr), environmentDescriptorSetLayout(nullptr), environmentDescriptorBufferInfos{}, descriptorBufferInfos{}, environmentDescriptorImageInfos{}, descriptorImageInfos{}, writeDescriptorSets{}, environmentWriteDescriptorSets{}, dynamicOffsets(), vertexViewProjectionUniformBuffer(nullptr), environmentVertexViewProjectionUniformBuffer(nullptr), fragmentLightsUniformBuffer(nullptr), fragmentMatricesUniformBuffer(nullptr), allBSDFVertexShaderModules(), envVertexShaderModule(nullptr), envFragmentShaderModule(nullptr), environmentPipelineLayout(nullptr), guiRenderFactory(nullptr), guiManager(nullptr), guiFactory(nullptr), font(nullptr), loadTask(), sceneLoaded(VK_FALSE), renderFactory(nullptr), sceneManager(nullptr), sceneFactory(nullptr), scene(nullptr), environmentRenderFactory(nullptr), environmentSceneManager(nullptr), environmentSceneFactory(nullptr), environmentScene(nullptr), swapchain(nullptr), renderPass(nullptr), allGraphicsPipelines(), depthTexture(), msaaColorTexture(), msaaDepthTexture(), depthStencilImageView(), msaaColorImageView(), msaaDepthStencilImageView(), swapchainImagesCount(0), swapchainImageView(), framebuffer(), cmdBuffer(), cmdBufferFence(), rebuildCmdBufferCounter(0), fps(0), ram(0), cpuUsageApp(0.0f), processors(0), sceneName(sceneName), outputSceneName(outputSceneName), environmentName(environmentName)
{
	processors = glm::min(vkts::processorGetNumber(), VKTS_MAX_CORES);

	for (uint32_t i = 0; i < processors; i++)
	{
		cpuUsage[i] = 0.0f;
	}
}

Example::~Example()
{
}

VkBool32 Example::buildCmdBuffer(const int32_t usedBuffer)
{
	int32_t toneMapping = 0;

	float strength = 1.0f;

	float maxLuminance = 1.0f;

	if (environmentScene.get())
	{
		if (environmentScene->getEnvironment()->getImageObject()->getImageData()->isSFLOAT())
		{
			toneMapping = 1;
		}

		strength = environmentScene->getEnvironmentStrength();

		maxLuminance = environmentScene->getMaxLuminance();
	}

	//

	VkResult result;

	if (cmdBuffer[usedBuffer].get())
	{
		result = cmdBuffer[usedBuffer]->reset();

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not reset command buffer.");

			return VK_FALSE;
		}
	}
	else
	{
		cmdBuffer[usedBuffer] = vkts::commandBuffersCreate(contextObject->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

		if (!cmdBuffer[usedBuffer].get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

			return VK_FALSE;
		}
	}

	result = cmdBuffer[usedBuffer]->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}

	//
	// Write and read for blending.
	//

    swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, usedBuffer);

    //

	VkViewport viewport{};

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapchain->getImageExtent().width;
	viewport.height = (float) swapchain->getImageExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapchain->getImageExtent();


	VkClearColorValue clearColorValue{};

	clearColorValue.float32[0] = 0.0f;
	clearColorValue.float32[1] = 0.0f;
	clearColorValue.float32[2] = 0.0f;
	clearColorValue.float32[3] = 0.0f;

	VkClearDepthStencilValue clearDepthStencilValue{};

	clearDepthStencilValue.depth = 1.0f;
	clearDepthStencilValue.stencil = 0;

	VkClearValue clearValues[4]{};

	clearValues[0].color = clearColorValue;
	clearValues[1].depthStencil = clearDepthStencilValue;
	clearValues[2].color = clearColorValue;
	clearValues[3].depthStencil = clearDepthStencilValue;


	VkRenderPassBeginInfo renderPassBeginInfo{};

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderPassBeginInfo.renderPass = renderPass->getRenderPass();
	renderPassBeginInfo.framebuffer = framebuffer[usedBuffer]->getFramebuffer();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = swapchain->getImageExtent();
	renderPassBeginInfo.clearValueCount = 4;
	renderPassBeginInfo.pClearValues = clearValues;

	cmdBuffer[usedBuffer]->cmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdSetViewport(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &viewport);

	vkCmdSetScissor(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &scissor);

	vkCmdPushConstants(cmdBuffer[usedBuffer]->getCommandBuffer(), allGraphicsPipelines[0]->getLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(int32_t), &toneMapping);
	vkCmdPushConstants(cmdBuffer[usedBuffer]->getCommandBuffer(), allGraphicsPipelines[0]->getLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(int32_t), sizeof(float), &strength);
	vkCmdPushConstants(cmdBuffer[usedBuffer]->getCommandBuffer(), allGraphicsPipelines[0]->getLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(int32_t) + sizeof(float), sizeof(float), &maxLuminance);

	// Render cube map.

	if (sphereScene.get())
	{
		sphereScene->drawRecursive(cmdBuffer[usedBuffer], allGraphicsPipelines, usedBuffer, dynamicOffsets);
	}

	// Render main scene.

	if (scene.get())
	{
		vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> empty;

		vkts::Blend blend;

		// First all opaque elements.
		blend.setPassTransparent(VK_FALSE);
		scene->drawRecursive(cmdBuffer[usedBuffer], empty, usedBuffer, dynamicOffsets, &blend);

		// Then, transparent elements.
		blend.setPassTransparent(VK_TRUE);
		scene->drawRecursive(cmdBuffer[usedBuffer], empty, usedBuffer, dynamicOffsets, &blend);
	}

	// Render font.

	if (font.get() && showStats)
	{
		glm::mat4 projectionMatrix = vkts::orthoMat4((float)swapchain->getImageExtent().width * -0.5f, (float)swapchain->getImageExtent().width * 0.5f, (float)swapchain->getImageExtent().height * -0.5f, (float)swapchain->getImageExtent().height  * 0.5f, 0.0f, 100.0f);

		char buffer[VKTS_OUTPUT_BUFFER_SIZE];

		sprintf(buffer, "Example FPS: %u\nExample RAM: %" SCNu64 " kb\nExample CPU: %.2f%%", fps, ram, cpuUsageApp);

		float y = (float)swapchain->getImageExtent().height * 0.5f - 10.0f - font->getLineHeight(VKTS_FONT_SIZE);

		font->drawText(cmdBuffer[usedBuffer], projectionMatrix, glm::vec2((float)swapchain->getImageExtent().width * -0.5f + 10.0f, y), buffer, VKTS_FONT_SIZE, VKTS_FONT_COLOR);

		y -= font->getLineHeight(VKTS_FONT_SIZE) * 4.0f;

		for (uint32_t cpu = 0; cpu < processors; cpu++)
		{
			sprintf(buffer, "CPU%u: %.2f%%", cpu, cpuUsage[cpu]);

			font->drawText(cmdBuffer[usedBuffer], projectionMatrix, glm::vec2((float)swapchain->getImageExtent().width * -0.5f + 10.0f, y), buffer, VKTS_FONT_SIZE, VKTS_FONT_COLOR);

			y -= font->getLineHeight(VKTS_FONT_SIZE);
		}
	}

	cmdBuffer[usedBuffer]->cmdEndRenderPass();

    //

    swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, usedBuffer);

    //

	result = cmdBuffer[usedBuffer]->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildFramebuffer(const int32_t usedBuffer)
{
	VkImageView imageViews[4];

	imageViews[0] = swapchainImageView[usedBuffer]->getImageView();
	imageViews[1] = depthStencilImageView->getImageView();
	imageViews[2] = msaaColorImageView->getImageView();
	imageViews[3] = msaaDepthStencilImageView->getImageView();

	framebuffer[usedBuffer] = vkts::framebufferCreate(contextObject->getDevice()->getDevice(), 0, renderPass->getRenderPass(), 4, imageViews, swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1);

	if (!framebuffer[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create frame buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::updateDescriptorSets(const int32_t usedBuffer)
{
	memset(environmentDescriptorBufferInfos, 0, sizeof(environmentDescriptorBufferInfos));

	environmentDescriptorBufferInfos[0].buffer = environmentVertexViewProjectionUniformBuffer->getBuffer()->getBuffer();
	environmentDescriptorBufferInfos[0].offset = 0;
	environmentDescriptorBufferInfos[0].range = environmentVertexViewProjectionUniformBuffer->getBuffer()->getSize() / environmentVertexViewProjectionUniformBuffer->getBufferCount();


	memset(environmentDescriptorImageInfos, 0, sizeof(environmentDescriptorImageInfos));

	environmentDescriptorImageInfos[0].sampler = environmentScene->getEnvironment()->getSampler()->getSampler();
	environmentDescriptorImageInfos[0].imageView = environmentScene->getEnvironment()->getImageObject()->getImageView()->getImageView();
	environmentDescriptorImageInfos[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;


	memset(environmentWriteDescriptorSets, 0, sizeof(environmentWriteDescriptorSets));

	environmentWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	environmentWriteDescriptorSets[0].dstSet = VK_NULL_HANDLE;	// Defined later.
	environmentWriteDescriptorSets[0].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	environmentWriteDescriptorSets[0].dstArrayElement = 0;
	environmentWriteDescriptorSets[0].descriptorCount = 1;
	environmentWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	environmentWriteDescriptorSets[0].pImageInfo = nullptr;
	environmentWriteDescriptorSets[0].pBufferInfo = &environmentDescriptorBufferInfos[0];
	environmentWriteDescriptorSets[0].pTexelBufferView = nullptr;


	environmentWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	environmentWriteDescriptorSets[1].dstSet = VK_NULL_HANDLE;	// Defined later.
	environmentWriteDescriptorSets[1].dstBinding = VKTS_BINDING_UNIFORM_SAMPLER_ENVIRONMENT;
	environmentWriteDescriptorSets[1].dstArrayElement = 0;
	environmentWriteDescriptorSets[1].descriptorCount = 1;
	environmentWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	environmentWriteDescriptorSets[1].pImageInfo = &environmentDescriptorImageInfos[0];
	environmentWriteDescriptorSets[1].pBufferInfo = nullptr;
	environmentWriteDescriptorSets[1].pTexelBufferView = nullptr;


	environmentWriteDescriptorSets[2].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;

	//
	//

	memset(descriptorBufferInfos, 0, sizeof(descriptorBufferInfos));

	memset(descriptorImageInfos, 0, sizeof(descriptorImageInfos));


	descriptorBufferInfos[0].buffer = vertexViewProjectionUniformBuffer->getBuffer()->getBuffer();
	descriptorBufferInfos[0].offset = 0;
	descriptorBufferInfos[0].range = vertexViewProjectionUniformBuffer->getBuffer()->getSize() / vertexViewProjectionUniformBuffer->getBufferCount();


	memset(writeDescriptorSets, 0, sizeof(writeDescriptorSets));

	writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[0].dstSet = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[0].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	writeDescriptorSets[0].dstArrayElement = 0;
	writeDescriptorSets[0].descriptorCount = 1;
	writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSets[0].pImageInfo = nullptr;
	writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfos[0];
	writeDescriptorSets[0].pTexelBufferView = nullptr;


	writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[1].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;


	writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[2].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM;


	// Diffuse/lambert.
	descriptorImageInfos[0].sampler = environmentScene->getDiffuseEnvironment()->getSampler()->getSampler();
	descriptorImageInfos[0].imageView = environmentScene->getDiffuseEnvironment()->getImageObject()->getImageView()->getImageView();
	descriptorImageInfos[0].imageLayout = environmentScene->getDiffuseEnvironment()->getImageObject()->getImage()->getImageLayout();

	writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[3].dstSet = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[3].dstBinding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_DIFFUSE;
	writeDescriptorSets[3].dstArrayElement = 0;
	writeDescriptorSets[3].descriptorCount = 1;
	writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSets[3].pImageInfo = &descriptorImageInfos[0];
	writeDescriptorSets[3].pBufferInfo = nullptr;
	writeDescriptorSets[3].pTexelBufferView = nullptr;

	// Specular/Cook-Torrance.
	descriptorImageInfos[1].sampler = environmentScene->getSpecularEnvironment()->getSampler()->getSampler();
	descriptorImageInfos[1].imageView = environmentScene->getSpecularEnvironment()->getImageObject()->getImageView()->getImageView();
	descriptorImageInfos[1].imageLayout = environmentScene->getSpecularEnvironment()->getImageObject()->getImage()->getImageLayout();

	writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[4].dstSet = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[4].dstBinding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_SPECULAR;
	writeDescriptorSets[4].dstArrayElement = 0;
	writeDescriptorSets[4].descriptorCount = 1;
	writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSets[4].pImageInfo = &descriptorImageInfos[1];
	writeDescriptorSets[4].pBufferInfo = nullptr;
	writeDescriptorSets[4].pTexelBufferView = nullptr;

	//Lut
	descriptorImageInfos[2].sampler = environmentScene->getLut()->getSampler()->getSampler();
	descriptorImageInfos[2].imageView = environmentScene->getLut()->getImageObject()->getImageView()->getImageView();
	descriptorImageInfos[2].imageLayout = environmentScene->getLut()->getImageObject()->getImage()->getImageLayout();

	writeDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[5].dstSet = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[5].dstBinding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_LUT;
	writeDescriptorSets[5].dstArrayElement = 0;
	writeDescriptorSets[5].descriptorCount = 1;
	writeDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSets[5].pImageInfo = &descriptorImageInfos[2];
	writeDescriptorSets[5].pBufferInfo = nullptr;
	writeDescriptorSets[5].pTexelBufferView = nullptr;

	// Dynamic lights.
	descriptorBufferInfos[1].buffer = fragmentLightsUniformBuffer->getBuffer()->getBuffer();
	descriptorBufferInfos[1].offset = 0;
	descriptorBufferInfos[1].range = fragmentLightsUniformBuffer->getBuffer()->getSize() / fragmentLightsUniformBuffer->getBufferCount();

	writeDescriptorSets[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[6].dstSet = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[6].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_BSDF_FORWARD_LIGHT;
	writeDescriptorSets[6].dstArrayElement = 0;
	writeDescriptorSets[6].descriptorCount = 1;
	writeDescriptorSets[6].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSets[6].pImageInfo = nullptr;
	writeDescriptorSets[6].pBufferInfo = &descriptorBufferInfos[1];
	writeDescriptorSets[6].pTexelBufferView = nullptr;

	// Inverse matrix.
	descriptorBufferInfos[2].buffer = fragmentMatricesUniformBuffer->getBuffer()->getBuffer();
	descriptorBufferInfos[2].offset = 0;
	descriptorBufferInfos[2].range = fragmentMatricesUniformBuffer->getBuffer()->getSize() / fragmentMatricesUniformBuffer->getBufferCount();

	writeDescriptorSets[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[7].dstSet = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[7].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_BSDF_FORWARD_INVERSE;
	writeDescriptorSets[7].dstArrayElement = 0;
	writeDescriptorSets[7].descriptorCount = 1;
	writeDescriptorSets[7].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSets[7].pImageInfo = nullptr;
	writeDescriptorSets[7].pBufferInfo = &descriptorBufferInfos[2];
	writeDescriptorSets[7].pTexelBufferView = nullptr;


	for (uint32_t i = 8; i < VKTS_BINDING_UNIFORM_BSDF_FORWARD_TOTAL_BINDING_COUNT; i++)
	{
		writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		writeDescriptorSets[i].dstBinding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_FIRST + (i - 8);
	}

	return VK_TRUE;
}

VkBool32 Example::buildSwapchainImageView(const int32_t usedBuffer)
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	swapchainImageView[usedBuffer] = vkts::imageViewCreate(contextObject->getDevice()->getDevice(), 0, swapchain->getAllSwapchainImages()[usedBuffer], VK_IMAGE_VIEW_TYPE_2D, swapchain->getImageFormat(), componentMapping, imageSubresourceRange);

	if (!swapchainImageView[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create color attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildMSAADepthStencilImageView()
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	msaaDepthStencilImageView = vkts::imageViewCreate(contextObject->getDevice()->getDevice(), 0, msaaDepthTexture->getImage()->getImage(), VK_IMAGE_VIEW_TYPE_2D, msaaDepthTexture->getImage()->getFormat(), componentMapping, imageSubresourceRange);

	if (!msaaDepthStencilImageView.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create MSAA depth attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildMSAAColorImageView()
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	msaaColorImageView = vkts::imageViewCreate(contextObject->getDevice()->getDevice(), 0, msaaColorTexture->getImage()->getImage(), VK_IMAGE_VIEW_TYPE_2D, msaaColorTexture->getImage()->getFormat(), componentMapping, imageSubresourceRange);

	if (!msaaColorImageView.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create MSAA color attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDepthStencilImageView()
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	depthStencilImageView = vkts::imageViewCreate(contextObject->getDevice()->getDevice(), 0, depthTexture->getImage()->getImage(), VK_IMAGE_VIEW_TYPE_2D, depthTexture->getImage()->getFormat(), componentMapping, imageSubresourceRange);

	if (!depthStencilImageView.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create depth attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildMSAADepthTexture(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkImageCreateInfo imageCreateInfo{};

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D16_UNORM;
	imageCreateInfo.extent = {swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1};
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VKTS_SAMPLE_COUNT_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	msaaDepthTexture = vkts::imageObjectCreate(contextObject, cmdBuffer, "MSAADepthTexture", imageCreateInfo, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (!msaaDepthTexture.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create depth texture.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildMSAAColorTexture(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkImageCreateInfo imageCreateInfo{};

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = swapchain->getImageFormat();
	imageCreateInfo.extent = {swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1};
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VKTS_SAMPLE_COUNT_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	msaaColorTexture = vkts::imageObjectCreate(contextObject, cmdBuffer, "MSAAColorTexture", imageCreateInfo, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (!msaaColorTexture.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create msaa color texture.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDepthTexture(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkImageCreateInfo imageCreateInfo{};

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D16_UNORM;
	imageCreateInfo.extent = {swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1};
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	depthTexture = vkts::imageObjectCreate(contextObject, cmdBuffer, "DepthTexture", imageCreateInfo, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (!depthTexture.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create depth texture.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildPipeline()
{
    vkts::DefaultGraphicsPipeline gp;

    gp.getPipelineShaderStageCreateInfo(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
    gp.getPipelineShaderStageCreateInfo(0).module = envVertexShaderModule->getShaderModule();

    gp.getPipelineShaderStageCreateInfo(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    gp.getPipelineShaderStageCreateInfo(1).module = envFragmentShaderModule->getShaderModule();


	VkTsVertexBufferType vertexBufferType = VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TANGENTS | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0;

    gp.getVertexInputBindingDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
    gp.getVertexInputBindingDescription(0).stride = vkts::alignmentGetStrideInBytes(vertexBufferType);
    gp.getVertexInputBindingDescription(0).inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    gp.getVertexInputAttributeDescription(0).location = 0;
    gp.getVertexInputAttributeDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
    gp.getVertexInputAttributeDescription(0).format = VK_FORMAT_R32G32B32A32_SFLOAT;
    gp.getVertexInputAttributeDescription(0).offset = vkts::alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_VERTEX, vertexBufferType);


    gp.getPipelineInputAssemblyStateCreateInfo().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    gp.getViewports(0).x = 0.0f;
    gp.getViewports(0).y = 0.0f;
    gp.getViewports(0).width = (float) swapchain->getImageExtent().width;
    gp.getViewports(0).height = (float) swapchain->getImageExtent().height;
    gp.getViewports(0).minDepth = 0.0f;
    gp.getViewports(0).maxDepth = 1.0f;


    gp.getScissors(0).offset.x = 0;
    gp.getScissors(0).offset.y = 0;
    gp.getScissors(0).extent = swapchain->getImageExtent();


	gp.getPipelineRasterizationStateCreateInfo();

	gp.getPipelineMultisampleStateCreateInfo().rasterizationSamples = VKTS_SAMPLE_COUNT_BIT;

    gp.getPipelineDepthStencilStateCreateInfo();


    gp.getPipelineColorBlendAttachmentState(0).blendEnable = VK_FALSE;
    gp.getPipelineColorBlendAttachmentState(0).colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;


    gp.getDynamicState(0) = VK_DYNAMIC_STATE_VIEWPORT;
    gp.getDynamicState(1) = VK_DYNAMIC_STATE_SCISSOR;


    gp.getGraphicsPipelineCreateInfo().layout = environmentPipelineLayout->getPipelineLayout();
    gp.getGraphicsPipelineCreateInfo().renderPass = renderPass->getRenderPass();


    auto pipeline = vkts::pipelineCreateGraphics(contextObject->getDevice()->getDevice(), VK_NULL_HANDLE, gp.getGraphicsPipelineCreateInfo(), vertexBufferType);

	if (!pipeline.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create graphics pipeline.");

		return VK_FALSE;
	}

	allGraphicsPipelines.append(pipeline);

	return VK_TRUE;
}

VkBool32 Example::buildRenderPass()
{
	VkAttachmentDescription attachmentDescription[4]{};

	attachmentDescription[0].flags = 0;
	attachmentDescription[0].format = surface->getFormat(contextObject->getPhysicalDevice()->getPhysicalDevice());
	attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachmentDescription[1].flags = 0;
	attachmentDescription[1].format = VK_FORMAT_D16_UNORM;
	attachmentDescription[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentDescription[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachmentDescription[2].flags = 0;
	attachmentDescription[2].format = surface->getFormat(contextObject->getPhysicalDevice()->getPhysicalDevice());	// Later request same format for MSAA image.
	attachmentDescription[2].samples = VKTS_SAMPLE_COUNT_BIT;
	attachmentDescription[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[2].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDescription[2].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachmentDescription[3].flags = 0;
	attachmentDescription[3].format = VK_FORMAT_D16_UNORM;
	attachmentDescription[3].samples = VKTS_SAMPLE_COUNT_BIT;
	attachmentDescription[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription[3].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[3].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[3].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentDescription[3].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference resolveAttachmentReference[2];

	resolveAttachmentReference[0].attachment = 0;
	resolveAttachmentReference[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	resolveAttachmentReference[1].attachment = 1;
	resolveAttachmentReference[1].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentReference;

	colorAttachmentReference.attachment = 2;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference deptStencilAttachmentReference;

	deptStencilAttachmentReference.attachment = 3;
	deptStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//

	VkSubpassDescription subpassDescription[1]{};

	subpassDescription[0].flags = 0;
	subpassDescription[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription[0].inputAttachmentCount = 0;
	subpassDescription[0].pInputAttachments = nullptr;
	subpassDescription[0].colorAttachmentCount = 1;
	subpassDescription[0].pColorAttachments = &colorAttachmentReference;
	subpassDescription[0].pResolveAttachments = resolveAttachmentReference;
	subpassDescription[0].pDepthStencilAttachment = &deptStencilAttachmentReference;
	subpassDescription[0].preserveAttachmentCount = 0;
	subpassDescription[0].pPreserveAttachments = nullptr;

    //

	renderPass = vkts::renderPassCreate( contextObject->getDevice()->getDevice(), 0, 4, attachmentDescription, 1, subpassDescription, 0, nullptr);

	if (!renderPass.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create render pass.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildPipelineLayout()
{
	// Using push constant to set the tone mapping dynamically.

	VkPushConstantRange pushConstantRange[1];

	pushConstantRange[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange[0].offset = 0;
	pushConstantRange[0].size = sizeof(int32_t) + sizeof(float) + sizeof(float);

	//

	VkDescriptorSetLayout setLayouts[1];

	setLayouts[0] = environmentDescriptorSetLayout->getDescriptorSetLayout();

	environmentPipelineLayout = vkts::pipelineCreateLayout(contextObject->getDevice()->getDevice(), 0, 1, setLayouts, 1, pushConstantRange);

	if (!environmentPipelineLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[3]{};

	descriptorSetLayoutBinding[0].binding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	descriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetLayoutBinding[0].descriptorCount = 1;
	descriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[0].pImmutableSamplers = nullptr;

	descriptorSetLayoutBinding[1].binding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
	descriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetLayoutBinding[1].descriptorCount = 1;
	descriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[1].pImmutableSamplers = nullptr;

	descriptorSetLayoutBinding[2].binding = VKTS_BINDING_UNIFORM_SAMPLER_ENVIRONMENT;
	descriptorSetLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBinding[2].descriptorCount = 1;
	descriptorSetLayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorSetLayoutBinding[2].pImmutableSamplers = nullptr;

	environmentDescriptorSetLayout = vkts::descriptorSetLayoutCreate(contextObject->getDevice()->getDevice(), 0, 3, descriptorSetLayoutBinding);

	if (!environmentDescriptorSetLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildShader()
{
	auto vertexShaderBinary = vkts::fileLoadBinary(VKTS_ENV_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_ENV_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	auto fragmentShaderBinary = vkts::fileLoadBinary(VKTS_ENV_FRAGMENT_SHADER_NAME);

	if (!fragmentShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", VKTS_ENV_FRAGMENT_SHADER_NAME);

		return VK_FALSE;
	}

	//

	envVertexShaderModule = vkts::shaderModuleCreate(VKTS_ENV_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!envVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	envFragmentShaderModule = vkts::shaderModuleCreate(VKTS_ENV_FRAGMENT_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, fragmentShaderBinary->getSize(), (uint32_t*)fragmentShaderBinary->getData());

	if (!envFragmentShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

		return VK_FALSE;
	}

	//

	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF0_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF0_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	auto bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF0_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF1_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF1_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF1_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF2_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF2_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF2_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF3_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF3_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF3_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF4_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF4_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF4_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF5_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF5_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF5_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF6_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF6_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF6_VERTEX_SHADER_NAME, contextObject->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);

	return VK_TRUE;
}

VkBool32 Example::buildUniformBuffers()
{
	VkBufferCreateInfo bufferCreateInfo{};

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = vkts::alignmentGetSizeInBytes(16 * sizeof(float) * 2, 16);
	bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;


	vertexViewProjectionUniformBuffer = vkts::bufferObjectCreate(contextObject, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VKTS_MAX_NUMBER_BUFFERS);

	if (!vertexViewProjectionUniformBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex uniform buffer.");

		return VK_FALSE;
	}

	environmentVertexViewProjectionUniformBuffer = vkts::bufferObjectCreate(contextObject, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VKTS_MAX_NUMBER_BUFFERS);

	if (!environmentVertexViewProjectionUniformBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex uniform buffer.");

		return VK_FALSE;
	}


	bufferCreateInfo.size = vkts::alignmentGetSizeInBytes(VKTS_MAX_LIGHTS * 4 * sizeof(float) * 2 + sizeof(int32_t), 16);

	fragmentLightsUniformBuffer = vkts::bufferObjectCreate(contextObject, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VKTS_MAX_NUMBER_BUFFERS);

	if (!fragmentLightsUniformBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment uniform buffer.");

		return VK_FALSE;
	}


	bufferCreateInfo.size = vkts::alignmentGetSizeInBytes(16 * sizeof(float) * 2, 16);

	fragmentMatricesUniformBuffer = vkts::bufferObjectCreate(contextObject, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VKTS_MAX_NUMBER_BUFFERS);

	if (!fragmentMatricesUniformBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment uniform buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildResources(const vkts::IUpdateThreadContext& updateContext)
{
	VkResult result;

	//

	auto lastSwapchain = swapchain;

	VkSwapchainKHR oldSwapchain = lastSwapchain.get() ? lastSwapchain->getSwapchain() : VK_NULL_HANDLE;

	swapchain = vkts::wsiSwapchainCreate(contextObject->getPhysicalDevice()->getPhysicalDevice(), contextObject->getDevice()->getDevice(), 0, surface->getSurface(), VKTS_NUMBER_BUFFERS, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_PRESENT_MODE_FIFO_KHR, VK_TRUE, oldSwapchain);

	if (!swapchain.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create swap chain.");

		return VK_FALSE;
	}

    //

    swapchainImagesCount = (uint32_t)swapchain->getAllSwapchainImages().size();

    if (swapchainImagesCount == 0)
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get swap chain images count.");

        return VK_FALSE;
    }

    if (swapchainImagesCount > VKTS_MAX_NUMBER_BUFFERS)
    {
    	return VK_FALSE;
    }

    swapchainImageView = vkts::SmartPointerVector<vkts::IImageViewSP>(swapchainImagesCount);
    framebuffer = vkts::SmartPointerVector<vkts::IFramebufferSP>(swapchainImagesCount);
    cmdBuffer = vkts::SmartPointerVector<vkts::ICommandBuffersSP>(swapchainImagesCount);
    rebuildCmdBufferCounter = swapchainImagesCount;

    cmdBufferFence = vkts::SmartPointerVector<vkts::IFenceSP>(swapchainImagesCount);

    for (uint32_t i = 0; i < swapchainImagesCount; i++)
    {
    	cmdBufferFence[i] = vkts::fenceCreate(contextObject->getDevice()->getDevice(), VK_FENCE_CREATE_SIGNALED_BIT);

        if (!cmdBufferFence[i].get())
        {
            vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fence.");

            return VK_FALSE;
        }
    }

    //

	if (lastSwapchain.get())
	{
		lastSwapchain->destroy();
	}

	//

	if (!buildPipeline())
	{
		return VK_FALSE;
	}

	//

	vkts::ICommandBuffersSP updateCmdBuffer = vkts::commandBuffersCreate(contextObject->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	if (!updateCmdBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

		return VK_FALSE;
	}

	//

	auto commandObject = vkts::commandObjectCreate(updateCmdBuffer);

	if (!commandObject.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command object.");

		return VK_FALSE;
	}

	//

	result = updateCmdBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}

	if (!buildMSAAColorTexture(updateCmdBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build MSAA color texture.");

		return VK_FALSE;
	}

	if (!buildMSAADepthTexture(updateCmdBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build MSAA depth texture.");

		return VK_FALSE;
	}

	if (!buildDepthTexture(updateCmdBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build depth texture.");

		return VK_FALSE;
	}

    vkts::SmartPointerVector<vkts::IImageSP> allStageImages;
    vkts::SmartPointerVector<vkts::IBufferSP> allStageBuffers;
    vkts::SmartPointerVector<vkts::IDeviceMemorySP> allStageDeviceMemories;

	if (!font.get())
	{
		guiRenderFactory = vkts::guiRenderFactoryCreate(renderPass, pipelineCache);
		if (!guiRenderFactory.get())
		{
			return VK_FALSE;
		}

	    guiFactory = vkts::guiFactoryCreate(guiRenderFactory);
		if (!guiFactory.get())
		{
			return VK_FALSE;
		}

		guiManager = vkts::guiManagerCreate(VK_FALSE, contextObject, commandObject);
		if (!guiManager.get())
		{
			return VK_FALSE;
		}

		font = vkts::loadFont(VKTS_FONT_DISTANCE_FIELD ? VKTS_FONT_DF_NAME : VKTS_FONT_NAME, guiManager, guiFactory, VKTS_FONT_DISTANCE_FIELD);

		if (!font.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build font.");

			return VK_FALSE;
		}
	}

	result = updateCmdBuffer->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}


	VkSubmitInfo submitInfo{};

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.commandBufferCount = updateCmdBuffer->getCommandBufferCount();
	submitInfo.pCommandBuffers = updateCmdBuffer->getCommandBuffers();
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	result = contextObject->getQueue()->submit(1, &submitInfo, VK_NULL_HANDLE);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

		return VK_FALSE;
	}

	result = contextObject->getQueue()->waitIdle();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for idle queue.");

		return VK_FALSE;
	}

	commandObject->destroy();

	updateCmdBuffer->destroy();

	//

	if (!buildMSAAColorImageView())
	{
		return VK_FALSE;
	}

	if (!buildMSAADepthStencilImageView())
	{
		return VK_FALSE;
	}

	if (!buildDepthStencilImageView())
	{
		return VK_FALSE;
	}

	//

	for (uint32_t i = 0; i < allStageImages.size(); i++)
	{
		allStageImages[i]->destroy();
	}
	for (uint32_t i = 0; i < allStageBuffers.size(); i++)
	{
		allStageBuffers[i]->destroy();
	}
	for (uint32_t i = 0; i < allStageDeviceMemories.size(); i++)
	{
		allStageDeviceMemories[i]->destroy();
	}

	//

	if (sceneLoaded)
	{
		if (scene.get() && environmentScene.get())
		{
			for (uint32_t i = 0; i < swapchainImagesCount; i++)
			{
				if (!updateDescriptorSets(i))
				{
					return VK_FALSE;
				}

				scene->updateDescriptorSetsRecursive(VKTS_BINDING_UNIFORM_BSDF_FORWARD_TOTAL_BINDING_COUNT, writeDescriptorSets, i);

				sphereScene->updateDescriptorSetsRecursive(VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT, environmentWriteDescriptorSets, i);
			}
		}
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

				if (framebuffer[i].get())
				{
					framebuffer[i]->destroy();
				}

				if (swapchainImageView[i].get())
				{
					swapchainImageView[i]->destroy();
				}
			}

			for (uint32_t i = 0; i < allGraphicsPipelines.size(); i++)
			{
				allGraphicsPipelines[i]->destroy();
			}
			allGraphicsPipelines.clear();


			if (depthStencilImageView.get())
			{
				depthStencilImageView->destroy();
			}

			if (msaaDepthStencilImageView.get())
			{
				msaaDepthStencilImageView->destroy();
			}

			if (msaaColorImageView.get())
			{
				msaaColorImageView->destroy();
			}


			if (depthTexture.get())
			{
				depthTexture->destroy();
			}


			if (msaaDepthTexture.get())
			{
				msaaDepthTexture->destroy();
			}

			if (msaaColorTexture.get())
			{
				msaaColorTexture->destroy();
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

	commandPool = vkts::commandPoolCreate(contextObject->getDevice()->getDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, contextObject->getQueue()->getQueueFamilyIndex());

	if (!commandPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get command pool.");

		return VK_FALSE;
	}


	//

	pipelineCache = vkts::pipelineCreateCache(contextObject->getDevice()->getDevice(), 0);

	if (!pipelineCache.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline cache.");

		return VK_FALSE;
	}

	//

    imageAcquiredSemaphore = vkts::semaphoreCreate(contextObject->getDevice()->getDevice(), 0);

    if (!imageAcquiredSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create semaphore.");

        return VK_FALSE;
    }

    renderingCompleteSemaphore = vkts::semaphoreCreate(contextObject->getDevice()->getDevice(), 0);

    if (!renderingCompleteSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create semaphore.");

        return VK_FALSE;
    }

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

	if (!buildRenderPass())
	{
		return VK_FALSE;
	}

	//

	loadTask = ILoadTaskSP(new LoadTask(contextObject, renderPass, allBSDFVertexShaderModules, environmentDescriptorSetLayout, renderFactory, sceneManager, sceneFactory, scene, environmentRenderFactory, environmentSceneManager, environmentSceneFactory, environmentScene, sphereRenderFactory, sphereSceneManager, sphereSceneFactory, sphereScene, sceneName, environmentName));

	if (!loadTask.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create load task.");

		return VK_FALSE;
	}

	if (!updateContext.sendTask(loadTask))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not send load task.");

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
	if (sceneLoaded)
	{
		static VkBool32 pressedA = VK_FALSE;

		if (visualContext->getGamepadButton(windowIndex, 0, VKTS_GAMEPAD_A))
		{
			pressedA = VK_TRUE;
		}
		else if (pressedA && !visualContext->getGamepadButton(windowIndex, 0, VKTS_GAMEPAD_A))
		{
			pressedA = VK_FALSE;

			showStats = !showStats;
		}

		//

		for (uint32_t i = 0; i < allUpdateables.size(); i++)
		{
			allUpdateables[i]->update(updateContext.getDeltaTime(), updateContext.getDeltaTicks(), updateContext.getTickTime());
		}

		//

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

		//

		if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		{
			// Wait until complete, before to commit again.
			result = cmdBufferFence[currentBuffer]->waitForFence(UINT64_MAX);
			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for fence.");

				return VK_FALSE;
			}

			result = cmdBufferFence[currentBuffer]->reset();
			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not reset fence.");

				return VK_FALSE;
			}

			//
			// Set Camera.
			//

			if (scene.get())
			{
				scene->updateTransformRecursive(updateContext.getDeltaTime(), updateContext.getDeltaTicks(), updateContext.getTickTime(), currentBuffer);

				// Camera

				if (!camera.get())
				{
					if (scene->getNumberCameras() > 0)
					{
						camera = vkts::userCameraCreate(scene->getCameras()[0]->getViewMatrix());
					}
					else
					{
						camera = vkts::userCameraCreate(glm::vec4(-1.0f, 1.0f, -2.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
					}

					if (!camera.get())
					{
						return VK_FALSE;
					}

					allUpdateables.append(camera);

					inputController = vkts::inputControllerCreate(updateContext, visualContext, windowIndex, 0, camera);

					if (!inputController.get())
					{
						return VK_FALSE;
					}

					allUpdateables.insert(0, inputController);
				}

				// Lights need to uploaded after the scene has been updated.

				int32_t lightCount = glm::min((int32_t)scene->getLights().size(), VKTS_MAX_LIGHTS);

				if (!fragmentLightsUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_LIGHT].stride * (VkDeviceSize)currentBuffer + VKTS_MAX_LIGHTS * 4 * sizeof(float) * 2, 0, lightCount))
				{
					vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload lights.");

					return VK_FALSE;
				}

				for (int32_t i = 0; i < lightCount; i++)
				{
					if (!fragmentLightsUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_LIGHT].stride * (VkDeviceSize)currentBuffer + i * 4 * sizeof(float), 0, scene->getLights()[i]->getDirection()))
					{
						vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload lights.");

						return VK_FALSE;
					}

					if (!fragmentLightsUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_LIGHT].stride * (VkDeviceSize)currentBuffer + i * 4 * sizeof(float) + VKTS_MAX_LIGHTS * 4 * sizeof(float), 0, glm::vec4(scene->getLights()[i]->getColor(), 1.0)))
					{
						vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload lights.");

						return VK_FALSE;
					}
				}
			}

			if (!camera.get())
			{
				return VK_FALSE;
			}

			//

			glm::mat4 projectionMatrix(1.0f);
			glm::mat4 viewMatrix(1.0f);

			const auto& currentExtent = surface->getCurrentExtent(contextObject->getPhysicalDevice()->getPhysicalDevice(), VK_FALSE);

			if (scene->getNumberCameras() > 0)
			{
				scene->getCameras()[0]->setWindowDimension(glm::ivec2((int32_t)currentExtent.width, (int32_t)currentExtent.height));

				projectionMatrix = scene->getCameras()[0]->getProjectionMatrix();
			}
			else
			{
				projectionMatrix = vkts::perspectiveMat4(45.0f, (float)currentExtent.width / (float)currentExtent.height, 0.1f, 1000.0f);
			}

			auto inverseProjectionMatrix = glm::inverse(projectionMatrix);

			viewMatrix = camera->getViewMatrix();

			auto inverseViewMatrix = glm::inverse(viewMatrix);

			if (!vertexViewProjectionUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION].stride * (VkDeviceSize)currentBuffer + 0 * sizeof(float) * 16, 0, projectionMatrix))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

				return VK_FALSE;
			}
			if (!environmentVertexViewProjectionUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION].stride * (VkDeviceSize)currentBuffer + 0 * sizeof(float) * 16, 0, projectionMatrix))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

				return VK_FALSE;
			}
			if (!fragmentMatricesUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_BSDF_FORWARD_INVERSE].stride * (VkDeviceSize)currentBuffer + 0 * sizeof(float) * 16, 0, inverseProjectionMatrix))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

				return VK_FALSE;
			}

			//

			if (!vertexViewProjectionUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION].stride * (VkDeviceSize)currentBuffer + 1 * sizeof(float) * 16, 0, viewMatrix))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

				return VK_FALSE;
			}
			if (!fragmentMatricesUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_BSDF_FORWARD_INVERSE].stride * (VkDeviceSize)currentBuffer + 1 * sizeof(float) * 16, 0, inverseViewMatrix))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

				return VK_FALSE;
			}

			// Environment is not moveable.
			auto fixedViewMatrix = viewMatrix;
			fixedViewMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			if (!environmentVertexViewProjectionUniformBuffer->upload(dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION].stride * (VkDeviceSize)currentBuffer + 1 * sizeof(float) * 16, 0, fixedViewMatrix))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

				return VK_FALSE;
			}

			if (sphereScene.get())
			{
				sphereScene->updateTransformRecursive(updateContext.getDeltaTime(), updateContext.getDeltaTicks(), updateContext.getTickTime(), currentBuffer);
			}

			//

			uint32_t currentFPS;

			if (vkts::profileApplicationGetFps(currentFPS, updateContext.getDeltaTime()))
			{
				fps = currentFPS;

				//

				uint64_t currentRAM;

				if (vkts::profileApplicationGetRam(currentRAM))
				{
					ram = currentRAM;
				}

				//

				float currentCpuUsageApp;

				if (vkts::profileApplicationGetCpuUsage(currentCpuUsageApp))
				{
					cpuUsageApp = currentCpuUsageApp;
				}

				//

				for (uint32_t cpu = 0; cpu < processors; cpu++)
				{
					if (vkts::profileGetCpuUsage(currentCpuUsageApp, cpu))
					{
						cpuUsage[cpu] = currentCpuUsageApp;
					}
				}

				//

				rebuildCmdBufferCounter = swapchainImagesCount;
			}

			if (rebuildCmdBufferCounter > 0)
			{
				if (!buildCmdBuffer(currentBuffer))
				{
					return VK_FALSE;
				}

				rebuildCmdBufferCounter--;
			}

			//

			VkSemaphore waitSemaphores = imageAcquiredSemaphore->getSemaphore();
			VkSemaphore signalSemaphores = renderingCompleteSemaphore->getSemaphore();


			VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

			VkSubmitInfo submitInfo{};

			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &waitSemaphores;
			submitInfo.pWaitDstStageMask = &waitDstStageMask;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = cmdBuffer[currentBuffer]->getCommandBuffers();
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &signalSemaphores;

			result = contextObject->getQueue()->submit(1, &submitInfo, cmdBufferFence[currentBuffer]->getFence());

			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

				return VK_FALSE;
			}

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
	}
	else
	{
		vkts::ITaskSP executedTask;

		// Do not wait.
		if (!updateContext.receiveExecutedTask(executedTask, VK_FALSE))
		{
			return VK_TRUE;
		}

		if (!loadTask->getSuccess())
		{
			return VK_FALSE;
		}

		//

		vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Scene loaded");

		sceneLoaded = VK_TRUE;

		//

		VkCommandBuffer updateCommandBuffer = loadTask->getCommandBuffer();

		//

		VkSubmitInfo submitInfo{};

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &updateCommandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		auto result = contextObject->getQueue()->submit(1, &submitInfo, VK_NULL_HANDLE);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

			return VK_FALSE;
		}

		result = contextObject->getQueue()->waitIdle();

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for idle queue.");

			return VK_FALSE;
		}

		//

		// Enlarge the sphere.
		sphereScene->getObjects()[0]->setScale(glm::vec3(100.0f, 100.0f, 100.0f));

		// Sorted by binding
		dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION] = VkTsDynamicOffset{0, (uint32_t)contextObject->getPhysicalDevice()->getUniformBufferAlignmentSizeInBytes(vkts::alignmentGetSizeInBytes(16 * sizeof(float) * 2, 16))};
		dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM] = VkTsDynamicOffset{0, (uint32_t)sceneFactory->getSceneRenderFactory()->getTransformUniformBufferAlignmentSize(sceneManager)};
		dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM] = VkTsDynamicOffset{0, (uint32_t)sceneFactory->getSceneRenderFactory()->getJointsUniformBufferAlignmentSize(sceneManager)};
		dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_BSDF_FORWARD_INVERSE] = VkTsDynamicOffset{0, (uint32_t)contextObject->getPhysicalDevice()->getUniformBufferAlignmentSizeInBytes(vkts::alignmentGetSizeInBytes(16 * sizeof(float) * 2, 16))};
		dynamicOffsets[VKTS_BINDING_UNIFORM_BUFFER_LIGHT] = VkTsDynamicOffset{0, (uint32_t)contextObject->getPhysicalDevice()->getUniformBufferAlignmentSizeInBytes(vkts::alignmentGetSizeInBytes(VKTS_MAX_LIGHTS * 4 * sizeof(float) * 2 + sizeof(int32_t), 16))};

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

		//

		if (environmentSceneFactory.get())
		{
			environmentSceneFactory.reset();
		}

		if (environmentSceneManager.get())
		{
			environmentSceneManager->destroy();

			environmentSceneManager.reset();
		}

		if (environmentRenderFactory.get())
		{
			environmentRenderFactory.reset();
		}

		// Destroys the load task.
		loadTask = ILoadTaskSP();

		//

		if (scene.get())
		{
			for (uint32_t i = 0; i < swapchainImagesCount; i++)
			{
				if (!updateDescriptorSets(i))
				{
					return VK_FALSE;
				}

				scene->updateDescriptorSetsRecursive(VKTS_BINDING_UNIFORM_BSDF_FORWARD_TOTAL_BINDING_COUNT, writeDescriptorSets, i);

				sphereScene->updateDescriptorSetsRecursive(VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT, environmentWriteDescriptorSets, i);
			}
		}

		//
		// glTF encoding.
		//

		if (outputSceneName != "")
		{
			char directory[VKTS_MAX_BUFFER_CHARS];
			char filename[VKTS_MAX_BUFFER_CHARS];

			vkts::fileGetDirectory(directory, outputSceneName.c_str());
			vkts::fileGetFilename(filename, outputSceneName.c_str());

			vkts::GltfParameter glTFparameter;

			scene->updateParameterRecursive(&glTFparameter);

			// Enable to store
			glTFparameter.save(filename, directory);
		}
	}

	return VK_TRUE;
}

//
// Vulkan termination.
//
void Example::terminate(const vkts::IUpdateThreadContext& updateContext)
{
	if (loadTask.get() && !loadTask->getSuccess() && !sceneLoaded)
	{
		vkts::ITaskSP executedTask;

		// Wait, until finished.
		updateContext.receiveExecutedTask(executedTask);

		loadTask = ILoadTaskSP();
	}

	if (contextObject.get())
	{
		if (contextObject->getDevice().get())
		{
			terminateResources(updateContext);

			for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
			{
				if (cmdBuffer[i].get())
				{
					cmdBuffer[i]->destroy();
				}
			}

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

			if (environmentSceneFactory.get())
			{
				environmentSceneFactory.reset();
			}

			if (environmentSceneManager.get())
			{
				environmentSceneManager->destroy();

				environmentSceneManager.reset();
			}

			if (environmentRenderFactory.get())
			{
				environmentRenderFactory.reset();
			}

			if (environmentScene.get())
			{
				environmentScene->destroy();
			}

			if (sphereSceneFactory.get())
			{
				sphereSceneFactory.reset();
			}

			if (sphereSceneManager.get())
			{
				sphereSceneManager->destroy();

				sphereSceneManager.reset();
			}

			if (sphereRenderFactory.get())
			{
				sphereRenderFactory.reset();
			}

			if (sphereScene.get())
			{
				sphereScene->destroy();
			}

			if (font.get())
			{
				font->destroy();
			}

			if (guiManager.get())
			{
				guiManager->destroy();
			}

			if (guiFactory.get())
			{
				guiFactory.reset();
			}

			if (guiRenderFactory.get())
			{
				guiRenderFactory.reset();
			}

			if (swapchain.get())
			{
				swapchain->destroy();
			}

			if (renderPass.get())
			{
				renderPass->destroy();
			}

			if (environmentPipelineLayout.get())
			{
				environmentPipelineLayout->destroy();
			}

			if (envVertexShaderModule.get())
			{
				envVertexShaderModule->destroy();
			}

			if (envFragmentShaderModule.get())
			{
				envFragmentShaderModule->destroy();
			}

			for (uint32_t i = 0; i < allBSDFVertexShaderModules.size(); i++)
			{
				allBSDFVertexShaderModules[i]->destroy();
			}
			allBSDFVertexShaderModules.clear();

			if (fragmentLightsUniformBuffer.get())
			{
				fragmentLightsUniformBuffer->destroy();
			}

			if (fragmentMatricesUniformBuffer.get())
			{
				fragmentMatricesUniformBuffer->destroy();
			}

			if (environmentVertexViewProjectionUniformBuffer.get())
			{
				environmentVertexViewProjectionUniformBuffer->destroy();
			}

			if (vertexViewProjectionUniformBuffer.get())
			{
				vertexViewProjectionUniformBuffer->destroy();
			}

			if (environmentDescriptorSetLayout.get())
			{
				environmentDescriptorSetLayout->destroy();
			}

	        if (renderingCompleteSemaphore.get())
	        {
	            renderingCompleteSemaphore->destroy();
	        }

	        if (imageAcquiredSemaphore.get())
	        {
	            imageAcquiredSemaphore->destroy();
	        }

			if (pipelineCache.get())
			{
				pipelineCache->destroy();
			}

			if (commandPool.get())
			{
				commandPool->destroy();
			}
		}
	}
}
