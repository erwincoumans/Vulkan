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

std::string VKTS_APIENTRY shaderFactoryCreate(const std::string& directory, const VkShaderStageFlagBits shaderStage, const VkTsAttributes attributes)
{
	std::string extension = "";

	switch (shaderStage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
			extension = ".vert";
			break;
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			extension = ".tesc";
			break;
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			extension = ".tese";
			break;
		case VK_SHADER_STAGE_GEOMETRY_BIT:
			extension = ".geom";
			break;
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			extension = ".frag";
			break;
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

    if (!shaderFactoryResolveInclude(shader, directory))
    {
    	return "";
    }

    // TODO: Attributes in.
    // TODO: Attributes out.

    // TODO: Main.

    //

    return shader;
}

}
