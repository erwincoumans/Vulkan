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

#include "Node.hpp"

#include "Animation.hpp"
#include "Mesh.hpp"

namespace vkts
{

void Node::reset()
{
    name = "";

    parentNode = INodeSP();

    translate = glm::vec3(0.0f, 0.0f, 0.0f);
    rotate = Quat();
    scale = glm::vec3(1.0f, 1.0f, 1.0f);

    finalTranslate = translate;
    finalRotate = rotate;
    finalScale = scale;

    transformMatrix = glm::mat4(1.0f);

    jointIndex = -1;
    joints = 0;
    inverseBindMatrix = glm::mat4(1.0f);

    setDirty();

    allChildNodes.clear();

    allMeshes.clear();

    allCameras.clear();

    allLights.clear();

    allAnimations.clear();

    currentAnimation = -1;

    transformUniformBuffer = IBufferObjectSP();

    jointsUniformBuffer = IBufferObjectSP();

    box = Aabb(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    layers = 0x01;

    //

    for (uint32_t i = 0; i < nodeData.size(); i++)
    {
        if (nodeData[i].get())
        {
        	nodeData[i]->reset();
        }
    }
    nodeData.clear();
}

Node::Node() :
    INode(), name(""), parentNode(), translate(0.0f, 0.0f, 0.0f), rotate(), scale(1.0f, 1.0f, 1.0f), finalTranslate(0.0f, 0.0f, 0.0f), finalRotate(), finalScale(1.0f, 1.0f, 1.0f), transformMatrix(1.0f), transformMatrixDirty(), jointIndex(-1), joints(0), inverseBindMatrix(1.0f), allChildNodes(), allMeshes(), allCameras(), allLights(), allAnimations(), currentAnimation(-1), transformUniformBuffer(), jointsUniformBuffer(), box(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), layers(0x01), nodeData()

{
    reset();
}

Node::Node(const Node& other) :
    INode(), name(other.name + "_clone"), parentNode(other.parentNode), translate(other.translate), rotate(other.rotate), scale(other.scale), finalTranslate(other.finalTranslate), finalRotate(other.finalRotate), finalScale(other.finalScale), transformMatrix(other.transformMatrix), transformMatrixDirty(other.transformMatrixDirty), jointIndex(-1), joints(0), inverseBindMatrix(other.inverseBindMatrix), box(other.box), layers(other.layers), nodeData()
{
    for (uint32_t i = 0; i < other.nodeData.size(); i++)
    {
        if (other.nodeData[i].get())
        {
        	auto currentNodeData = other.nodeData[i]->create();

    		if (!currentNodeData.get())
    		{
    			reset();

    			return;
    		}

        	nodeData.append(currentNodeData);
        }
    }

    for (uint32_t i = 0; i < other.allChildNodes.size(); i++)
    {
        const auto& currentChildNode = other.allChildNodes[i];

        if (!currentChildNode.get())
        {
            reset();

            break;
        }

        INodeSP cloneChildNode = currentChildNode->clone();

        if (!cloneChildNode.get())
        {
            reset();

            break;
        }

        allChildNodes.append(cloneChildNode);
    }

    for (uint32_t i = 0; i < other.allMeshes.size(); i++)
    {
        const auto& currentMesh = other.allMeshes[i];

        if (!currentMesh.get())
        {
            reset();

            break;
        }

        IMeshSP cloneMesh = currentMesh->clone();

        if (!cloneMesh.get())
        {
            reset();

            break;
        }

        allMeshes.append(cloneMesh);
    }

    for (uint32_t i = 0; i < other.allCameras.size(); i++)
    {
        const auto& currentCamera = other.allCameras[i];

        if (!currentCamera.get())
        {
            reset();

            break;
        }

        ICameraSP cloneCamera = currentCamera->clone();

        if (!cloneCamera.get())
        {
            reset();

            break;
        }

        allCameras.append(cloneCamera);
    }

    for (uint32_t i = 0; i < other.allLights.size(); i++)
    {
        const auto& currentLight = other.allLights[i];

        if (!currentLight.get())
        {
            reset();

            break;
        }

        ILightSP cloneLight = currentLight->clone();

        if (!cloneLight.get())
        {
            reset();

            break;
        }

        allLights.append(cloneLight);
    }

    for (uint32_t i = 0; i < other.allAnimations.size(); i++)
    {
        const auto& currentAnimation = other.allAnimations[i];

        if (!currentAnimation.get())
        {
            reset();

            break;
        }

        IAnimationSP cloneAnimation = currentAnimation->clone();

        if (!cloneAnimation.get())
        {
            reset();

            break;
        }

        allAnimations.append(cloneAnimation);
    }

    currentAnimation = other.currentAnimation;

    //

    if (other.transformUniformBuffer.get())
    {
		IBufferObjectSP transformUniformBuffer = bufferObjectCreate(other.transformUniformBuffer->getContextObject(), other.transformUniformBuffer->getBuffer()->getBufferCreateInfo(), other.transformUniformBuffer->getDeviceMemory()->getMemoryPropertyFlags());

		if (!transformUniformBuffer.get())
		{
			reset();

			return;
		}

		setTransformUniformBuffer(transformUniformBuffer);
    }

    //

    setJointIndex(other.jointIndex);

    //

    if (other.jointsUniformBuffer.get())
    {
        IBufferObjectSP jointsUniformBuffer = bufferObjectCreate(other.jointsUniformBuffer->getContextObject(), other.jointsUniformBuffer->getBuffer()->getBufferCreateInfo(), other.jointsUniformBuffer->getDeviceMemory()->getMemoryPropertyFlags());

        if (!jointsUniformBuffer.get())
        {
            reset();

            return;
        }

        setJointsUniformBuffer(other.joints, jointsUniformBuffer);
    }
}

Node::~Node()
{
    destroy();
}

//
// INode
//

const std::string& Node::getName() const
{
    return name;
}

void Node::setName(const std::string& name)
{
    this->name = name;
}

IRenderNodeSP Node::getRenderNode(const uint32_t index) const
{
	if (index >= nodeData.size())
	{
		return IRenderNodeSP();
	}

	return nodeData[index];
}

uint32_t Node::getRenderNodeSize() const
{
	return nodeData.size();
}

void Node::addRenderNode(const IRenderNodeSP& nodeData)
{
	this->nodeData.append(nodeData);
}

const INodeSP& Node::getParentNode() const
{
    return parentNode;
}

void Node::setParentNode(const INodeSP& parentNode)
{
    this->parentNode = parentNode;
}

const glm::vec3& Node::getTranslate() const
{
    return translate;
}

void Node::setTranslate(const glm::vec3& translate)
{
    this->translate = translate;

    setDirty();
}

const Quat& Node::getRotate() const
{
    return rotate;
}

void Node::setRotate(const Quat& rotate)
{
    this->rotate = rotate;

    setDirty();
}

const glm::vec3& Node::getScale() const
{
    return scale;
}

void Node::setScale(const glm::vec3& scale)
{
    this->scale = scale;

    setDirty();
}

const glm::vec3& Node::getFinalTranslate() const
{
    return finalTranslate;
}

void Node::setFinalTranslate(const glm::vec3& translate)
{
    this->finalTranslate = translate;
}

const Quat& Node::getFinalRotate() const
{
    return finalRotate;
}

void Node::setFinalRotate(const Quat& rotate)
{
    this->finalRotate = rotate;
}

const glm::vec3& Node::getFinalScale() const
{
    return finalScale;
}

void Node::setFinalScale(const glm::vec3& scale)
{
    this->finalScale = scale;
}

int32_t Node::getJointIndex() const
{
    return jointIndex;
}

void Node::setJointIndex(const int32_t jointIndex)
{
    this->jointIndex = jointIndex;
}

int32_t Node::getNumberJoints() const
{
	return joints;
}

const glm::mat4& Node::getInverseBindMatrix() const
{
	return inverseBindMatrix;
}

void Node::setInverseBindMatrix(const glm::mat4& inverseBindMatrix)
{
	this->inverseBindMatrix = inverseBindMatrix;
}

void Node::addChildNode(const INodeSP& childNode)
{
    allChildNodes.append(childNode);

    //

    if (allChildNodes.size() == 1)
    {
    	this->box = childNode->getAABB();
    }
    else
    {
    	this->box += childNode->getAABB();
    }
}

VkBool32 Node::removeChildNode(const INodeSP& childNode)
{
    return allChildNodes.remove(childNode);
}

uint32_t Node::getNumberChildNodes() const
{
    return allChildNodes.size();
}

const SmartPointerVector<INodeSP>& Node::getChildNodes() const
{
    return allChildNodes;
}

void Node::sortChildNodes()
{
	SmartPointerVector<INodeSP> allJointNodes;
	SmartPointerVector<INodeSP> allRestNodes;

	for (uint32_t i = 0; i < allChildNodes.size(); i++)
	{
		if (allChildNodes[i]->isJoint())
		{
			allJointNodes.append(allChildNodes[i]);
		}
		else
		{
			allRestNodes.append(allChildNodes[i]);
		}
	}

	//

	if (allJointNodes.size() > 0)
	{
		allChildNodes.clear();

		for (uint32_t i = 0; i < allJointNodes.size(); i++)
		{
			allChildNodes.append(allJointNodes[i]);
		}

		for (uint32_t i = 0; i < allRestNodes.size(); i++)
		{
			allChildNodes.append(allRestNodes[i]);
		}
	}
}

void Node::addMesh(const IMeshSP& mesh)
{
    allMeshes.append(mesh);

    //

    if (allMeshes.size() == 1)
    {
    	this->box = mesh->getAABB();
    }
    else
    {
    	this->box += mesh->getAABB();
    }
}

VkBool32 Node::removeMesh(const IMeshSP& mesh)
{
    return allMeshes.remove(mesh);
}

uint32_t Node::getNumberMeshes() const
{
    return allMeshes.size();
}

const SmartPointerVector<IMeshSP>& Node::getMeshes() const
{
    return allMeshes;
}

void Node::addCamera(const ICameraSP& camera)
{
    allCameras.append(camera);
}

VkBool32 Node::removeCamera(const ICameraSP& camera)
{
    return allCameras.remove(camera);
}

uint32_t Node::getNumberCameras() const
{
    return allCameras.size();
}

const SmartPointerVector<ICameraSP>& Node::getCameras() const
{
    return allCameras;
}

void Node::addLight(const ILightSP& light)
{
    allLights.append(light);
}

VkBool32 Node::removeLight(const ILightSP& light)
{
    return allLights.remove(light);
}

uint32_t Node::getNumberLights() const
{
    return allLights.size();
}

const SmartPointerVector<ILightSP>& Node::getLights() const
{
    return allLights;
}

void Node::addAnimation(const IAnimationSP& animation)
{
    allAnimations.append(animation);

    if (allAnimations.size() == 1)
    {
        currentAnimation = 0;
    }
}

VkBool32 Node::removeAnimation(const IAnimationSP& animation)
{
    VkBool32 result = allAnimations.remove(animation);

    if (currentAnimation >= (int32_t) allAnimations.size())
    {
        currentAnimation = (int32_t) allAnimations.size() - 1;
    }

    return result;
}

uint32_t Node::getNumberAnimations() const
{
    return allAnimations.size();
}

const SmartPointerVector<IAnimationSP>& Node::getAnimations() const
{
    return allAnimations;
}

int32_t Node::getCurrentAnimation() const
{
	return currentAnimation;
}

void Node::setCurrentAnimation(const int32_t currentAnimation)
{
	if (currentAnimation >= -1 && currentAnimation < (int32_t)allAnimations.size())
	{
		this->currentAnimation = currentAnimation;
	}
	else
	{
		this->currentAnimation = -1;
	}
}

VkBool32 Node::getDirty() const
{
	VkBool32 total = VK_FALSE;

	for (uint32_t i = 0; i < transformMatrixDirty.size(); i++)
    {
		total = total || transformMatrixDirty[i];
    }

    return total;
}

void Node::setDirty(const VkBool32 dirty)
{
    for (uint32_t i = 0; i < transformMatrixDirty.size(); i++)
    {
    	transformMatrixDirty[i] = dirty;
    }
}

IBufferObjectSP Node::getTransformUniformBuffer() const
{
    return transformUniformBuffer;
}

void Node::setTransformUniformBuffer(const IBufferObjectSP& transformUniformBuffer)
{
    this->transformUniformBuffer = transformUniformBuffer;

    this->transformMatrixDirty.resize(0);

    for (uint32_t i = 0; i < nodeData.size(); i++)
    {
        if (nodeData[i].get())
        {
            nodeData[i]->updateTransformUniformBuffer(transformUniformBuffer);
        }
    }
}

IBufferObjectSP Node::getJointsUniformBuffer() const
{
	return jointsUniformBuffer;
}

void Node::setJointsUniformBuffer(const int32_t joints, const IBufferObjectSP& jointsUniformBuffer)
{
	this->joints = joints;
	this->jointsUniformBuffer = jointsUniformBuffer;

    this->transformMatrixDirty.resize(0);

    for (uint32_t i = 0; i < nodeData.size(); i++)
    {
        if (nodeData[i].get())
        {
            nodeData[i]->updateJointsUniformBuffer(jointsUniformBuffer);
        }
    }
}

const Aabb& Node::getAABB() const
{
	return box;
}

Sphere Node::getBoundingSphere() const
{
	Sphere boundingSphere = transformMatrix * box.getSphere();

	for (uint32_t i = 0; i < allChildNodes.size(); i++)
    {
		boundingSphere += allChildNodes[i]->getBoundingSphere();
    }

	return boundingSphere;
}

uint32_t Node::getLayers() const
{
	return layers;
}

void Node::setLayers(const uint32_t layers)
{
	this->layers = layers;
}

VkBool32 Node::isOnLayer(const uint8_t layer) const
{
	if (layer >= 20)
	{
		throw std::out_of_range("layer >= 20");
	}

	return layers & (1 << (uint32_t)layer);
}

void Node::setOnLayer(const uint8_t layer)
{
	if (layer >= 20)
	{
		throw std::out_of_range("layer >= 20");
	}

	layers |= 1 << (uint32_t)layer;
}

void Node::removeFromLayer(const uint8_t layer)
{
	if (layer >= 20)
	{
		throw std::out_of_range("layer >= 20");
	}

	layers &= ~(1 << (uint32_t)layer) & 0x000FFFFF;
}

const glm::mat4& Node::getTransformMatrix() const
{
	return transformMatrix;
}

INodeSP Node::findNodeRecursive(const std::string& searchName)
{
	if (name == searchName)
	{
		return INode::shared_from_this();
	}

    for (uint32_t i = 0; i < allChildNodes.size(); i++)
    {
    	auto result = allChildNodes[i]->findNodeRecursive(searchName);

    	if (result.get())
    	{
    		return result;
    	}
    }

    return INodeSP(nullptr);
}

INodeSP Node::findNodeRecursiveFromRoot(const std::string& searchName)
{
	auto currentNode = INode::shared_from_this();

	// Search, until parent node is hit.
	while (currentNode.get())
	{
		if (currentNode->getName() == searchName)
		{
			return currentNode;
		}

		if (currentNode->getParentNode().get())
		{
			currentNode = currentNode->getParentNode();
		}
		else
		{
			break;
		}
	}

	//

	// Now, search complete tree.
	return findNodeRecursive(searchName);
}

void Node::updateParameterRecursive(Parameter* parameter)
{
	if (parameter)
	{
		parameter->visit(*this);
	}

    //

	for (uint32_t i = 0; i < allMeshes.size(); i++)
	{
		allMeshes[i]->updateParameterRecursive(parameter);
	}

	for (uint32_t i = 0; i < allChildNodes.size(); i++)
	{
		allChildNodes[i]->updateParameterRecursive(parameter);
	}
}

void Node::updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer)
{
	if (currentBuffer >= nodeData.size())
	{
		return;
	}

	if (nodeData[currentBuffer].get())
	{
		nodeData[currentBuffer]->updateDescriptorSets(allWriteDescriptorSetsCount, allWriteDescriptorSets);
	}

    //

	for (uint32_t i = 0; i < allMeshes.size(); i++)
	{
		allMeshes[i]->updateDescriptorSetsRecursive(allWriteDescriptorSetsCount, allWriteDescriptorSets, currentBuffer, name);
	}

	for (uint32_t i = 0; i < allChildNodes.size(); i++)
	{
		allChildNodes[i]->updateDescriptorSetsRecursive(allWriteDescriptorSetsCount, allWriteDescriptorSets, currentBuffer);
	}
}

void Node::updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer, const glm::mat4& parentTransformMatrix, const VkBool32 parentTransformMatrixDirty, const INodeSP& armatureNode, const OverwriteUpdate* updateOverwrite)
{
    const OverwriteUpdate* currentOverwrite = updateOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(*this, deltaTime, deltaTicks, tickTime, currentBuffer, parentTransformMatrix, parentTransformMatrixDirty, armatureNode.get()))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	//
    //

	if (currentBuffer >= (uint32_t)transformMatrixDirty.size())
	{
		transformMatrixDirty.resize(currentBuffer + 1);

		setDirty();
	}

    transformMatrixDirty[currentBuffer] = transformMatrixDirty[currentBuffer] || parentTransformMatrixDirty;

    //
    //

    // Gathering armature.
    auto newArmatureNode = isArmature() ? INode::shared_from_this() : armatureNode;

    //
    //

    finalTranslate = translate;
    finalRotate = rotate;
    finalScale = scale;

    //
    //

    if (currentAnimation >= 0 && currentAnimation < (int32_t) allAnimations.size())
    {
    	float currentTime = allAnimations[currentAnimation]->update((float)deltaTime);

        const auto& currentChannels = allAnimations[currentAnimation]->getChannels();

        //

        Quat a = rotate;
        Quat b = rotate;
        float t = 0.0f;
        VkBool32 quaternionDirty = VK_FALSE;

        glm::vec3 eulerRotation;
        VkBool32 eulerDirty = VK_FALSE;

        //

        for (uint32_t i = 0; i < currentChannels.size(); i++)
        {
        	if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_ROTATE)
        	{
        		quaternionDirty = VK_TRUE;

        	    if (currentChannels[i]->getNumberEntries() == 0)
        	    {
        	        // Do nothing.
        	    }
        	    else if (currentChannels[i]->getNumberEntries() == 1 || currentTime <= currentChannels[i]->getKeys()[0])
        	    {
        	        a[currentChannels[i]->getTargetTransformElement()] = currentChannels[i]->getValues()[0];
        	        b[currentChannels[i]->getTargetTransformElement()] = currentChannels[i]->getValues()[0];
        	    }
        	    else
        	    {
            	    auto lastIndex = currentChannels[i]->getNumberEntries() - 1;

            	    if (currentTime >= currentChannels[i]->getKeys()[lastIndex])
            	    {
            	    	a[currentChannels[i]->getTargetTransformElement()] = currentChannels[i]->getValues()[lastIndex];
            	    	b[currentChannels[i]->getTargetTransformElement()] = currentChannels[i]->getValues()[lastIndex];
            	    }
            	    else
            	    {
            	        uint32_t currentIndex = 0;
            	        while (currentIndex < currentChannels[i]->getNumberEntries())
            	        {
            	            if (currentTime < currentChannels[i]->getKeys()[currentIndex])
            	            {
            	            	currentIndex--;

            	                break;
            	            }

            	            currentIndex++;
            	        }

            	        float delta = (currentChannels[i]->getKeys()[currentIndex + 1] - currentChannels[i]->getKeys()[currentIndex]);

            	        if (delta > 0.0f)
            	        {
            	        	t = glm::clamp((currentTime - currentChannels[i]->getKeys()[currentIndex]) / delta, 0.0f, 1.0f);
            	        }
            	        else
            	        {
            	        	t = 0.0f;
            	        }

            	    	a[currentChannels[i]->getTargetTransformElement()] = currentChannels[i]->getValues()[currentIndex];
            	    	b[currentChannels[i]->getTargetTransformElement()] = currentChannels[i]->getValues()[currentIndex + 1];
            	    }
        	    }
        	}
        	else
        	{
				float value = interpolate(currentTime, currentChannels[i]);

				if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_TRANSLATE)
				{
					finalTranslate[currentChannels[i]->getTargetTransformElement()] = value;
				}
				else if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_EULER_ROTATE)
				{
					eulerRotation[currentChannels[i]->getTargetTransformElement()] = value;

					eulerDirty = VK_TRUE;
				}
				else if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_SCALE)
				{
					finalScale[currentChannels[i]->getTargetTransformElement()] = value;
				}
        	}
        }

        //

        if (quaternionDirty)
        {
            finalRotate = slerp(normalize(a), normalize(b), t);
        }

        if (eulerDirty)
        {
        	finalRotate = rotateRzRyRx(eulerRotation.z, eulerRotation.y, eulerRotation.x);
        }

        //

        transformMatrixDirty[currentBuffer] = VK_TRUE;
    }

    //
	//

    if (transformMatrixDirty[currentBuffer])
    {
		this->transformMatrix = parentTransformMatrix * translateMat4(finalTranslate.x, finalTranslate.y, finalTranslate.z) * finalRotate.mat4() * scaleMat4(finalScale.x, finalScale.y, finalScale.z);

        if (isNode() || isArmature())
        {
    		if (isArmature())
    		{
        		// Skeleton/Armature.

    			auto currentJointsUniformBuffer = getJointsUniformBuffer();

    			if (currentJointsUniformBuffer.get())
    			{
    	        	uint32_t dynamicOffset = currentBuffer * (uint32_t)(currentJointsUniformBuffer->getBuffer()->getSize() / currentJointsUniformBuffer->getBufferCount());

    	        	glm::mat4 inverseTransfromMatrix = glm::inverse(this->transformMatrix);

    				if (!currentJointsUniformBuffer->upload(dynamicOffset + 0, 0, inverseTransfromMatrix))
    				{
    					return;
    				}

    	            auto inverseTransformNormalMatrix = glm::transpose(glm::mat3(this->transformMatrix));

    	            if (!currentJointsUniformBuffer->upload(dynamicOffset + sizeof(float) * 16, 0, inverseTransformNormalMatrix))
    	            {
    	            	return;
    	            }
    			}
    		}

    		// Process node and armature.

        	if (allCameras.size() > 0)
			{
				for (uint32_t i = 0; i < allCameras.size(); i++)
				{
					allCameras[i]->updateViewMatrix(this->transformMatrix);
				}
			}

			if (allLights.size() > 0)
			{
				for (uint32_t i = 0; i < allLights.size(); i++)
				{
					allLights[i]->updateDirection(this->transformMatrix);
				}
			}

			if (allMeshes.size() > 0)
			{
				uint32_t dynamicOffset = currentBuffer * (uint32_t)(transformUniformBuffer->getBuffer()->getSize() / transformUniformBuffer->getBufferCount());

				// A mesh has to be rendered, so update with transform matrix from the node tree.

				if (!transformUniformBuffer->upload(dynamicOffset + 0, 0, this->transformMatrix))
				{
					return;
				}

				auto transformNormalMatrix = glm::transpose(glm::inverse(glm::mat3(this->transformMatrix)));

				if (!transformUniformBuffer->upload(dynamicOffset + sizeof(float) * 16, 0, transformNormalMatrix))
				{
					return;
				}
			}
        }
        else if (isJoint())
        {
			// Process joint.

			if (jointIndex >= 0 && jointIndex < VKTS_MAX_JOINTS)
			{
				if (newArmatureNode.get())
				{
					auto currentJointsUniformBuffer = newArmatureNode->getJointsUniformBuffer();

					if (currentJointsUniformBuffer.get())
					{
			        	auto jointMatrix = this->transformMatrix * this->inverseBindMatrix;

			        	//

						uint32_t dynamicOffset = currentBuffer * (uint32_t)(currentJointsUniformBuffer->getBuffer()->getSize() / currentJointsUniformBuffer->getBufferCount());

						uint32_t offset = sizeof(float) * 16 + sizeof(float) * 12;

						// Upload the joint matrices to blend them on the GPU.

						if (!currentJointsUniformBuffer->upload(dynamicOffset + offset + jointIndex * sizeof(float) * 16, 0, jointMatrix))
						{
							return;
						}

						auto transformNormalMatrix = glm::transpose(glm::inverse(glm::mat3(jointMatrix)));

						if (!currentJointsUniformBuffer->upload(dynamicOffset + offset + VKTS_MAX_JOINTS * sizeof(float) * 16 + jointIndex * sizeof(float) * 12, 0, transformNormalMatrix))
						{
							return;
						}
					}
					else
					{
						logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No joint uniform buffer");

						return;
					}
				}
				else
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No root armature node");

					return;
				}
			}
			else if (jointIndex >= VKTS_MAX_JOINTS)
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Too many joints: %d >= %d",  jointIndex, VKTS_MAX_JOINTS);

				return;
			}
        }
    }

    //

    // Process children.

    for (uint32_t i = 0; i < allChildNodes.size(); i++)
    {
        allChildNodes[i]->updateTransformRecursive(deltaTime, deltaTicks, tickTime, currentBuffer, this->transformMatrix, this->transformMatrixDirty[currentBuffer], newArmatureNode, updateOverwrite);
    }

    //

    // Reset dirty for current buffer.

    transformMatrixDirty[currentBuffer] = VK_FALSE;
}

