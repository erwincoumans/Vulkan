/**
 * VKTS - VulKan ToolS.
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

#ifndef VKTS_VKTS_WRAPPER_HPP_
#define VKTS_VKTS_WRAPPER_HPP_

/**
 *
 * Depends on VKTS core.
 *
 */

#include <vkts/core/vkts_core.hpp>

/**
 *
 * VKTS Start.
 *
 */

/**
 * Defines.
 */

#define VKTS_ENGINE_NAME            "VKTS"
#define VKTS_ENGINE_MAJOR           1
#define VKTS_ENGINE_MINOR           0
#define VKTS_ENGINE_PATCH           46
#define VKTS_ENGINE_REVISION        0

/**
 * Types.
 */

typedef enum VkTsVertexBufferTypeBits_
{
    VKTS_VERTEX_BUFFER_TYPE_VERTEX = 0x00000001,
    VKTS_VERTEX_BUFFER_TYPE_NORMAL = 0x00000002,
    VKTS_VERTEX_BUFFER_TYPE_BITANGENT = 0x00000004,
    VKTS_VERTEX_BUFFER_TYPE_TANGENT = 0x00000008,
    VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0 = 0x00000010,
    VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1 = 0x00000020,
    VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0 = 0x00000040,
    VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1 = 0x00000080,
    VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0 = 0x00000100,
    VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1 = 0x00000200,
    VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS = 0x00000400,

    VKTS_VERTEX_BUFFER_TYPE_TANGENTS = VKTS_VERTEX_BUFFER_TYPE_NORMAL | VKTS_VERTEX_BUFFER_TYPE_BITANGENT | VKTS_VERTEX_BUFFER_TYPE_TANGENT,

    VKTS_VERTEX_BUFFER_TYPE_BONES = VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0 | VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1 | VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0 | VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1 | VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS
} VkTsVertexBufferTypeBits;

typedef VkFlags VkTsVertexBufferType;

typedef enum VkTsAttributesBits_
{
    VKTS_ATTRIBUTE_POSITION 	= 0x00000001,
    VKTS_ATTRIBUTE_NORMAL 		= 0x00000002,
    VKTS_ATTRIBUTE_TANGENT 		= 0x00000004,
    VKTS_ATTRIBUTE_TEXCOORD_0 	= 0x00000008,
    VKTS_ATTRIBUTE_TEXCOORD_1 	= 0x00000010,
    VKTS_ATTRIBUTE_COLOR_0 		= 0x00000020,
    VKTS_ATTRIBUTE_COLOR_1 		= 0x00000040,
    VKTS_ATTRIBUTE_JOINTS_0 	= 0x00000080,
    VKTS_ATTRIBUTE_JOINTS_1 	= 0x00000100,
    VKTS_ATTRIBUTE_WEIGHTS_0 	= 0x00000200,
    VKTS_ATTRIBUTE_WEIGHTS_1 	= 0x00000400,
} VkTsAttributesBits;

typedef VkFlags VkTsAttributes;

typedef enum VkTsMaterial_
{
    VKTS_MATERIAL_METAL_ROUGHNESS,
	VKTS_MATERIAL_SPECULAR_GLOSSINESS,
	VKTS_MATERIAL_COMMON
} VkTsMaterial;

typedef enum VkTsRenderer_
{
    VKTS_RENDERER_FORWARD,
    VKTS_RENDERER_DEFERRED
} VkTsRenderer;

typedef struct VkTsStructureTypeHeader_
{
    VkStructureType sType;
    const void* pNext;
} VkTsStructureTypeHeader;

/**
 * Alignment.
 */

#include <vkts/vulkan/wrapper/alignment/fn_alignment.hpp>

/**
 * Serialize.
 */

#include <vkts/vulkan/wrapper/serialize/fn_serialize.hpp>

/**
 * Extensions and layers.
 */

#include <vkts/vulkan/wrapper/extension/fn_extension.hpp>

#include <vkts/vulkan/wrapper/extension/fn_layer.hpp>

#include <vkts/vulkan/wrapper/extension/fn_debug.hpp>
#include <vkts/vulkan/wrapper/extension/fn_validation.hpp>

/**
 * Instance.
 */

#include <vkts/vulkan/wrapper/instance/IInstance.hpp>

#include <vkts/vulkan/wrapper/instance/fn_instance.hpp>

