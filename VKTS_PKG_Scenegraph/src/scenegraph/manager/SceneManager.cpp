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

#include "SceneManager.hpp"

namespace vkts
{

SceneManager::SceneManager(const IAssetManagerSP& assetManager) :
    ISceneManager(), assetManager(assetManager), allObjects(), allCameras(), allLights(), allMeshes(), allSubMeshes(), allAnimations(), allChannels(), allBSDFMaterials(), allUsedBSDFMaterials(), allPhongMaterials(), allUsedPhongMaterials()
{
}

SceneManager::~SceneManager()
{
    destroy();
}

//
// ISceneManager
//

const IAssetManagerSP& SceneManager::getAssetManager() const
{
    return assetManager;
}

const IContextObjectSP& SceneManager::getContextObject() const
{
    return assetManager->getContextObject();
}

const ICommandObjectSP& SceneManager::getCommandObject() const
{
    return assetManager->getCommandObject();
}

//

IObjectSP SceneManager::useObject(const std::string& name) const
{
	if (!contains(name, allObjects))
	{
		return IObjectSP();
	}

	//

    return get(name, allObjects);
}

VkBool32 SceneManager::addObject(const IObjectSP& object)
{
    if (!object.get())
    {
        return VK_FALSE;
    }

    return add(object->getName(), object, allObjects);
}

VkBool32 SceneManager::removeObject(const IObjectSP& object)
{
    if (!object.get())
    {
        return VK_FALSE;
    }

    return remove(object->getName(), allObjects);
}

const SmartPointerMap<std::string, IObjectSP>& SceneManager::getAllObjects() const
{
	return allObjects;
}

//

ICameraSP SceneManager::useCamera(const std::string& name) const
{
	if (!contains(name, allCameras))
	{
		return ICameraSP();
	}

	//

    return get(name, allCameras);
}

VkBool32 SceneManager::addCamera(const ICameraSP& camera)
{
    if (!camera.get())
    {
        return VK_FALSE;
    }

    return add(camera->getName(), camera, allCameras);
}

VkBool32 SceneManager::removeCamera(const ICameraSP& camera)
{
    if (!camera.get())
    {
        return VK_FALSE;
    }

    return remove(camera->getName(), allCameras);
}

const SmartPointerMap<std::string, ICameraSP>& SceneManager::getAllCameras() const
{
	return allCameras;
}

//

ILightSP SceneManager::useLight(const std::string& name) const
{
	if (!contains(name, allLights))
	{
		return ILightSP();
	}

	//

    return get(name, allLights);
}

VkBool32 SceneManager::addLight(const ILightSP& light)
{
    if (!light.get())
    {
        return VK_FALSE;
    }

    return add(light->getName(), light, allLights);
}

VkBool32 SceneManager::removeLight(const ILightSP& light)
{
    if (!light.get())
    {
        return VK_FALSE;
    }

    return remove(light->getName(), allLights);
}

const SmartPointerMap<std::string, ILightSP>& SceneManager::getAllLights() const
{
	return allLights;
}

//

IMeshSP SceneManager::useMesh(const std::string& name) const
{
	if (!contains(name, allMeshes))
	{
		return IMeshSP();
	}

	//

    return get(name, allMeshes);
}

VkBool32 SceneManager::addMesh(const IMeshSP& mesh)
{
    if (!mesh.get())
    {
        return VK_FALSE;
    }

    return add(mesh->getName(), mesh, allMeshes);
}

VkBool32 SceneManager::removeMesh(const IMeshSP& mesh)
{
    if (!mesh.get())
    {
        return VK_FALSE;
    }

    return remove(mesh->getName(), allMeshes);
}

ISubMeshSP SceneManager::useSubMesh(const std::string& name) const
{
	if (!contains(name, allSubMeshes))
	{
		return ISubMeshSP();
	}

	//

    return get(name, allSubMeshes);
}

VkBool32 SceneManager::addSubMesh(const ISubMeshSP& subMesh)
{
    if (!subMesh.get())
    {
        return VK_FALSE;
    }

    return add(subMesh->getName(), subMesh, allSubMeshes);
}

VkBool32 SceneManager::removeSubMesh(const ISubMeshSP& subMesh)
{
    if (!subMesh.get())
    {
        return VK_FALSE;
    }

    return remove(subMesh->getName(), allSubMeshes);
}

const SmartPointerMap<std::string, ISubMeshSP>& SceneManager::getAllSubMeshes() const
{
	return allSubMeshes;
}

//

IAnimationSP SceneManager::useAnimation(const std::string& name) const
{
	if (!contains(name, allAnimations))
	{
		return IAnimationSP();
	}

	//

    return get(name, allAnimations);
}

VkBool32 SceneManager::addAnimation(const IAnimationSP& animation)
{
    if (!animation.get())
    {
        return VK_FALSE;
    }

    return add(animation->getName(), animation, allAnimations);
}

VkBool32 SceneManager::removeAnimation(const IAnimationSP& animation)
{
    if (!animation.get())
    {
        return VK_FALSE;
    }

    return remove(animation->getName(), allAnimations);
}

IChannelSP SceneManager::useChannel(const std::string& name) const
{
	if (!contains(name, allChannels))
	{
		return IChannelSP();
	}

	//

    return get(name, allChannels);
}

VkBool32 SceneManager::addChannel(const IChannelSP& channel)
{
    if (!channel.get())
    {
        return VK_FALSE;
    }

    return add(channel->getName(), channel, allChannels);
}

VkBool32 SceneManager::removeChannel(const IChannelSP& channel)
{
    if (!channel.get())
    {
        return VK_FALSE;
    }

    return remove(channel->getName(), allChannels);
}

//

IBSDFMaterialSP SceneManager::useBSDFMaterial(const std::string& name) const
{
	if (!contains(name, allBSDFMaterials))
	{
		return IBSDFMaterialSP();
	}

	//

	IBSDFMaterialSP result = get(name, allBSDFMaterials);

	if (std::find(allUsedBSDFMaterials.begin(), allUsedBSDFMaterials.end(), name) != allUsedBSDFMaterials.end())
	{
		result = result->clone();
	}
	else
	{
		allUsedBSDFMaterials.push_back(name);
	}

    return result;
}

VkBool32 SceneManager::addBSDFMaterial(const IBSDFMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    return add(material->getName(), material, allBSDFMaterials);
}

VkBool32 SceneManager::removeBSDFMaterial(const IBSDFMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    auto removeElement = std::find(allUsedBSDFMaterials.begin(), allUsedBSDFMaterials.end(), material->getName());
    if (removeElement != allUsedBSDFMaterials.end())
    {
    	allUsedBSDFMaterials.erase(removeElement);
    }

    return remove(material->getName(), allBSDFMaterials);
}

IPhongMaterialSP SceneManager::usePhongMaterial(const std::string& name) const
{
	if (!contains(name, allPhongMaterials))
	{
		return IPhongMaterialSP();
	}

	//

	IPhongMaterialSP result = get(name, allPhongMaterials);

	if (std::find(allUsedPhongMaterials.begin(), allUsedPhongMaterials.end(), name) != allUsedPhongMaterials.end())
	{
		result = result->clone();
	}
	else
	{
		allUsedPhongMaterials.push_back(name);
	}

    return result;
}

VkBool32 SceneManager::addPhongMaterial(const IPhongMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    return add(material->getName(), material, allPhongMaterials);
}

VkBool32 SceneManager::removePhongMaterial(const IPhongMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    auto removeElement = std::find(allUsedPhongMaterials.begin(), allUsedPhongMaterials.end(), material->getName());
    if (removeElement != allUsedPhongMaterials.end())
    {
    	allUsedPhongMaterials.erase(removeElement);
    }

    return remove(material->getName(), allPhongMaterials);
}

//

ITextureObjectSP SceneManager::useTextureObject(const std::string& name) const
{
	return assetManager->useTextureObject(name);
}

VkBool32 SceneManager::addTextureObject(const ITextureObjectSP& textureObject)
{
	return assetManager->addTextureObject(textureObject);
}

VkBool32 SceneManager::removeTextureObject(const ITextureObjectSP& textureObject)
{
	return assetManager->removeTextureObject(textureObject);
}

//

IImageObjectSP SceneManager::useImageObject(const std::string& name) const
{
	return assetManager->useImageObject(name);
}

VkBool32 SceneManager::addImageObject(const IImageObjectSP& imageObject)
{
	return assetManager->addImageObject(imageObject);
}

VkBool32 SceneManager::removeImageObject(const IImageObjectSP& imageObject)
{
	return assetManager->removeImageObject(imageObject);
}

//

ISamplerSP SceneManager::useSampler(const std::string& name) const
{
	return assetManager->useSampler(name);
}

VkBool32 SceneManager::addSampler(const ISamplerSP& sampler)
{
	return assetManager->addSampler(sampler);
}

VkBool32 SceneManager::removeSampler(const ISamplerSP& sampler)
{
	return assetManager->removeSampler(sampler);
}

//

IImageDataSP SceneManager::useImageData(const std::string& name) const
{
	return assetManager->useImageData(name);
}

VkBool32 SceneManager::addImageData(const IImageDataSP& imageData)
{
	return assetManager->addImageData(imageData);
}

VkBool32 SceneManager::removeImageData(const IImageDataSP& imageData)
{
    return assetManager->removeImageData(imageData);
}

const SmartPointerMap<std::string, IImageDataSP>& SceneManager::getAllImageDatas() const
{
	return assetManager->getAllImageDatas();
}

//

IShaderModuleSP SceneManager::useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const
{
	return assetManager->useVertexShaderModule(vertexBufferType);
}

VkBool32 SceneManager::addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
	return assetManager->addVertexShaderModule(vertexBufferType, shaderModule);
}

VkBool32 SceneManager::removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
	return assetManager->removeVertexShaderModule(vertexBufferType, shaderModule);
}

IShaderModuleSP SceneManager::useFragmentShaderModule(const std::string& name) const
{
	return assetManager->useFragmentShaderModule(name);
}

VkBool32 SceneManager::addFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
	return assetManager->addFragmentShaderModule(shaderModule);
}

VkBool32 SceneManager::removeFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
	return assetManager->removeFragmentShaderModule(shaderModule);
}

//
// IDestroyable
//

void SceneManager::destroy()
{
    allObjects.clear();

    allCameras.clear();

    allLights.clear();

    allMeshes.clear();

    allSubMeshes.clear();

    allAnimations.clear();

    allChannels.clear();

    allBSDFMaterials.clear();
    allUsedBSDFMaterials.clear();

    allPhongMaterials.clear();
    allUsedPhongMaterials.clear();

    assetManager.reset();
}

} /* namespace vkts */
