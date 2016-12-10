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

#ifndef VKTS_IRENDERFONT_HPP_
#define VKTS_IRENDERFONT_HPP_

#include <vkts/gui/vkts_gui.hpp>

namespace vkts
{

class IFont;

class IRenderFont: public IDestroyable
{

public:

    IRenderFont() :
    	IDestroyable()
    {
    }

    virtual ~IRenderFont()
    {
    }

    virtual std::shared_ptr<IRenderFont> create(const VkBool32 createData = VK_TRUE) const = 0;

    virtual void draw(const ICommandBuffersSP& cmdBuffer, const glm::mat4& viewProjection, const glm::vec2& translate, const std::string& text, const float fontSize, const glm::vec4& color, const IFont& font, const uint32_t dynamicOffsetCount = 0, const uint32_t* dynamicOffsets = nullptr) = 0;

};

typedef std::shared_ptr<IRenderFont> IRenderFontSP;

} /* namespace vkts */

#endif /* VKTS_IRENDERFONT_HPP_ */