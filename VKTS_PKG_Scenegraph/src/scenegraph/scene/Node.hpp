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

#ifndef VKTS_NODE_HPP_
#define VKTS_NODE_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class Node: public INode
{

private:

    std::string name;

    INodeSP parentNode;

    glm::vec3 translate;
    Quat rotate;
    glm::vec3 scale;

    glm::vec3 finalTranslate;
    Quat finalRotate;
    glm::vec3 finalScale;

    glm::mat4 transformMatrix;
    std::vector<VkBool32> transformMatrixDirty;

    int32_t jointIndex;
    int32_t joints;

    glm::mat4 inverseBindMatrix;

    SmartPointerVector<INodeSP> allChildNodes;

    SmartPointerVector<IMeshSP> allMeshes;

    SmartPointerVector<ICameraSP> allCameras;
    SmartPointerVector<ILightSP> allLights;

    SmartPointerVector<IAnimationSP> allAnimations;

    int32_t currentAnimation;

    IBufferObjectSP transformUniformBuffer;

    IBufferObjectSP jointsUniformBuffer;

    Aabb box;

    uint32_t layers;

    SmartPointerVector<IRenderNodeSP> nodeData;

    void reset();

public:

    Node();
    Node(const Node& other);
    Node(Node&& other) = delete;
    virtual ~Node();

    Node& operator =(const Node& other) = delete;
    Node& operator =(Node && other) = delete;

    //
    // INode
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;


    virtual IRenderNodeSP getRenderNode(const uint32_t index) const override;

    virtual uint32_t getRenderNodeSize() const override;

    virtual void addRenderNode(const IRenderNodeSP& nodeData) override;


    virtual const INodeSP& getParentNode() const override;

    virtual void setParentNode(const INodeSP& parentNode) override;

    virtual const glm::vec3& getTranslate() const override;

    virtual void setTranslate(const glm::vec3& translate) override;

    virtual const Quat& getRotate() const override;

    virtual void setRotate(const Quat& rotate) override;

    virtual const glm::vec3& getScale() const override;

    virtual void setScale(const glm::vec3& scale) override;

    virtual const glm::vec3& getFinalTranslate() const override;

    virtual void setFinalTranslate(const glm::vec3& translate) override;

    virtual const Quat& getFinalRotate() const override;

    virtual void setFinalRotate(const Quat& rotate) override;

    virtual const glm::vec3& getFinalScale() const override;

    virtual void setFinalScale(const glm::vec3& scale) override;

    virtual int32_t getJointIndex() const override;

    virtual void setJointIndex(const int32_t jointIndex) override;

    virtual int32_t getNumberJoints() const override;

    virtual const glm::mat4& getInverseBindMatrix() const override;

    virtual void setInverseBindMatrix(const glm::mat4& inverseBindMatrix) override;

    virtual void addChildNode(const INodeSP& childNode) override;

    virtual VkBool32 removeChildNode(const INodeSP& childNode) override;

    virtual uint32_t getNumberChildNodes() const override;

    virtual const SmartPointerVector<INodeSP>& getChildNodes() const override;

    virtual void sortChildNodes() override;

    virtual void addMesh(const IMeshSP& mesh) override;

    virtual VkBool32 removeMesh(const IMeshSP& mesh) override;

    virtual uint32_t getNumberMeshes() const override;

    virtual const SmartPointerVector<IMeshSP>& getMeshes() const override;

    virtual void addCamera(const ICameraSP& camera) override;

    virtual VkBool32 removeCamera(const ICameraSP& camera) override;

    virtual uint32_t getNumberCameras() const override;

    virtual const SmartPointerVector<ICameraSP>& getCameras() const override;

    virtual void addLight(const ILightSP& light) override;

    virtual VkBool32 removeLight(const ILightSP& light) override;

    virtual uint32_t getNumberLights() const override;

    virtual const SmartPointerVector<ILightSP>& getLights() const override;

    virtual void addAnimation(const IAnimationSP& animation) override;

    virtual VkBool32 removeAnimation(const IAnimationSP& animation) override;

    virtual uint32_t getNumberAnimations() const override;

    virtual const SmartPointerVector<IAnimationSP>& getAnimations() const override;

    virtual int32_t getCurrentAnimation() const override;

    virtual void setCurrentAnimation(const int32_t currentAnimation) override;


    virtual VkBool32 getDirty() const override;

    virtual void setDirty(const VkBool32 dirty = VK_TRUE) override;

    virtual IBufferObjectSP getTransformUniformBuffer() const override;

    virtual void setTransformUniformBuffer(const IBufferObjectSP& transformUniformBuffer) override;

    virtual IBufferObjectSP getJointsUniformBuffer() const override;

    virtual void setJointsUniformBuffer(const int32_t joints, const IBufferObjectSP& jointsUniformBuffer) override;

    virtual const Aabb& getAABB() const override;

    virtual Sphere getBoundingSphere() const override;

    virtual uint32_t getLayers() const override;

    virtual void setLayers(const uint32_t layers) override;

    virtual VkBool32 isOnLayer(const uint8_t layer) const override;

    virtual void setOnLayer(const uint8_t layer) override;

    virtual void removeFromLayer(const uint8_t layer) override;

    virtual const glm::mat4& getTransformMatrix() const override;

    virtual INodeSP findNodeRecursive(const std::string& searchName) override;

    virtual INodeSP findNodeRecursiveFromRoot(const std::string& searchName) override;

    virtual void updateParameterRecursive(Parameter* parameter) override;

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer) override;

    virtual void updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer, const glm::mat4& parentTransformMatrix, const VkBool32 parentTransformMatrixDirty, const INodeSP& armatureNode, const OverwriteUpdate* updateOverwrite = nullptr) override;

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite = nullptr) override;


    virtual VkBool32 isNode() const override;

    virtual VkBool32 isArmature() const override;

    virtual VkBool32 isJoint() const override;

    //
    // ICloneable
    //

    virtual INodeSP clone() const override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_NODE_HPP_ */
