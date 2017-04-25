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

#include "SceneFactory.hpp"

#include "../scene/Animation.hpp"
#include "../scene/BSDFMaterial.hpp"
#include "../scene/Camera.hpp"
#include "../scene/Channel.hpp"
#include "../scene/CopyConstraint.hpp"
#include "../scene/Light.hpp"
#include "../scene/LimitConstraint.hpp"
#include "../scene/Marker.hpp"
#include "../scene/Mesh.hpp"
#include "../scene/Node.hpp"
#include "../scene/Object.hpp"
#include "../scene/PhongMaterial.hpp"
#include "../scene/Scene.hpp"
#include "../scene/SubMesh.hpp"



namespace vkts
{

SceneFactory::SceneFactory(const ISceneRenderFactorySP& sceneRenderFactory, const VkBool32 gpu) :
	ISceneFactory(), sceneRenderFactory(sceneRenderFactory), gpu(gpu)
{
}

SceneFactory::~SceneFactory()
{
}

//
// ISceneFactory
//


ISceneRenderFactorySP SceneFactory::getSceneRenderFactory() const
{
	return sceneRenderFactory;
}

IPhongMaterialSP SceneFactory::createPhongMaterial(const ISceneManagerSP& sceneManager, const VkBool32 forwardRendering)
{
	auto material = IPhongMaterialSP(new PhongMaterial(forwardRendering));

	if (!material.get())
	{
		return IPhongMaterialSP();
	}

	for (VkDeviceSize i = 0; i < sceneRenderFactory->getBufferCount(); i++)
	{
		auto renderMaterial = sceneRenderFactory->createRenderMaterial(sceneManager);

		if (!renderMaterial.get())
		{
			return IPhongMaterialSP();
		}

		material->addRenderMaterial(renderMaterial);
	}

	return material;
}

//

IBSDFMaterialSP SceneFactory::createBSDFMaterial(const ISceneManagerSP& sceneManager, const VkBool32 forwardRendering)
{
	auto material = IBSDFMaterialSP(new BSDFMaterial(forwardRendering));

	if (!material.get())
	{
		return IBSDFMaterialSP();
	}

	for (VkDeviceSize i = 0; i < sceneRenderFactory->getBufferCount(); i++)
	{
		auto renderMaterial = sceneRenderFactory->createRenderMaterial(sceneManager);

		if (!renderMaterial.get())
		{
			return IBSDFMaterialSP();
		}

		material->addRenderMaterial(renderMaterial);
	}

	return material;
}

//

ISubMeshSP SceneFactory::createSubMesh(const ISceneManagerSP& sceneManager)
{
	auto subMesh = ISubMeshSP(new SubMesh());

	if (!subMesh.get())
	{
		return ISubMeshSP();
	}

	auto subMeshData = sceneRenderFactory->createRenderSubMesh(sceneManager);

	if (!subMeshData.get())
	{
		return ISubMeshSP();
	}

	subMesh->setRenderSubMesh(subMeshData);

	return subMesh;
}

IMeshSP SceneFactory::createMesh(const ISceneManagerSP& sceneManager)
{
	return IMeshSP(new Mesh());
}

//

IChannelSP SceneFactory::createChannel(const ISceneManagerSP& sceneManager)
{
	return IChannelSP(new Channel());
}

IMarkerSP SceneFactory::createMarker(const ISceneManagerSP& sceneManager)
{
	return IMarkerSP(new Marker());
}

IAnimationSP SceneFactory::createAnimation(const ISceneManagerSP& sceneManager)
{
	return IAnimationSP(new Animation());
}

//

ICameraSP SceneFactory::createCamera(const ISceneManagerSP& sceneManager)
{
	return ICameraSP(new Camera());
}

ILightSP SceneFactory::createLight(const ISceneManagerSP& sceneManager)
{
	return ILightSP(new Light());
}

//

IConstraintSP SceneFactory::createCopyConstraint(const ISceneManagerSP& sceneManager, const enum CopyConstraintType copyConstraintType)
{
	return IConstraintSP(new CopyConstraint(copyConstraintType));
}

IConstraintSP SceneFactory::createLimitConstraint(const ISceneManagerSP& sceneManager, const enum LimitConstraintType limitConstraintType)
{
	return IConstraintSP(new LimitConstraint(limitConstraintType));
}

//

INodeSP SceneFactory::createNode(const ISceneManagerSP& sceneManager)
{
	auto node = INodeSP(new Node());

	if (!node.get())
	{
		return INodeSP();
	}

	for (VkDeviceSize i = 0; i < sceneRenderFactory->getBufferCount(); i++)
	{
		auto renderNode = sceneRenderFactory->createRenderNode(sceneManager);

		if (!renderNode.get())
		{
			return INodeSP();
		}

		node->addRenderNode(renderNode);
	}

	return node;
}

//

IObjectSP SceneFactory::createObject(const ISceneManagerSP& sceneManager)
{
	return IObjectSP(new Object());
}

//

ISceneSP SceneFactory::createScene(const ISceneManagerSP& sceneManager)
{
	return ISceneSP(new Scene());
}

//

VkBool32 SceneFactory::useGPU() const
{
	return gpu;
}

} /* namespace vkts */
