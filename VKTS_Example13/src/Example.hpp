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

#ifndef EXAMPLE_HPP_
#define EXAMPLE_HPP_

#include <vkts/vkts.hpp>

#include "LoadTask.hpp"

#define VKTS_EXAMPLE_NAME "Example13"

#define VKTS_SAMPLE_COUNT_BIT VK_SAMPLE_COUNT_4_BIT

#define VKTS_NUMBER_BUFFERS 2

#define VKTS_BINDING_VERTEX_BUFFER 0

#define VKTS_MAX_LIGHTS 16

#define VKTS_FONT_DISTANCE_FIELD VK_TRUE
#define VKTS_FONT_NAME "font/Arial_32.fnt"
#define VKTS_FONT_DF_NAME "font/Arial_32_DF.fnt"
#define VKTS_FONT_SIZE 16.0f
#define VKTS_FONT_COLOR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)


#define VKTS_BSDF0_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf.vert.spv"
#define VKTS_BSDF1_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_no_texcoord.vert.spv"
#define VKTS_BSDF2_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_tangents.vert.spv"
#define VKTS_BSDF3_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_skinning.vert.spv"
#define VKTS_BSDF4_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_tangents_two_texcoord.vert.spv"
#define VKTS_BSDF5_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_skinning_two_texcoord.vert.spv"
#define VKTS_BSDF6_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_tangents_no_texcoord.vert.spv"


#define VKTS_ENV_VERTEX_SHADER_NAME "shader/SPIR/V/environment.vert.spv"
#define VKTS_ENV_FRAGMENT_SHADER_NAME "shader/SPIR/V/environment.frag.spv"

#define VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT (VKTS_BINDING_UNIFORM_TRANSFORM_BINDING_COUNT + VKTS_BINDING_UNIFORM_ENVIRONMENT_COUNT + VKTS_BINDING_UNIFORM_LIGHTING_BINDING_COUNT)

#define VKTS_MAX_CORES 32u

#define VKTS_OUTPUT_BUFFER_SIZE 1024

#define VKTS_BSDF_DESCRIPTOR_SET_COUNT (11 + 1 + 1 + 1)

class Example: public vkts::IUpdateThread
{

private:

	const vkts::IContextObjectSP contextObject;

	const int32_t windowIndex;

	const vkts::IVisualContextSP visualContext;

	const vkts::ISurfaceSP surface;

	VkBool32 showStats;

	vkts::IUserCameraSP camera;
	vkts::IInputControllerSP inputController;

	vkts::SmartPointerVector<vkts::IUpdateableSP> allUpdateables;

	vkts::ICommandPoolSP commandPool;
	vkts::IPipelineCacheSP pipelineCache;

    vkts::ISemaphoreSP imageAcquiredSemaphore;
    vkts::ISemaphoreSP renderingCompleteSemaphore;

	vkts::IDescriptorSetLayoutSP environmentDescriptorSetLayout;

    VkDescriptorBufferInfo environmentDescriptorBufferInfos[1];
    VkDescriptorBufferInfo descriptorBufferInfos[1 + 1 + 1];
    VkDescriptorImageInfo environmentDescriptorImageInfos[1];
    VkDescriptorImageInfo descriptorImageInfos[VKTS_BSDF_DESCRIPTOR_SET_COUNT];

    VkWriteDescriptorSet writeDescriptorSets[VKTS_BINDING_UNIFORM_BSDF_FORWARD_TOTAL_BINDING_COUNT];
    VkWriteDescriptorSet environmentWriteDescriptorSets[VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT];

    std::map<uint32_t, VkTsDynamicOffset> dynamicOffsets;

    vkts::IBufferObjectSP vertexViewProjectionUniformBuffer;
	vkts::IBufferObjectSP environmentVertexViewProjectionUniformBuffer;
	vkts::IBufferObjectSP fragmentLightsUniformBuffer;
	vkts::IBufferObjectSP fragmentMatricesUniformBuffer;

	vkts::SmartPointerVector<vkts::IShaderModuleSP> allBSDFVertexShaderModules;

