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

#ifndef VKTS_INODE_HPP_
#define VKTS_INODE_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class INode: public ICloneable<INode>, public IDestroyable, public std::enable_shared_from_this<INode>
{

public:

    INode() :
        ICloneable<INode>(), IDestroyable()
    {
    }

    virtual ~INode()
    {
    }

    virtual const std::string& getName() const = 0;

    virtual void setName(const std::string& name) = 0;


    virtual IRenderNodeSP getRenderNode(const uint32_t index) const = 0;

    virtual uint32_t getRenderNodeSize() const = 0;

    virtual void addRenderNode(const IRenderNodeSP& renderNode) = 0;


    virtual const std::shared_ptr<INode>& getParentNode() const = 0;

    virtual void setParentNode(const std::shared_ptr<INode>& parentNode) = 0;

    virtual const glm::vec3& getTranslate() const = 0;

    virtual void setTranslate(const glm::vec3& translate) = 0;

    virtual const Quat& getRotate() const = 0;

    virtual void setRotate(const Quat& rotate) = 0;

    virtual const glm::vec3& getScale() const = 0;

    virtual void setScale(const glm::vec3& scale) = 0;

    virtual const glm::vec3& getFinalTranslate() const = 0;

    virtual void setFinalTranslate(const glm::vec3& translate) = 0;

    virtual const Quat& getFinalRotate() const = 0;

    virtual void setFinalRotate(const Quat& rotate) = 0;

    virtual const glm::vec3& getFinalScale() const = 0;

    virtual void setFinalScale(const glm::vec3& scale) = 0;

    virtual int32_t getJointIndex() const = 0;

    virtual void setJointIndex(const int32_t jointIndex) = 0;

    virtual int32_t getNumberJoints() const = 0;

    virtual const glm::mat4& getInverseBindMatrix() const = 0;

    virtual void setInverseBindMatrix(const glm::mat4& inverseBindMatrix) = 0;

    virtual void addChildNode(const std::shared_ptr<INode>& childNode) = 0;

    virtual VkBool32 removeChildNode(const std::shared_ptr<INode>& childNode) = 0;

    virtual uint32_t getNumberChildNodes() const = 0;

    virtual const SmartPointerVector<std::shared_ptr<INode>>& getChildNodes() const = 0;

    virtual void sortChildNodes() = 0;

    virtual void addMesh(const IMeshSP& mesh) = 0;

    virtual VkBool32 removeMesh(const IMeshSP& mesh) = 0;

    virtual uint32_t getNumberMeshes() const = 0;

    virtual const SmartPointerVector<IMeshSP>& getMeshes() const = 0;

    virtual void addCamera(const ICameraSP& camera) = 0;

    virtual VkBool32 removeCamera(const ICameraSP& camera) = 0;

    virtual uint32_t getNumberCameras() const = 0;

    virtual const SmartPointerVector<ICameraSP>& getCameras() const = 0;

    virtual void addLight(const ILightSP& light) = 0;

    virtual VkBool32 removeLight(const ILightSP& light) = 0;

    virtual uint32_t getNumberLights() const = 0;

    virtual const SmartPointerVector<ILightSP>& getLights() const = 0;

    virtual void addAnimation(const IAnimationSP& animation) = 0;

    virtual VkBool32 removeAnimation(const IAnimationSP& animation) = 0;

    virtual uint32_t getNumberAnimations() const = 0;

    virtual const SmartPointerVector<IAnimationSP>& getAnimations() const = 0;

    virtual int32_t getCurrentAnimation() const = 0;

    virtual void setCurrentAnimation(const int32_t currentAnimation) = 0;


    virtual VkBool32 getDirty() const = 0;

    virtual void setDirty(const VkBool32 dirty = VK_TRUE) = 0;

    virtual IBufferObjectSP getTransformUniformBuffer() const = 0;

    virtual void setTransformUniformBuffer(const IBufferObjectSP& transformUniformBuffer) = 0;

    virtual IBufferObjectSP getJointsUniformBuffer() const = 0;

    virtual void setJointsUniformBuffer(const int32_t joints, const IBufferObjectSP& jointsUniformBuffer) = 0;

    virtual const Aabb& getAABB() const = 0;

    virtual Sphere getBoundingSphere() const = 0;

    virtual uint32_t getLayers() const = 0;

    virtual void setLayers(const uint32_t layers) = 0;

    virtual VkBool32 isOnLayer(const uint8_t layer) const = 0;

    virtual void setOnLayer(const uint8_t layer) = 0;

    virtual void removeFromLayer(const uint8_t layer) = 0;

    virtual const glm::mat4& getTransformMatrix() const = 0;

	virtual std::shared_ptr<INode> findNodeRecursive(const std::string& searchName) = 0;

	virtual std::shared_ptr<INode> findNodeRecursiveFromRoot(const std::string& searchName) = 0;

    virtual void updateParameterRecursive(Parameter* parameter) = 0;

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer) = 0;

    virtual void updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer, const glm::mat4& parentTransformMatrix, const VkBool32 parentTransformMatrixDirty, const std::shared_ptr<INode>& armatureNode, const OverwriteUpdate* updateOverwrite = nullptr) = 0;

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite = nullptr) = 0;


    virtual VkBool32 isNode() const = 0;

    virtual VkBool32 isArmature() const = 0;

    virtual VkBool32 isJoint() const = 0;

};

typedef std::shared_ptr<INode> INodeSP;

} /* namespace vkts */

#endif /* VKTS_INODE_HPP_ */
