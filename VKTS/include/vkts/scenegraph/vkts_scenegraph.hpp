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

#ifndef VKTS_SCENEGRAPH_HPP_
#define VKTS_SCENEGRAPH_HPP_

/**
 *
 * Depends on VKTS entity and Vulkan object.
 *
 */

#include <vkts/vulkan/composition/vkts_composition.hpp>

#include <vkts/scenegraph/fn_bindings.hpp>
#include <vkts/entity/vkts_entity.hpp>

#define VKTS_BSDF_LENGTH 512
#define VKTS_BSDF_SAMPLES 256
#define VKTS_BSDF_SAMPLES_CPU_CUBE_MAP 1024
#define VKTS_BSDF_SAMPLES_GPU_CUBE_MAP 512

#define VKTS_CONVERT_BEZIER VK_TRUE
#define VKTS_CONVERT_SAMPLING (1.0f/60.0f)

#define VKTS_SHADER_DIRECTORY "shader/SPIR/V/"
#define VKTS_TEXTURE_DIRECTORY "texture/"

/**
 * Types.
 */

typedef enum VkTsTargetTransform_
{
    VKTS_TARGET_TRANSFORM_TRANSLATE = 0,
    VKTS_TARGET_TRANSFORM_ROTATE = 1,
    VKTS_TARGET_TRANSFORM_QUATERNION_ROTATE = 2,
    VKTS_TARGET_TRANSFORM_SCALE = 3
} VkTsTargetTransform;

typedef enum VkTsTargetTransformElement_
{
    VKTS_TARGET_TRANSFORM_ELEMENT_X = 0,
    VKTS_TARGET_TRANSFORM_ELEMENT_Y = 1,
    VKTS_TARGET_TRANSFORM_ELEMENT_Z = 2,
	VKTS_TARGET_TRANSFORM_ELEMENT_W = 3
} VkTsTargetTransformElement;

typedef enum VkTsRotationMode_
{
    VKTS_EULER_YXZ = 0,
    VKTS_EULER_XYZ = 1,
    VKTS_EULER_XZY = 2
} VkTsRotationMode;

typedef enum VkTsInterpolator_
{
    VKTS_INTERPOLATOR_CONSTANT = 0,
    VKTS_INTERPOLATOR_LINEAR = 1,
    VKTS_INTERPOLATOR_BEZIER = 2,
	VKTS_INTERPOLATOR_CATMULLROMSPLINE = 3,
	VKTS_INTERPOLATOR_CUBICSPLINE = 4
} VkTsInterpolator;

/**
 * Parameter set.
 */

#include <vkts/scenegraph/parameter/Parameter.hpp>

/**
 * Overwrite.
 */

#include <vkts/scenegraph/overwrite/OverwriteDraw.hpp>
#include <vkts/scenegraph/overwrite/OverwriteUpdate.hpp>

/**
 * Scene.
 */

#include <vkts/scenegraph/scene/IChannel.hpp>

#include <vkts/scenegraph/scene/IMarker.hpp>

#include <vkts/scenegraph/scene/IRenderMaterial.hpp>
#include <vkts/scenegraph/scene/IBSDFMaterial.hpp>
#include <vkts/scenegraph/scene/IPhongMaterial.hpp>

#include <vkts/scenegraph/scene/IRenderSubMesh.hpp>
#include <vkts/scenegraph/scene/ISubMesh.hpp>

#include <vkts/scenegraph/scene/IAnimation.hpp>

#include <vkts/scenegraph/scene/IConstraint.hpp>

#include <vkts/scenegraph/scene/ICamera.hpp>

#include <vkts/scenegraph/scene/ILight.hpp>

#include <vkts/scenegraph/scene/IMesh.hpp>

#include <vkts/scenegraph/scene/IRenderNode.hpp>
#include <vkts/scenegraph/scene/INode.hpp>

#include <vkts/scenegraph/scene/IObject.hpp>

#include <vkts/scenegraph/scene/IScene.hpp>


#include <vkts/scenegraph/scene/ICopyConstraint.hpp>

#include <vkts/scenegraph/scene/ILimitConstraint.hpp>

/**
 * Shader.
 */

#include <vkts/scenegraph/shader/fn_shader_factory.hpp>

/**
 * Scene manager.
 */

#include <vkts/scenegraph/manager/ISceneManager.hpp>

#include <vkts/scenegraph/manager/fn_scene_manager.hpp>

/**
 * Scene factory.
 */

#include <vkts/scenegraph/factory/ISceneRenderFactory.hpp>
#include <vkts/scenegraph/factory/ISceneFactory.hpp>

#include <vkts/scenegraph/factory/fn_scene_factory.hpp>

/**
 * Scene load.
 */

#include <vkts/scenegraph/load/fn_gltf_load.hpp>
#include <vkts/scenegraph/load/fn_scene_load.hpp>

/**
 * Parameter setting.
 */

#include <vkts/scenegraph/parameter/GltfParameter.hpp>
#include <vkts/scenegraph/parameter/AnimationParameter.hpp>

/**
 * Interpolator.
 */

#include <vkts/scenegraph/interpolator/fn_interpolator.hpp>

#endif /* VKTS_SCENEGRAPH_HPP_ */