	vkts::IShaderModuleSP envVertexShaderModule;
	vkts::IShaderModuleSP envFragmentShaderModule;

	vkts::IPipelineLayoutSP environmentPipelineLayout;

    vkts::IGuiRenderFactorySP guiRenderFactory;
	vkts::IGuiManagerSP guiManager;
	vkts::IGuiFactorySP guiFactory;
    vkts::IFontSP font;

	ILoadTaskSP loadTask;

	VkBool32 sceneLoaded;
    vkts::ISceneRenderFactorySP renderFactory;
	vkts::ISceneManagerSP sceneManager;
	vkts::ISceneFactorySP sceneFactory;
	vkts::ISceneSP scene;

    vkts::ISceneRenderFactorySP environmentRenderFactory;
	vkts::ISceneManagerSP environmentSceneManager;
	vkts::ISceneFactorySP environmentSceneFactory;
	vkts::ISceneSP environmentScene;

    vkts::ISceneRenderFactorySP sphereRenderFactory;
	vkts::ISceneManagerSP sphereSceneManager;
	vkts::ISceneFactorySP sphereSceneFactory;
	vkts::ISceneSP sphereScene;

	vkts::ISwapchainSP swapchain;

	vkts::IRenderPassSP renderPass;

	vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> allGraphicsPipelines;

	vkts::IImageObjectSP depthTexture;
	vkts::IImageObjectSP msaaColorTexture;
	vkts::IImageObjectSP msaaDepthTexture;

	vkts::IImageViewSP depthStencilImageView;
	vkts::IImageViewSP msaaColorImageView;
	vkts::IImageViewSP msaaDepthStencilImageView;

    uint32_t swapchainImagesCount;

    vkts::SmartPointerVector<vkts::IImageViewSP> swapchainImageView;

    vkts::SmartPointerVector<vkts::IFramebufferSP> framebuffer;

    vkts::SmartPointerVector<vkts::ICommandBuffersSP> cmdBuffer;

    vkts::SmartPointerVector<vkts::IFenceSP> cmdBufferFence;

    uint32_t rebuildCmdBufferCounter;

    uint32_t fps;
    uint64_t ram;
    float cpuUsageApp;
    uint32_t processors;
    float cpuUsage[VKTS_MAX_CORES];

    std::string sceneName;

    std::string outputSceneName;

    std::string environmentName;

	VkBool32 buildCmdBuffer(const int32_t usedBuffer);

	VkBool32 buildFramebuffer(const int32_t usedBuffer);

	VkBool32 updateDescriptorSets(const int32_t usedBuffer);

	VkBool32 buildSwapchainImageView(const int32_t usedBuffer);

	VkBool32 buildMSAADepthStencilImageView();

	VkBool32 buildMSAAColorImageView();

	VkBool32 buildDepthStencilImageView();

	VkBool32 buildMSAADepthTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildMSAAColorTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildDepthTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildPipeline();

	VkBool32 buildRenderPass();

	VkBool32 buildPipelineLayout();

	VkBool32 buildDescriptorSetLayout();

	VkBool32 buildShader();

	VkBool32 buildUniformBuffers();

	VkBool32 buildResources(const vkts::IUpdateThreadContext& updateContext);

	void terminateResources(const vkts::IUpdateThreadContext& updateContext);

public:

	Example(const vkts::IContextObjectSP& contextObject, const int32_t windowIndex, const vkts::IVisualContextSP& visualContext, const vkts::ISurfaceSP& surface, const std::string& sceneName, const std::string& outputSceneName, const std::string& environmentName);

	virtual ~Example();

	//
	// Vulkan initialization.
	//
	virtual VkBool32 init(const vkts::IUpdateThreadContext& updateContext);

	//
	// Vulkan update.
	//
	virtual VkBool32 update(const vkts::IUpdateThreadContext& updateContext);

	//
	// Vulkan termination.
	//
	virtual void terminate(const vkts::IUpdateThreadContext& updateContext);

};

#endif /* EXAMPLE_HPP_ */
