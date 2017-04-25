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

#ifndef VKTS_ISCENEMANAGER_HPP_
#define VKTS_ISCENEMANAGER_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class ISceneManager: public IDestroyable
{

public:

    ISceneManager() :
        IDestroyable()
    {
    }

    virtual ~ISceneManager()
    {
    }

    virtual const IAssetManagerSP& getAssetManager() const = 0;

    virtual const IContextObjectSP& getContextObject() const = 0;

    virtual const ICommandObjectSP& getCommandObject() const = 0;

    //

    virtual IObjectSP useObject(const std::string& name) const = 0;

    virtual VkBool32 addObject(const IObjectSP& object) = 0;

    virtual VkBool32 removeObject(const IObjectSP& object) = 0;

    virtual const SmartPointerMap<std::string, IObjectSP>& getAllObjects() const = 0;

    //

    virtual ICameraSP useCamera(const std::string& name) const = 0;

    virtual VkBool32 addCamera(const ICameraSP& camera) = 0;

    virtual VkBool32 removeCamera(const ICameraSP& camera) = 0;

    virtual const SmartPointerMap<std::string, ICameraSP>& getAllCameras() const = 0;

    //

    virtual ILightSP useLight(const std::string& name) const = 0;

    virtual VkBool32 addLight(const ILightSP& light) = 0;

    virtual VkBool32 removeLight(const ILightSP& light) = 0;

    virtual const SmartPointerMap<std::string, ILightSP>& getAllLights() const = 0;

    //

    virtual IMeshSP useMesh(const std::string& name) const = 0;

    virtual VkBool32 addMesh(const IMeshSP& mesh) = 0;

    virtual VkBool32 removeMesh(const IMeshSP& mesh) = 0;


    virtual ISubMeshSP useSubMesh(const std::string& name) const = 0;

    virtual VkBool32 addSubMesh(const ISubMeshSP& subMesh) = 0;

    virtual VkBool32 removeSubMesh(const ISubMeshSP& subMesh) = 0;

    virtual const SmartPointerMap<std::string, ISubMeshSP>& getAllSubMeshes() const = 0;

    //

    virtual IAnimationSP useAnimation(const std::string& name) const = 0;

    virtual VkBool32 addAnimation(const IAnimationSP& animation) = 0;

    virtual VkBool32 removeAnimation(const IAnimationSP& animation) = 0;


    virtual IChannelSP useChannel(const std::string& name) const = 0;

    virtual VkBool32 addChannel(const IChannelSP& channel) = 0;

    virtual VkBool32 removeChannel(const IChannelSP& channel) = 0;

    //

    virtual IBSDFMaterialSP useBSDFMaterial(const std::string& name) const = 0;

    virtual VkBool32 addBSDFMaterial(const IBSDFMaterialSP& material) = 0;

    virtual VkBool32 removeBSDFMaterial(const IBSDFMaterialSP& material) = 0;


    virtual IPhongMaterialSP usePhongMaterial(const std::string& name) const = 0;

    virtual VkBool32 addPhongMaterial(const IPhongMaterialSP& material) = 0;

    virtual VkBool32 removePhongMaterial(const IPhongMaterialSP& material) = 0;

    //

    virtual ITextureObjectSP useTextureObject(const std::string& name) const = 0;

    virtual VkBool32 addTextureObject(const ITextureObjectSP& textureObject) = 0;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) = 0;

    //

    virtual IImageObjectSP useImageObject(const std::string& name) const = 0;

    virtual VkBool32 addImageObject(const IImageObjectSP& imageObject) = 0;

    virtual VkBool32 removeImageObject(const IImageObjectSP& imageObject) = 0;

    //

    virtual ISamplerSP useSampler(const std::string& name) const = 0;

    virtual VkBool32 addSampler(const ISamplerSP& sampler) = 0;

    virtual VkBool32 removeSampler(const ISamplerSP& sampler) = 0;

    //

    virtual IImageDataSP useImageData(const std::string& name) const = 0;

    virtual VkBool32 addImageData(const IImageDataSP& imageData) = 0;

    virtual VkBool32 removeImageData(const IImageDataSP& imageData) = 0;

    virtual const SmartPointerMap<std::string, IImageDataSP>& getAllImageDatas() const = 0;

    //

    virtual IShaderModuleSP useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const = 0;

    virtual VkBool32 addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) = 0;

    virtual VkBool32 removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) = 0;


    virtual IShaderModuleSP useFragmentShaderModule(const std::string& name) const = 0;

    virtual VkBool32 addFragmentShaderModule(const IShaderModuleSP& shaderModule) = 0;

    virtual VkBool32 removeFragmentShaderModule(const IShaderModuleSP& shaderModule) = 0;

};

typedef std::shared_ptr<ISceneManager> ISceneManagerSP;

} /* namespace vkts */

#endif /* VKTS_ISCENEMANAGER_HPP_ */
