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

#include <vkts/core/vkts_core.hpp>

namespace vkts
{

VkBool32 VKTS_APIENTRY parseSkipBuffer(const char* buffer)
{
    if (!buffer)
    {
        return VK_TRUE;
    }

    if (strlen(buffer) == 0)
    {
    	// No content, just skip.

    	return VK_TRUE;
    }

    if (strncmp(buffer, "#", 1) == 0)
    {
        // Comment, just skip.

        return VK_TRUE;
    }
    else if (strncmp(buffer, " ", 1) == 0 || strncmp(buffer, "\t", 1) == 0 || strncmp(buffer, "\r", 1) == 0 || strncmp(buffer, "\n", 1) == 0)
    {
        // Empty line, just skip.

        return VK_TRUE;
    }

    return VK_FALSE;
}

void VKTS_APIENTRY parseUnknownBuffer(const char* buffer)
{
    if (!buffer)
    {
        return;
    }

    std::string unknown(buffer);

    if (unknown.length() >= 2 && unknown[unknown.length() - 2] == '\r')
    {
        logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not parse line '%s'", unknown.substr(0, unknown.length() - 2).c_str());
    }
    else if (unknown.length() >= 1 && unknown[unknown.length() - 1] == '\n')
    {
        logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not parse line '%s'", unknown.substr(0, unknown.length() - 1).c_str());
    }
    else
    {
        logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not parse line '%s'", unknown.c_str());
    }
}

VkBool32 VKTS_APIENTRY parseIsToken(const char* buffer, const char* token)
{
    if (!buffer || !token)
    {
        return VK_FALSE;
    }

    if (strlen(buffer) <= strlen(token))
    {
    	return VK_FALSE;
    }

    if (!(buffer[strlen(token)] == ' ' || buffer[strlen(token)] == '\t' || buffer[strlen(token)] == '\r' || buffer[strlen(token)] == '\n' || buffer[strlen(token)] == '\0'))
    {
        return VK_FALSE;
    }

    if (strncmp(buffer, token, strlen(token)) == 0)
    {
        return VK_TRUE;
    }

    return VK_FALSE;
}

VkBool32 VKTS_APIENTRY parseString(const char* buffer, char* string, const uint32_t stringSize)
{
    if (!buffer || !string)
    {
        return VK_FALSE;
    }

    if (stringSize > VKTS_MAX_TOKEN_CHARS)
    {
    	return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %256s", token, string) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseStringTuple(const char* buffer, char* string0, const uint32_t string0Size, char* string1, const uint32_t string1Size)
{
    if (!buffer || !string0 || !string1)
    {
        return VK_FALSE;
    }

    if (string0Size > VKTS_MAX_TOKEN_CHARS || string1Size > VKTS_MAX_TOKEN_CHARS)
    {
    	return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %256s %256s", token, string0, string1) != 3)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseStringFloat(const char* buffer, char* string, const uint32_t stringSize, float* scalar)
{
    if (!buffer || !string || !scalar)
    {
        return VK_FALSE;
    }

    if (stringSize > VKTS_MAX_TOKEN_CHARS)
    {
    	return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %256s %f", token, string, scalar) != 3)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseStringBool(const char* buffer, char* string, const uint32_t stringSize, VkBool32* scalar)
{
    if (!buffer || !string || !scalar)
    {
        return VK_FALSE;
    }

    if (stringSize > VKTS_MAX_TOKEN_CHARS)
    {
    	return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    char value[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %256s %256s", token, string, value) != 3)
    {
        return VK_FALSE;
    }

    if (strcmp(value, "true") == 0)
    {
    	*scalar = VK_TRUE;
    }
    else if (strcmp(value, "false") == 0)
    {
    	*scalar = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseBool(const char* buffer, VkBool32* scalar)
{
    if (!buffer || !scalar)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    char value[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %256s", token, value) != 2)
    {
        return VK_FALSE;
    }

    if (strcmp(value, "true") == 0)
    {
    	*scalar = VK_TRUE;
    }
    else if (strcmp(value, "false") == 0)
    {
    	*scalar = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseBoolTriple(const char* buffer, VkBool32* scalar0, VkBool32* scalar1, VkBool32* scalar2)
{
    if (!buffer || !scalar0 || !scalar1 || !scalar2)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    char value0[VKTS_MAX_TOKEN_CHARS + 1];
    char value1[VKTS_MAX_TOKEN_CHARS + 1];
    char value2[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %256s %256s %256s", token, value0, value1, value2) != 4)
    {
        return VK_FALSE;
    }

    if (strcmp(value0, "true") == 0)
    {
    	*scalar0 = VK_TRUE;
    }
    else if (strcmp(value0, "false") == 0)
    {
    	*scalar0 = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    if (strcmp(value1, "true") == 0)
    {
    	*scalar1 = VK_TRUE;
    }
    else if (strcmp(value1, "false") == 0)
    {
    	*scalar1 = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    if (strcmp(value2, "true") == 0)
    {
    	*scalar2 = VK_TRUE;
    }
    else if (strcmp(value2, "false") == 0)
    {
    	*scalar2 = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseFloat(const char* buffer, float* scalar)
{
    if (!buffer || !scalar)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %f", token, scalar) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseVec2(const char* buffer, float vec2[2])
{
    if (!buffer || !vec2)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %f %f", token, &vec2[0], &vec2[1]) != 3)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseVec3(const char* buffer, float vec3[3])
{
    if (!buffer || !vec3)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %f %f %f", token, &vec3[0], &vec3[1], &vec3[2]) != 4)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseVec4(const char* buffer, float vec4[4])
{
    if (!buffer || !vec4)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %f %f %f %f", token, &vec4[0], &vec4[1], &vec4[2], &vec4[3]) != 5)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseVec6(const char* buffer, float vec6[6])
{
    if (!buffer || !vec6)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %f %f %f %f %f %f", token, &vec6[0], &vec6[1], &vec6[2], &vec6[3], &vec6[4], &vec6[5]) != 7)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseVec8(const char* buffer, float vec8[8])
{
    if (!buffer || !vec8)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %f %f %f %f %f %f %f %f", token, &vec8[0], &vec8[1], &vec8[2], &vec8[3], &vec8[4], &vec8[5], &vec8[6], &vec8[7]) != 9)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseVec16(const char* buffer, float vec16[16])
{
    if (!buffer || !vec16)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", token, &vec16[0], &vec16[1], &vec16[2], &vec16[3], &vec16[4], &vec16[5], &vec16[6], &vec16[7], &vec16[8], &vec16[9], &vec16[10], &vec16[11], &vec16[12], &vec16[13], &vec16[14], &vec16[15]) != 17)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseInt(const char* buffer, int32_t* scalar)
{
    if (!buffer || !scalar)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %d", token, scalar) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseIVec3(const char* buffer, int32_t ivec3[3])
{
    if (!buffer || !ivec3)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %d %d %d", token, &ivec3[0], &ivec3[1], &ivec3[2]) != 4)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parseUIntHex(const char* buffer, uint32_t* scalar)
{
    if (!buffer || !scalar)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%256s %x", token, scalar) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

}
