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

#ifndef VKTS_SCENEMANAGER_HPP_
#define VKTS_SCENEMANAGER_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class SceneManager: public ISceneManager
{

protected:

    IAssetManagerSP assetManager;


    SmartPointerMap<std::string, IObjectSP> allObjects;

    SmartPointerMap<std::string, ICameraSP> allCameras;

    SmartPointerMap<std::string, ILightSP> allLights;

    SmartPointerMap<std::string, IMeshSP> allMeshes;

    SmartPointerMap<std::string, ISubMeshSP> allSubMeshes;

    SmartPointerMap<std::string, IAnimationSP> allAnimations;

    SmartPointerMap<std::string, IChannelSP> allChannels;

    SmartPointerMap<std::string, IBSDFMaterialSP> allBSDFMaterials;
    mutable std::vector<std::string> allUsedBSDFMaterials;

    SmartPointerMap<std::string, IPhongMaterialSP> allPhongMaterials;
    mutable std::vector<std::string> allUsedPhongMaterials;


    template<class S, class T>
    VkBool32 contains(const S& key, const SmartPointerMap<S, T>& map) const
    {
    	return map.contains(key);
    }

    template<class S, class T>
    const T& get(const S& key, const SmartPointerMap<S, T>& map) const
    {
    	return map[key];
    }

    template<class S, class T>
    VkBool32 add(const S& key, const T& value, SmartPointerMap<S, T>& map)
    {
        if (assetManager->isReplace() || map.find(key) == map.size())
        {
            map[key] = value;

            return VK_TRUE;
        }

        return VK_FALSE;
    }

    template<class S, class T>
    VkBool32 remove(const S& key, SmartPointerMap<S, T>& map)
    {
        auto index = map.find(key);

        if (index != map.size())
        {
            map.removeAt(index);

            return VK_TRUE;
        }

        return VK_FALSE;
    }

public:

    SceneManager() = delete;
    explicit SceneManager(const IAssetManagerSP& assetManager);
    SceneManager(const SceneManager& other) = delete;
    SceneManager(SceneManager&& other) = delete;
    virtual ~SceneManager();

    SceneManager& operator =(const SceneManager& other) = delete;

    SceneManager& operator =(SceneManager && other) = delete;

    //
    // ISceneManager
    //

    virtual const IAssetManagerSP& getAssetManager() const override;

    virtual const IContextObjectSP& getContextObject() const override;

    virtual const ICommandObjectSP& getCommandObject() const override;

    //

    virtual IObjectSP useObject(const std::string& name) const override;

    virtual VkBool32 addObject(const IObjectSP& object) override;

    virtual VkBool32 removeObject(const IObjectSP& object) override;

    virtual const SmartPointerMap<std::string, IObjectSP>& getAllObjects() const override;

    //

    virtual ICameraSP useCamera(const std::string& name) const override;

    virtual VkBool32 addCamera(const ICameraSP& camera) override;

    virtual VkBool32 removeCamera(const ICameraSP& camera) override;

    virtual const SmartPointerMap<std::string, ICameraSP>& getAllCameras() const override;

    //

    virtual ILightSP useLight(const std::string& name) const override;

    virtual VkBool32 addLight(const ILightSP& light) override;

    virtual VkBool32 removeLight(const ILightSP& light) override;

    virtual const SmartPointerMap<std::string, ILightSP>& getAllLights() const override;

    //

    virtual IMeshSP useMesh(const std::string& name) const override;

    virtual VkBool32 addMesh(const IMeshSP& mesh) override;

    virtual VkBool32 removeMesh(const IMeshSP& mesh) override;


    virtual ISubMeshSP useSubMesh(const std::string& name) const override;

    virtual VkBool32 addSubMesh(const ISubMeshSP& subMesh) override;

    virtual VkBool32 removeSubMesh(const ISubMeshSP& subMesh) override;

    virtual const SmartPointerMap<std::string, ISubMeshSP>& getAllSubMeshes() const override;

    //

    virtual IAnimationSP useAnimation(const std::string& name) const override;

    virtual VkBool32 addAnimation(const IAnimationSP& animation) override;

    virtual VkBool32 removeAnimation(const IAnimationSP& animation) override;


    virtual IChannelSP useChannel(const std::string& name) const override;

    virtual VkBool32 addChannel(const IChannelSP& channel) override;

    virtual VkBool32 removeChannel(const IChannelSP& channel) override;

    //

    virtual IBSDFMaterialSP useBSDFMaterial(const std::string& name) const override;

    virtual VkBool32 addBSDFMaterial(const IBSDFMaterialSP& material) override;

    virtual VkBool32 removeBSDFMaterial(const IBSDFMaterialSP& material) override;


    virtual IPhongMaterialSP usePhongMaterial(const std::string& name) const override;

    virtual VkBool32 addPhongMaterial(const IPhongMaterialSP& material) override;

    virtual VkBool32 removePhongMaterial(const IPhongMaterialSP& material) override;

    //

    virtual ITextureObjectSP useTextureObject(const std::string& name) const override;

    virtual VkBool32 addTextureObject(const ITextureObjectSP& textureObject) override;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) override;

    //

    virtual IImageObjectSP useImageObject(const std::string& name) const override;

    virtual VkBool32 addImageObject(const IImageObjectSP& imageObject) override;

    virtual VkBool32 removeImageObject(const IImageObjectSP& imageObject) override;

    //

    virtual ISamplerSP useSampler(const std::string& name) const override;

    virtual VkBool32 addSampler(const ISamplerSP& sampler) override;

    virtual VkBool32 removeSampler(const ISamplerSP& sampler) override;

    //

    virtual IImageDataSP useImageData(const std::string& name) const override;

    virtual VkBool32 addImageData(const IImageDataSP& imageData) override;

    virtual VkBool32 removeImageData(const IImageDataSP& imageData) override;

    virtual const SmartPointerMap<std::string, IImageDataSP>& getAllImageDatas() const override;

    //

    virtual IShaderModuleSP useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const override;

    virtual VkBool32 addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) override;

    virtual VkBool32 removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) override;


    virtual IShaderModuleSP useFragmentShaderModule(const std::string& name) const override;

    virtual VkBool32 addFragmentShaderModule(const IShaderModuleSP& shaderModule) override;

    virtual VkBool32 removeFragmentShaderModule(const IShaderModuleSP& shaderModule) override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_SCENEMANAGER_HPP_ */
