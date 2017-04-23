/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated doisReplacumentation files (the "Software"), to deal
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

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

static uint32_t VKTS_APIENTRY shaderFactoryReplace(std::string& shader, const std::string& token, const std::string& replacement)
{
	if (shader.length() == 0)
	{
		return 0;
	}

	if (token.length() == 0)
	{
		return 0;
	}

	//

	uint32_t replaceCount = 0;

	auto includeIndex = shader.find(token);

	while (includeIndex != shader.npos)
	{
		shader.replace(includeIndex, token.length(), replacement);

		replaceCount++;

		includeIndex = shader.find(token);
	}

	return replaceCount;
}

static VkBool32 VKTS_APIENTRY shaderFactoryResolveInclude(std::string& shader, const std::string& directory)
{
    std::vector<std::string> allIncludes;

	auto includeIndex = shader.find("#include");

	while (includeIndex != shader.npos)
	{
		auto originalIndex = includeIndex;

		includeIndex += 8;

		while (shader[includeIndex] != '"')
		{
			if (shader[includeIndex] != ' ' && shader[includeIndex] != '\t')
			{
				return VK_FALSE;
			}

			includeIndex++;

			if (includeIndex == shader.npos)
			{
				return VK_FALSE;
			}
		}

		includeIndex++;

		//

		auto startIndex = includeIndex;

		auto endIndex = shader.find('"', startIndex);

		if (endIndex == shader.npos)
		{
			return VK_FALSE;
		}

		//
		//

		auto includeString = shader.substr(startIndex, endIndex - startIndex - 1);

		std::string insertString = "";

		if (std::find(allIncludes.begin(), allIncludes.end(), includeString) == allIncludes.end())
		{
			auto textFile = fileLoadText((directory + includeString).c_str());

			if (!textFile.get())
			{
				return VK_FALSE;
			}

			insertString = std::string(textFile->getString());

			allIncludes.push_back(includeString);
		}

		shader.replace(originalIndex, endIndex - originalIndex, insertString);

		//
		//

		includeIndex = shader.find("#include");
	}

	return VK_TRUE;
}

std::string VKTS_APIENTRY shaderFactoryCreate(const std::string& directory, const VkShaderStageFlagBits shaderStage, const VkTsAttributes attributes, const VkTsMaterial material, const VkTsRenderer renderer)
{
	std::string extension = "";

	switch (shaderStage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
		{
			extension = ".vert";
			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		{
			extension = ".tesc";
			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		{
			extension = ".tese";
			break;
		}
		case VK_SHADER_STAGE_GEOMETRY_BIT:
		{
			extension = ".geom";
			break;
		}
		case VK_SHADER_STAGE_FRAGMENT_BIT:
		{
			extension = ".frag";
			break;
		}
		default:
			return "";
	}

	//

	auto shaderTextFile = fileLoadText((directory + "main" + extension).c_str());

	if (!shaderTextFile.get())
	{
		return "";
	}

	std::string shader = std::string(shaderTextFile->getString());

    //

    // TODO: Header.

	// Includes.
    if (!shaderFactoryResolveInclude(shader, directory))
    {
    	return "";
    }

    //

    std::string attributesIn;
    std::string attributesOut;

    uint32_t locationIn = 0;
    uint32_t locationOut = 0;

    for (auto currentAttribute = (uint32_t)VKTS_ATTRIBUTE_POSITION; currentAttribute <= (uint32_t)VKTS_ATTRIBUTE_WEIGHTS_1; currentAttribute *= 2)
    {
    	if ((currentAttribute & attributes) == currentAttribute)
    	{
    	    if (currentAttribute == VKTS_ATTRIBUTE_POSITION)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_position;";
    	    		locationIn++;

    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_position;";
    	    		locationOut++;
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_incident;";
    	    		locationOut++;
    	    	}
    	    	else
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_position;";
    	    		locationIn++;
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_incident;";
    	    		locationIn++;

    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_position;";
    	    		locationOut++;
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_incident;";
    	    		locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_NORMAL)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_normal;";
	    		locationIn++;

	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_normal;";
	    		locationOut++;
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_TANGENT)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_tangent;";
    	    		locationIn++;

    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_tangent;";
    	    		locationOut++;
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_bitangent;";
    	    		locationOut++;
    	    	}
    	    	else
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_tangent;";
    	    		locationIn++;
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_bitangent;";
    	    		locationIn++;

    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_tangent;";
    	    		locationOut++;
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_bitangent;";
    	    		locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_TEXCOORD_0)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec2 in_texcoord_0;";
	    		locationIn++;

	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec2 out_texcoord_0;";
	    		locationOut++;
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_TEXCOORD_1)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec2 in_texcoord_1;";
	    		locationIn++;

	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec2 out_texcoord_1;";
	    		locationOut++;
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_COLOR_0)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_color_0;";
	    		locationIn++;

	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_color_0;";
	    		locationOut++;
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_COLOR_1)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_color_1;";
	    		locationIn++;

	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_color_1;";
	    		locationOut++;
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_JOINTS_0)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_joints_0;";
    	    		locationIn++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_JOINTS_1)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_joints_1;";
    	    		locationIn++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_WEIGHTS_0)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_weights_0;";
    	    		locationIn++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_WEIGHTS_1)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_weights_1;";
    	    		locationIn++;
    	    	}
    	    }
    	}
    }

    // Attributes in.
    if (shaderFactoryReplace(shader, "/*%VKTS_ATTRIBUTES_IN%*/", attributesIn) == 0)
    {
    	return "";
    }

    // Attributes out.
    if (shaderFactoryReplace(shader, "/*%VKTS_ATTRIBUTES_OUT%*/", attributesOut) == 0)
    {
    	return "";
    }

    // TODO: Main.

    //

    return shader;
}

}