/**
 * Physical device.
 */

#include <vkts/vulkan/wrapper/physical_device/IPhysicalDevice.hpp>

#include <vkts/vulkan/wrapper/physical_device/fn_physical_device.hpp>

/**
 * Device.
 */

#include <vkts/vulkan/wrapper/device/IDevice.hpp>

#include <vkts/vulkan/wrapper/device/fn_device.hpp>

/**
 * Queue.
 */

#include <vkts/vulkan/wrapper/queue/IQueue.hpp>

#include <vkts/vulkan/wrapper/queue/fn_queue.hpp>

/**
 * Command.
 */

#include <vkts/vulkan/wrapper/command/ICommandBuffers.hpp>
#include <vkts/vulkan/wrapper/command/ICommandPool.hpp>

#include <vkts/vulkan/wrapper/command/fn_command.hpp>

/**
 * Fence.
 */

#include <vkts/vulkan/wrapper/fence/IFence.hpp>

#include <vkts/vulkan/wrapper/fence/fn_fence.hpp>

/**
 * Event.
 */

#include <vkts/vulkan/wrapper/event/IEvent.hpp>

#include <vkts/vulkan/wrapper/event/fn_event.hpp>

/**
 * Semaphore.
 */

#include <vkts/vulkan/wrapper/semaphore/ISemaphore.hpp>

#include <vkts/vulkan/wrapper/semaphore/fn_semaphore.hpp>

/**
 * Shader module.
 */

#include <vkts/vulkan/wrapper/shader_module/IShaderModule.hpp>

#include <vkts/vulkan/wrapper/shader_module/fn_shader_module.hpp>

/**
 * Buffer.
 */

#include <vkts/vulkan/wrapper/buffer/IBuffer.hpp>
#include <vkts/vulkan/wrapper/buffer/IBufferView.hpp>

#include <vkts/vulkan/wrapper/buffer/fn_buffer.hpp>

/**
 * Image.
 */

#include <vkts/vulkan/wrapper/image/IImage.hpp>

#include <vkts/vulkan/wrapper/image/fn_image.hpp>

/**
 * Image view.
 */

#include <vkts/vulkan/wrapper/image_view/IImageView.hpp>

#include <vkts/vulkan/wrapper/image_view/fn_image_view.hpp>

/**
 * Sampler.
 */

#include <vkts/vulkan/wrapper/sampler/ISampler.hpp>

#include <vkts/vulkan/wrapper/sampler/fn_sampler.hpp>

/**
 * Device memory.
 */

#include <vkts/vulkan/wrapper/device_memory/IDeviceMemory.hpp>

#include <vkts/vulkan/wrapper/device_memory/fn_device_memory.hpp>

/**
 * Descriptor.
 */

#include <vkts/vulkan/wrapper/descriptor/IDescriptorSetLayout.hpp>
#include <vkts/vulkan/wrapper/descriptor/IDescriptorPool.hpp>
#include <vkts/vulkan/wrapper/descriptor/IDescriptorSets.hpp>

#include <vkts/vulkan/wrapper/descriptor/fn_descriptor.hpp>

/**
 * Render pass.
 */

#include <vkts/vulkan/wrapper/render_pass/IRenderPass.hpp>

#include <vkts/vulkan/wrapper/render_pass/fn_render_pass.hpp>

/**
 * Frame buffer.
 */

#include <vkts/vulkan/wrapper/framebuffer/IFramebuffer.hpp>

#include <vkts/vulkan/wrapper/framebuffer/fn_framebuffer.hpp>

/**
 * Pipeline.
 */

#include <vkts/vulkan/wrapper/pipeline/IComputePipeline.hpp>
#include <vkts/vulkan/wrapper/pipeline/IGraphicsPipeline.hpp>
#include <vkts/vulkan/wrapper/pipeline/IPipelineCache.hpp>
#include <vkts/vulkan/wrapper/pipeline/IPipelineLayout.hpp>

#include <vkts/vulkan/wrapper/pipeline/DefaultComputePipeline.hpp>
#include <vkts/vulkan/wrapper/pipeline/DefaultGraphicsPipeline.hpp>

#include <vkts/vulkan/wrapper/pipeline/fn_pipeline.hpp>

#endif /* VKTS_VKTS_WRAPPER_HPP_ */
