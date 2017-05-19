/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT);
 *
 * Copyright (c); since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software");, to deal
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

#ifndef VKTS_FN_PARSE_HPP_
#define VKTS_FN_PARSE_HPP_

#include <vkts/core/vkts_core.hpp>

namespace vkts
{

VKTS_APICALL VkBool32 VKTS_APIENTRY parseSkipBuffer(const char* buffer);

VKTS_APICALL void VKTS_APIENTRY parseUnknownBuffer(const char* buffer);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseIsToken(const char* buffer, const char* token);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseString(const char* buffer, char* string, const uint32_t stringSize);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseStringTuple(const char* buffer, char* string0, const uint32_t string0Size, char* string1, const uint32_t string1Size);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseStringFloat(const char* buffer, char* string, const uint32_t stringSize, float* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseStringBool(const char* buffer, char* string, const uint32_t stringSize, VkBool32* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseBool(const char* buffer, VkBool32* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseBoolTriple(const char* buffer, VkBool32* scalar0, VkBool32* scalar1, VkBool32* scalar2);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseFloat(const char* buffer, float* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseVec2(const char* buffer, float vec2[2]);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseVec3(const char* buffer, float vec3[3]);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseVec4(const char* buffer, float vec4[4]);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseVec6(const char* buffer, float vec6[6]);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseVec8(const char* buffer, float vec8[8]);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseVec16(const char* buffer, float vec16[16]);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseInt(const char* buffer, int32_t* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseIVec3(const char* buffer, int32_t ivec3[3]);

VKTS_APICALL VkBool32 VKTS_APIENTRY parseUIntHex(const char* buffer, uint32_t* scalar);

}

#endif /* VKTS_FN_PARSE_HPP_ */
