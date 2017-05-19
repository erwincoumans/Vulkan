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

#ifndef VKTS_OVERWRITEUPDATE_HPP_
#define VKTS_OVERWRITEUPDATE_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class IScene;
class IObject;
class INode;

class OverwriteUpdate
{

private:

	const OverwriteUpdate* nextOverwrite;

public:

	OverwriteUpdate() :
		nextOverwrite(nullptr)
    {
    }

	OverwriteUpdate(const OverwriteUpdate* nextOverwrite) :
		nextOverwrite(nextOverwrite)
    {
    }

    virtual ~OverwriteUpdate()
    {
    }

    //

	const OverwriteUpdate* getNextOverwrite() const
	{
		return nextOverwrite;
	}

	void setNextOverwrite(const OverwriteUpdate* nextOverwrite)
	{
		this->nextOverwrite = nextOverwrite;
	}

    //

    virtual VkBool32 visit(const IScene& scene, const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer, const uint32_t objectOffset, const uint32_t objectStep, const uint32_t objectLimit) const
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(const IObject& object, const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer = 0) const
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(const INode& node, const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer, const glm::mat4& parentTransformMatrix, const VkBool32 parentTransformMatrixDirty, const INode* armatureNode) const
    {
    	return VK_TRUE;
    }
};

} /* namespace vkts */

#endif /* VKTS_OVERWRITEUPDATE_HPP_ */