void Node::drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(*this, cmdBuffer, allGraphicsPipelines, currentBuffer, dynamicOffsetMappings))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

    //

	for (uint32_t i = 0; i < allMeshes.size(); i++)
	{
		allMeshes[i]->drawRecursive(cmdBuffer, allGraphicsPipelines, currentBuffer, dynamicOffsetMappings, renderOverwrite, name);
	}

	for (uint32_t i = 0; i < allChildNodes.size(); i++)
	{
		allChildNodes[i]->drawRecursive(cmdBuffer, allGraphicsPipelines, currentBuffer, dynamicOffsetMappings, renderOverwrite);
	}
}

VkBool32 Node::isNode() const
{
	return (joints == 0) && (jointIndex == -1);
}

VkBool32 Node::isArmature() const
{
	return (joints != 0) && (jointIndex == -1);
}

VkBool32 Node::isJoint() const
{
	return (joints == 0) && (jointIndex != -1);
}

//
// ICloneable
//

INodeSP Node::clone() const
{
	auto result = INodeSP(new Node(*this));

	if (result.get() && (getRenderNodeSize() != result->getRenderNodeSize()))
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberCameras() != getNumberCameras())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberLights() != getNumberLights())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberChildNodes() != getNumberChildNodes())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberMeshes() != getNumberMeshes())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberAnimations() != getNumberAnimations())
	{
		return INodeSP();
	}

	if (result.get() && getTransformUniformBuffer().get() && !result->getTransformUniformBuffer().get())
	{
		return INodeSP();
	}

	if (result.get() && getJointsUniformBuffer().get() && !result->getJointsUniformBuffer().get())
	{
		return INodeSP();
	}

    return result;
}

//
// IDestroyable
//

void Node::destroy()
{
	try
	{
	    for (uint32_t i = 0; i < allChildNodes.size(); i++)
	    {
	        allChildNodes[i]->destroy();
	    }
	    allChildNodes.clear();

	    for (uint32_t i = 0; i < allMeshes.size(); i++)
	    {
	        allMeshes[i]->destroy();
	    }
	    allMeshes.clear();

	    allLights.clear();

	    for (uint32_t i = 0; i < allAnimations.size(); i++)
	    {
	        allAnimations[i]->destroy();
	    }
	    allAnimations.clear();

	    for (uint32_t i = 0; i < nodeData.size(); i++)
	    {
	        if (nodeData[i].get())
	        {
	        	nodeData[i]->destroy();
	        }
	    }
	}
	catch(const std::exception& e)
	{
    	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Catched exception '%s'", e.what());
	}
}

} /* namespace vkts */
