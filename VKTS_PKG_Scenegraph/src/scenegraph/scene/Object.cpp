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

#include "Object.hpp"

#include "Node.hpp"

namespace vkts
{


//
// IMoveable
//

void Object::update()
{
	setDirty();
}

Object::Object() :
    IObject(), name(""), scale(1.0f, 1.0f, 1.0f), transformMatrix(1.0f), dirty(VK_TRUE), rootNode()
{
}

Object::Object(const Object& other) :
    IObject(other), name(other.name + "_clone"), scale(other.scale), transformMatrix(other.transformMatrix), dirty(VK_TRUE), rootNode()
{
    if (!other.rootNode.get())
    {
    	// No error case.

        return;
    }

    rootNode = other.rootNode->clone();

    if (!rootNode.get())
    {
        name = "";

        setTranslate(glm::vec3(0.0f, 0.0f, 0.0f));
        setRotate(glm::vec3(0.0f, 0.0f, 0.0f));
        scale = glm::vec3(1.0f, 1.0f, 1.0f);

        return;
    }
}

Object::~Object()
{
    destroy();
}

//
// IObject
//

const std::string& Object::getName() const
{
    return name;
}

void Object::setName(const std::string& name)
{
    this->name = name;
}

const glm::vec3& Object::getScale() const
{
    return scale;
}

const INodeSP& Object::getRootNode() const
{
    return rootNode;
}

void Object::setRootNode(const INodeSP& rootNode)
{
    this->rootNode = rootNode;

    setDirty();
}

void Object::setScale(const glm::vec3& scale)
{
    this->scale = scale;

    setDirty();
}

void Object::setDirty()
{
    dirty = VK_TRUE;
}

void Object::updateParameterRecursive(Parameter* parameter)
{
	if (parameter)
	{
		parameter->visit(*this);
	}

	//

    if (rootNode.get())
    {
        rootNode->updateParameterRecursive(parameter);
    }
}

void Object::updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer)
{
    if (rootNode.get())
    {
        rootNode->updateDescriptorSetsRecursive(allWriteDescriptorSetsCount, allWriteDescriptorSets, currentBuffer);
    }
}

void Object::updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer, const OverwriteUpdate* updateOverwrite)
{
    const OverwriteUpdate* currentOverwrite = updateOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(*this, deltaTime, deltaTicks, tickTime, currentBuffer))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	//

	if (!IMoveable::update(deltaTime, deltaTicks, tickTime))
	{
		return;
	}

    if (dirty)
    {
        transformMatrix = translateMat4(translate.x, translate.y, translate.z) * (rotateZ * rotateY * rotateX) * scaleMat4(scale.x, scale.y, scale.z);
    }

    if (rootNode.get())
    {
        rootNode->updateTransformRecursive(deltaTime, deltaTicks, tickTime, currentBuffer, transformMatrix, dirty, INodeSP(), updateOverwrite);
    }

    dirty = VK_FALSE;
}


void Object::drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite)
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

    if (rootNode.get())
    {
        rootNode->drawRecursive(cmdBuffer, allGraphicsPipelines, currentBuffer, dynamicOffsetMappings, renderOverwrite);
    }
}

//
// ICloneable
//

IObjectSP Object::clone() const
{
	auto result = IObjectSP(new Object(*this));

	if (result.get() && getRootNode().get() && !result->getRootNode().get())
	{
		return IObjectSP();
	}

    return result;
}

//
// IDestroyable
//

void Object::destroy()
{
    if (rootNode.get())
    {
        rootNode->destroy();
    }
}

} /* namespace vkts */
