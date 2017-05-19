/**
 * VKTS Examples - Examples for Vulkan using VulKan ToolS.
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

#include <vkts/vkts.hpp>

static void terminateApp()
{
	vkts::engineTerminate();
}

static void printUsage()
{
	printf("Usage: VKTS_ShaderGenerator -m material -r renderer -a attributes - s stage\n");
	printf("   -m Material one of: PBR_MR PBR_SG COMMON\n");
	printf("   -r Renderer one of: FORWARD DEFERRED RESOLVE\n");
	printf("   -a XXXXXXXXXXX where X is either 1 or 0, if the following attribute is provided:\n");
	printf("      POSITION\n");
	printf("      NORMAL\n");
	printf("      TANGENT\n");
	printf("      TEXCOORD_0\n");
	printf("      TEXCOORD_1\n");
	printf("      COLOR_0\n");
	printf("      COLOR_1\n");
	printf("      JOINTS_0\n");
	printf("      JOINTS_1\n");
	printf("      WEIGHTS_0\n");
	printf("      WEIGHTS_1\n");
	printf("   -s Shader stage one of: vert tesc tese geom frag\n");
}

int main(int argc, char* argv[])
{
	//
	// Engine initialization.
	//

	if (!vkts::engineInit(vkts::visualDispatchMessages))
	{
		terminateApp();
		return -1;
	}

	vkts::logSetLevel(VKTS_LOG_INFO);

	//
	// Gathering command line.

	VkTsMaterial material;
	VkTsRenderer renderer;
	VkTsAttributes attributes;
	VkShaderStageFlagBits shaderStage;

	//

	std::string current;

	//

	if (vkts::parameterGetString(current, std::string("-m"), argc, argv))
	{
		if (current == "PBR_MR")
		{
			material = VKTS_MATERIAL_METAL_ROUGHNESS;
		}
		else if (current == "PBR_SG")
		{
			material = VKTS_MATERIAL_SPECULAR_GLOSSINESS;
		}
		else if (current == "COMMON")
		{
			material = VKTS_MATERIAL_COMMON;
		}
		else
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid material: %s", current);
			terminateApp();
			return -1;

		}
	}
	else
	{
		printUsage();
		terminateApp();
		return -1;
	}

	//

	if (vkts::parameterGetString(current, std::string("-r"), argc, argv))
	{
		if (current == "FORWARD")
		{
			renderer = VKTS_RENDERER_FORWARD;
		}
		else if (current == "DEFERRED")
		{
			renderer = VKTS_RENDERER_DEFERRED;
		}
		else if (current == "RESOLVE")
		{
			renderer = VKTS_RENDERER_RESOLVE;
		}
		else
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid renderer: %s", current);
			terminateApp();
			return -1;

		}
	}
	else
	{
		printUsage();
		terminateApp();
		return -1;
	}

	//

	if (vkts::parameterGetString(current, std::string("-a"), argc, argv))
	{
		if (current.length() != 11)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid amount of attributes: 11 != %u", (uint32_t)current.length());
			terminateApp();
			return -1;
		}

		for (uint32_t i = 0; i < 11; i++)
		{
			VkBool32 enabled;

			if (current[i] == '1')
			{
				enabled = VK_TRUE;
			}
			else if (current[i] == '0')
			{
				enabled = VK_FALSE;
			}
			else
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid attribute at position %u: %c", current[i]);
				terminateApp();
				return -1;
			}

			if (enabled)
			{
				attributes |= (VkTsAttributes)(1 << i);
			}
		}
	}
	else
	{
		printUsage();
		terminateApp();
		return -1;
	}


	//

	if (vkts::parameterGetString(current, std::string("-s"), argc, argv))
	{
		if (current == "vert")
		{
			shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
		}
		else if (current == "tesc")
		{
			shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}
		else if (current == "tese")
		{
			shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}
		else if (current == "geom")
		{
			shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
		}
		else if (current == "frag")
		{
			shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		else
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid shader stage: %s", current);
			terminateApp();
			return -1;
		}
	}
	else
	{
		printUsage();
		terminateApp();
		return -1;
	}

	//
	// Shader generator.
	//

	auto glsl = vkts::shaderFactoryCreate("shader/GLSL/4_5/template/", material, renderer, attributes, shaderStage);

	if (glsl == "")
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Generating shader failed: Could not create GLSL.");
		terminateApp();
		return -1;
	}

	auto glslTextBuffer = vkts::textBufferCreate(glsl.c_str());

	if (!glslTextBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Generating shader failed: Could not create GLSL text buffer.");
		terminateApp();
		return -1;
	}

	auto glslFilename = vkts::shaderFactoryCreateFilename(material, renderer, attributes, shaderStage);

	if (glslFilename == "")
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Generating shader failed: Could not create GLSL filename.");
		terminateApp();
		return -1;
	}

	if (!vkts::fileSaveText(glslFilename.c_str(), glslTextBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Generating shader failed: Could not save GLSL file.");
		terminateApp();
		return -1;
	}

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Generating shader succeeded: %s", glslFilename.c_str());

	//
	// Termination.
	//

	terminateApp();

	return 0;
}
