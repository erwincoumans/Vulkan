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

static VkBool32 VKTS_APIENTRY shaderFactoryResolveHeaderForward(std::string& shader, const std::string& directory, const VkTsMaterial material, const VkTsAttributes attributes, const VkShaderStageFlagBits shaderStage)
{
	std::string filename = "";

	switch (shaderStage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
		{
			filename = "header.glsl";

			if ((attributes & (VKTS_ATTRIBUTE_JOINTS_0 | VKTS_ATTRIBUTE_WEIGHTS_0)) == (VKTS_ATTRIBUTE_JOINTS_0 | VKTS_ATTRIBUTE_WEIGHTS_0))
			{
				filename = "header_joints.glsl";
			}

			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		{
			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		{
			break;
		}
		case VK_SHADER_STAGE_GEOMETRY_BIT:
		{
			break;
		}
		case VK_SHADER_STAGE_FRAGMENT_BIT:
		{
			if (material == VKTS_MATERIAL_METAL_ROUGHNESS)
			{
				filename = "pbr_mr_material_header.glsl";
			}
			else if (material == VKTS_MATERIAL_SPECULAR_GLOSSINESS)
			{
				filename = "pbr_sg_material_header.glsl";
			}
			else // VKTS_MATERIAL_COMMON
			{
				// TODO: Add header for common material.
			}
			break;
		}
		default:
			return VK_FALSE;
	}

	auto textFile = fileLoadText((directory + filename).c_str());

	if (!textFile.get())
	{
		return VK_FALSE;
	}

    if (shaderFactoryReplace(shader, "/*%VKTS_HEADER%*/", textFile->getString()) == 0)
    {
    	return VK_FALSE;
    }

	return VK_TRUE;
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

		auto includeString = shader.substr(startIndex, endIndex - startIndex);

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

		shader.replace(originalIndex, endIndex - originalIndex + 1, insertString);

		//
		//

		includeIndex = shader.find("#include");
	}

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY shaderFactoryValidateAttributes(const VkTsAttributes attributes)
{
	// Position always needed.
	if (!((attributes & VKTS_ATTRIBUTE_POSITION) == VKTS_ATTRIBUTE_POSITION))
	{
		return VK_FALSE;
	}

	// If tangent is present, normal has to be present as well.
	if ((attributes & VKTS_ATTRIBUTE_TANGENT) == VKTS_ATTRIBUTE_TANGENT)
	{
		if (!((attributes & VKTS_ATTRIBUTE_NORMAL) == VKTS_ATTRIBUTE_NORMAL))
		{
			return VK_FALSE;
		}
	}

	// If texcoord_1 is present, texcoord_0 has to be present as well.
	if ((attributes & VKTS_ATTRIBUTE_TEXCOORD_1) == VKTS_ATTRIBUTE_TEXCOORD_1)
	{
		if (!((attributes & VKTS_ATTRIBUTE_TEXCOORD_0) == VKTS_ATTRIBUTE_TEXCOORD_0))
		{
			return VK_FALSE;
		}
	}

	// If color_1 is present, color_0 has to be present as well.
	if ((attributes & VKTS_ATTRIBUTE_COLOR_1) == VKTS_ATTRIBUTE_COLOR_1)
	{
		if (!((attributes & VKTS_ATTRIBUTE_COLOR_0) == VKTS_ATTRIBUTE_COLOR_0))
		{
			return VK_FALSE;
		}
	}

	// If joints_0 is present, weights_0 has to be present as well.
	if ((attributes & VKTS_ATTRIBUTE_JOINTS_0) == VKTS_ATTRIBUTE_JOINTS_0)
	{
		if (!((attributes & VKTS_ATTRIBUTE_WEIGHTS_0) == VKTS_ATTRIBUTE_WEIGHTS_0))
		{
			return VK_FALSE;
		}
	}

	// If weights_0 is present, joints_0 has to be present as well.
	if ((attributes & VKTS_ATTRIBUTE_WEIGHTS_0) == VKTS_ATTRIBUTE_WEIGHTS_0)
	{
		if (!((attributes & VKTS_ATTRIBUTE_JOINTS_0) == VKTS_ATTRIBUTE_JOINTS_0))
		{
			return VK_FALSE;
		}
	}


	// If joints_1 is present, weights_1 has to be present as well. Also, joints_0 has to be present.
	if ((attributes & VKTS_ATTRIBUTE_JOINTS_1) == VKTS_ATTRIBUTE_JOINTS_1)
	{
		if (!((attributes & VKTS_ATTRIBUTE_WEIGHTS_1) == VKTS_ATTRIBUTE_WEIGHTS_1))
		{
			return VK_FALSE;
		}

		//

		if ((attributes & VKTS_ATTRIBUTE_JOINTS_0) == VKTS_ATTRIBUTE_JOINTS_0)
		{
			return VK_FALSE;
		}
	}

	// If weights_1 is present, joints_1 has to be present as well. Also, wights_0 has to be present.
	if ((attributes & VKTS_ATTRIBUTE_WEIGHTS_1) == VKTS_ATTRIBUTE_WEIGHTS_1)
	{
		if (!((attributes & VKTS_ATTRIBUTE_JOINTS_1) == VKTS_ATTRIBUTE_JOINTS_1))
		{
			return VK_FALSE;
		}

		//

		if ((attributes & VKTS_ATTRIBUTE_WEIGHTS_0) == VKTS_ATTRIBUTE_WEIGHTS_0)
		{
			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

std::string VKTS_APIENTRY shaderFactoryCreate(const std::string& directory, const VkTsMaterial material, const VkTsRenderer renderer, const VkTsAttributes attributes, const VkShaderStageFlagBits shaderStage)
{
	if (!shaderFactoryValidateAttributes(attributes))
	{
		return "";
	}

	//

	ITextBufferSP textFile;

	std::string extension = "";

	switch (shaderStage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
		{
			extension = "vert";
			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		{
			extension = "tesc";
			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		{
			extension = "tese";
			break;
		}
		case VK_SHADER_STAGE_GEOMETRY_BIT:
		{
			extension = "geom";
			break;
		}
		case VK_SHADER_STAGE_FRAGMENT_BIT:
		{
			extension = "frag";
			break;
		}
		default:
			return "";
	}

	//

	auto shaderTextFile = fileLoadText((directory + "main." + extension).c_str());

	if (!shaderTextFile.get())
	{
		return "";
	}

	std::string shader = std::string(shaderTextFile->getString());

    //
    // Header section.
    //

	if (renderer == VKTS_RENDERER_FORWARD)
	{
		if (!shaderFactoryResolveHeaderForward(shader, directory + extension + "/", material, attributes, shaderStage))
		{
			return "";
		}
	}
	else // VKTS_RENDERER_DEFERRED
	{
		// TODO: Resolve header for deferred rendering.
	}

	// Includes.
    if (!shaderFactoryResolveInclude(shader, directory + extension + "/"))
    {
    	return "";
    }


    //
    // Attributes section.
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
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_position;\n";
    	    		locationIn++;
    	    	}
    	    	else
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_position;\n";
    	    		locationIn++;
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_incident;\n";
    	    		locationIn++;
    	    	}

    	    	if (shaderStage == VK_SHADER_STAGE_FRAGMENT_BIT)
    	    	{
    	    		if (renderer == VKTS_RENDERER_FORWARD)
    	    		{
        	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_color;\n";
        	    		locationOut++;
    	    		}
    	    		else // VKTS_RENDERER_DEFERRED
    	    		{
        	    		// TODO: Add output for writing channels.
    	    		}
    	    	}
    	    	else
    	    	{
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_position;\n";
    	    		locationOut++;
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_incident;\n";
    	    		locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_NORMAL)
    	    {
				attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_normal;\n";
				locationIn++;

				if (shaderStage != VK_SHADER_STAGE_FRAGMENT_BIT)
    	    	{
		    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_normal;\n";
					locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_TANGENT)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_tangent;\n";
    	    		locationIn++;
    	    	}
    	    	else
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_tangent;\n";
    	    		locationIn++;
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec3 in_bitangent;\n";
    	    		locationIn++;
    	    	}

    	    	if (shaderStage != VK_SHADER_STAGE_FRAGMENT_BIT)
    	    	{
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_tangent;\n";
    	    		locationOut++;
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec3 out_bitangent;\n";
    	    		locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_TEXCOORD_0)
    	    {
				attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec2 in_texcoord_0;\n";
				locationIn++;

				if (shaderStage != VK_SHADER_STAGE_FRAGMENT_BIT)
    	    	{
					attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec2 out_texcoord_0;\n";
					locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_TEXCOORD_1)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec2 in_texcoord_1;\n";
	    		locationIn++;

	    		if (shaderStage != VK_SHADER_STAGE_FRAGMENT_BIT)
    	    	{
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec2 out_texcoord_1;\n";
    	    		locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_COLOR_0)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_color_0;\n";
	    		locationIn++;

	    		if (shaderStage != VK_SHADER_STAGE_FRAGMENT_BIT)
    	    	{
    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_color_0;\n";
    	    		locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_COLOR_1)
    	    {
	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_color_1;\n";
	    		locationIn++;

	    		if (shaderStage != VK_SHADER_STAGE_FRAGMENT_BIT)
    	    	{

    	    		attributesOut += "layout (location = " + std::to_string(locationOut) + ") out vec4 out_color_1;\n";
    	    		locationOut++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_JOINTS_0)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_joints_0;\n";
    	    		locationIn++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_JOINTS_1)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_joints_1;\n";
    	    		locationIn++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_WEIGHTS_0)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_weights_0;\n";
    	    		locationIn++;
    	    	}
    	    }
    	    else if (currentAttribute == VKTS_ATTRIBUTE_WEIGHTS_1)
    	    {
    	    	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
    	    	{
    	    		attributesIn += "layout (location = " + std::to_string(locationIn) + ") in vec4 in_weights_1;\n";
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

    //
    // Main section.
    //

    std::string mainContent;

    //
    // Attribute gathering.
    //
	if (shaderStage == VK_SHADER_STAGE_VERTEX_BIT)
	{
		std::string postMainContent = "";

		std::string jointDefineContent = "";
		std::string jointLoopContent = "";
		std::string jointAssignContent = "";

		if ((VKTS_ATTRIBUTE_POSITION & attributes) == VKTS_ATTRIBUTE_POSITION)
    	{
			mainContent += "    vec4 position = vec4(in_position, 1.0);\n";

			jointDefineContent += "    vec4 bone_position = vec4(0.0, 0.0, 0.0, 0.0);\n";
			jointLoopContent += "                bone_position += (u_bufferBoneTransform.inverseModelMatrix * u_bufferBoneTransform.jointMatrix[boneIndex] * position) * boneWeight;\n";
			jointAssignContent += "    position =  bone_position / bone_position.w;\n";

			postMainContent += "    position = u_bufferViewProjection.viewMatrix * u_bufferTransform.modelMatrix * position;\n";
			postMainContent += "    out_incident = -normalize(position.xyz);\n";
			postMainContent += "    out_position = u_bufferViewProjection.projectionMatrix * position;\n";
    	}
		else
		{
			return "";
		}

		//

		if ((VKTS_ATTRIBUTE_NORMAL & attributes) == VKTS_ATTRIBUTE_NORMAL)
    	{
			mainContent += "    vec3 normal = in_normal;\n";

			jointDefineContent += "    vec3 bone_normal = vec3(0.0, 0.0, 0.0);\n";
			jointLoopContent += "                bone_normal += (u_bufferBoneTransform.inverseModelNormalMatrix * u_bufferBoneTransform.jointNormalMatrix[boneIndex] * normal) * boneWeight;\n";
			jointAssignContent += "    normal = bone_normal;\n";

			postMainContent += "    out_normal = mat3(u_bufferViewProjection.viewMatrix) * u_bufferTransform.normalMatrix * normal;\n";
    	}

		if ((VKTS_ATTRIBUTE_TANGENT & attributes) == VKTS_ATTRIBUTE_TANGENT)
    	{
			mainContent += "    vec3 tangent = in_tangent.xyz;\n";
			mainContent += "    vec3 bitangent = cross(in_normal, in_tangent.xyz) * in_tangent.w;\n";

			jointDefineContent += "    vec3 bone_tangent = vec3(0.0, 0.0, 0.0);\n";
			jointDefineContent += "    vec3 bone_bitangent = vec3(0.0, 0.0, 0.0);\n";
			jointLoopContent += "                bone_tangent += (mat3(u_bufferBoneTransform.inverseModelMatrix) * mat3(u_bufferBoneTransform.jointMatrix[boneIndex]) * tangent) * boneWeight;\n";
			jointLoopContent += "                bone_bitangent += (mat3(u_bufferBoneTransform.inverseModelMatrix) * mat3(u_bufferBoneTransform.jointMatrix[boneIndex]) * bitangent) * boneWeight;\n";
			jointAssignContent += "    tangent = bone_tangent;\n";
			jointAssignContent += "    bitangent = bone_bitangent;\n";

			postMainContent += "    out_tangent = mat3(u_bufferViewProjection.viewMatrix) * mat3(u_bufferTransform.modelMatrix) * tangent;\n";
			postMainContent += "    out_bitangent = mat3(u_bufferViewProjection.viewMatrix) * mat3(u_bufferTransform.modelMatrix) * bitangent;\n";
    	}

		//

		if ((VKTS_ATTRIBUTE_TEXCOORD_0 & attributes) == VKTS_ATTRIBUTE_TEXCOORD_0)
    	{
			postMainContent += "    out_texcoord_0 = in_texcoord_0;\n";
    	}

		if ((VKTS_ATTRIBUTE_TEXCOORD_1 & attributes) == VKTS_ATTRIBUTE_TEXCOORD_1)
    	{
			postMainContent += "    out_texcoord_1 = in_texcoord_1;\n";
    	}

		mainContent += "    \n";

		//

		if (((VKTS_ATTRIBUTE_JOINTS_0 | VKTS_ATTRIBUTE_WEIGHTS_0) & attributes) == (VKTS_ATTRIBUTE_JOINTS_0 | VKTS_ATTRIBUTE_WEIGHTS_0))
    	{
			int32_t boneCount = 4;

			if (((VKTS_ATTRIBUTE_JOINTS_1 | VKTS_ATTRIBUTE_WEIGHTS_1) & attributes) == (VKTS_ATTRIBUTE_JOINTS_1 | VKTS_ATTRIBUTE_WEIGHTS_1))
	    	{
				boneCount = 8;
	    	}

			//

			textFile = fileLoadText((directory + extension + "/" + "main_joints.glsl").c_str());

			if (!textFile.get())
			{
				return "";
			}

			std::string jointString = "    int boneCount = " + std::to_string(boneCount) + ";\n";

			jointString += textFile->getString();

		    if (shaderFactoryReplace(jointString, "/*%VKTS_MAIN_JOINTS_DEFINE%*/", jointDefineContent) == 0)
		    {
		    	return "";
		    }

		    if (boneCount > 4)
		    {
				textFile = fileLoadText((directory + extension + "/" + "main_joints_check.glsl").c_str());

				if (!textFile.get())
				{
					return "";
				}

				if (shaderFactoryReplace(jointString, "/*%VKTS_MAIN_JOINTS_LOOP_CHECK%*/", textFile->getString()) == 0)
			    {
			    	return "";
			    }
		    }
		    else
		    {
			    if (shaderFactoryReplace(jointString, "/*%VKTS_MAIN_JOINTS_LOOP_CHECK%*/", "") == 0)
			    {
			    	return "";
			    }
		    }

		    if (shaderFactoryReplace(jointString, "/*%VKTS_MAIN_JOINTS_LOOP%*/", jointLoopContent) == 0)
		    {
		    	return "";
		    }

		    if (shaderFactoryReplace(jointString, "/*%VKTS_MAIN_JOINTS_ASSIGN%*/", jointAssignContent) == 0)
		    {
		    	return "";
		    }

			mainContent += jointString;
			mainContent += "    \n";
    	}

		mainContent += postMainContent;
	}
	else if (shaderStage == VK_SHADER_STAGE_FRAGMENT_BIT)
	{
		if ((VKTS_ATTRIBUTE_POSITION & attributes) == VKTS_ATTRIBUTE_POSITION)
    	{
			mainContent += "    vec4 position = u_bufferMatrices.inverseProjectionMatrix * in_position / in_position.w;\n";
			mainContent += "    vec3 incident = normalize(in_incident);\n";
    	}
		else
		{
			return "";
		}

		//

		if ((VKTS_ATTRIBUTE_NORMAL & attributes) == VKTS_ATTRIBUTE_NORMAL)
    	{
			mainContent += "    vec3 normal = normalize(in_normal);\n";
    	}
		else
		{
			mainContent += "    vec3 normal = vec3(0.0, 0.0, 1.0);\n";
		}

		if (((VKTS_ATTRIBUTE_NORMAL | VKTS_ATTRIBUTE_TANGENT) & attributes) == (VKTS_ATTRIBUTE_NORMAL | VKTS_ATTRIBUTE_TANGENT))
    	{
			mainContent += "    vec3 tangent = normalize(in_tangent);\n";
			mainContent += "    vec3 bitangent = normalize(in_bitangent);\n";
    	}
		else
		{
			mainContent += "    vec3 tangent = vec3(1.0, 0.0, 0.0);\n";
			mainContent += "    vec3 bitangent = vec3(0.0, 1.0, 0.0);\n";
		}

		//

		if ((VKTS_ATTRIBUTE_TEXCOORD_0 & attributes) == VKTS_ATTRIBUTE_TEXCOORD_0)
    	{
			mainContent += "    vec2 texcoord_0 = in_texcoord_0;\n";
    	}
		else
		{
			mainContent += "    vec2 texcoord_0 = vec2(0.5, 0.5);\n";
		}

		if ((VKTS_ATTRIBUTE_TEXCOORD_1 & attributes) == VKTS_ATTRIBUTE_TEXCOORD_1)
    	{
			mainContent += "    vec2 texcoord_1 = in_texcoord_1;\n";
    	}
		else
		{
			mainContent += "    vec2 texcoord_1 = vec2(0.5, 0.5);\n";
		}

		//

		if ((VKTS_ATTRIBUTE_COLOR_0 & attributes) == VKTS_ATTRIBUTE_COLOR_0)
    	{
			mainContent += "    vec4 color_0 = in_color_0;\n";
    	}
		else
		{
			mainContent += "    vec4 color_0 = vec4(1.0, 1.0, 1.0, 1.0);\n";
		}

		if ((VKTS_ATTRIBUTE_COLOR_1 & attributes) == VKTS_ATTRIBUTE_COLOR_1)
    	{
			mainContent += "    vec4 color_1 = in_color_1;\n";
    	}
		else
		{
			mainContent += "    vec4 color_1 = vec4(1.0, 1.0, 1.0, 1.0);\n";
		}

		mainContent += "    \n";

		//

		if ((attributes & VKTS_ATTRIBUTE_TANGENT) == VKTS_ATTRIBUTE_TANGENT)
	    {
			mainContent += "    mat3 objectToWorldMatrix = mat3(tangent, bitangent, normal);\n";
	    	mainContent += "    normal = objectToWorldMatrix * normalize((texture(u_normalTexture, texcoord_0.st).rgb * 2.0 - 1.0) * u_bufferMaterial.scale);\n";
			mainContent += "    \n";
	    }
	}

	if (shaderStage == VK_SHADER_STAGE_FRAGMENT_BIT)
	{
		//
	    // Material gathering.
		//

		textFile = fileLoadText((directory + extension + "/" + "material_main_pre.glsl").c_str());

		if (!textFile.get())
		{
			return "";
		}

		mainContent += textFile->getString();

		//

		if (material == VKTS_MATERIAL_METAL_ROUGHNESS)
		{
			textFile = fileLoadText((directory + extension + "/" + "pbr_mr_material_main_pre.glsl").c_str());

			if (!textFile.get())
			{
				return "";
			}

			mainContent += textFile->getString();
		}
		else if (material == VKTS_MATERIAL_SPECULAR_GLOSSINESS)
		{
			textFile = fileLoadText((directory + extension + "/" + "pbr_sg_material_main_pre.glsl").c_str());

			if (!textFile.get())
			{
				return "";
			}

			mainContent += textFile->getString();
		}
		else // VKTS_MATERIAL_COMMON
		{
			// TODO: Resolve main for common.
		}

		//
	    // Material processing.
		//

		textFile = fileLoadText((directory + extension + "/" + "material_main.glsl").c_str());

		if (!textFile.get())
		{
			return "";
		}

		mainContent += textFile->getString();

		if (material == VKTS_MATERIAL_METAL_ROUGHNESS)
		{
			textFile = fileLoadText((directory + extension + "/" + "pbr_mr_material_main.glsl").c_str());

			if (!textFile.get())
			{
				return "";
			}

			mainContent += textFile->getString();
		}
		else if (material == VKTS_MATERIAL_SPECULAR_GLOSSINESS)
		{
			textFile = fileLoadText((directory + extension + "/" + "pbr_sg_material_main.glsl").c_str());

			if (!textFile.get())
			{
				return "";
			}

			mainContent += textFile->getString();
		}
		else // VKTS_MATERIAL_COMMON
		{
			// TODO: Resolve main for common.
		}

		//

		textFile = fileLoadText((directory + extension + "/" + "material_light_main.glsl").c_str());

		if (!textFile.get())
		{
			return "";
		}

		std::string lightMain = textFile->getString();

		if (material == VKTS_MATERIAL_METAL_ROUGHNESS || material == VKTS_MATERIAL_SPECULAR_GLOSSINESS)
		{
			textFile = fileLoadText((directory + extension + "/" + "pbr_material_light_loop.glsl").c_str());

			if (!textFile.get())
			{
				return "";
			}

		    if (shaderFactoryReplace(lightMain, "/*%VKTS_LIGHT_LOOP%*/", textFile->getString()) == 0)
		    {
		    	return "";
		    }
		}
		else // VKTS_MATERIAL_COMMON
		{
			// TODO: Resolve main for common.
		}

		mainContent += lightMain;

		//

		if (material == VKTS_MATERIAL_METAL_ROUGHNESS || material == VKTS_MATERIAL_SPECULAR_GLOSSINESS)
		{
			textFile = fileLoadText((directory + extension + "/" + "tonemap_main_post.glsl").c_str());

			if (!textFile.get())
			{
				return "";
			}

			mainContent += textFile->getString();
		}
		else // VKTS_MATERIAL_COMMON
		{
			// TODO: Resolve main for common.
		}

		//

		textFile = fileLoadText((directory + extension + "/" + "material_main_post.glsl").c_str());

		if (!textFile.get())
		{
			return "";
		}

		mainContent += textFile->getString();
	}

	//

	//
	// Output section.
	//
	if (renderer == VKTS_RENDERER_FORWARD)
	{
		if (shaderStage == VK_SHADER_STAGE_FRAGMENT_BIT)
		{
			mainContent += "    \n";
			mainContent += "    // Writing out color.\n";
			mainContent += "    \n";
			mainContent += "    out_color = vec4(color, alpha);\n";
		}
	}
	else // VKTS_RENDERER_DEFERRED
	{
		// TODO: Resolve main for deferred rendering.
	}

    // Main.
    if (shaderFactoryReplace(shader, "/*%VKTS_MAIN%*/", mainContent) == 0)
    {
    	return "";
    }

    //

    return shader;
}

std::string VKTS_APIENTRY shaderFactoryCreateFilename(const VkTsMaterial material, const VkTsRenderer renderer, const VkTsAttributes attributes, const VkShaderStageFlagBits shaderStage)
{
	if (!shaderFactoryValidateAttributes(shaderStage))
	{
		return "";
	}

	//

	std::string materialName = "";

	switch (material)
	{
		case VKTS_MATERIAL_METAL_ROUGHNESS:
			materialName = "PBR_MR";
			break;
		case VKTS_MATERIAL_SPECULAR_GLOSSINESS:
			materialName = "PBR_SG";
			break;
		case VKTS_MATERIAL_COMMON:
			materialName = "COMMON";
			break;
		default:
			return "";
	}

	//
	std::string rendereName = "";

	switch (renderer)
	{
		case VKTS_RENDERER_FORWARD:
			rendereName = "FORWARD";
			break;
		case VKTS_RENDERER_DEFERRED:
			rendereName = "DEFERRED";
			break;
		case VKTS_RENDERER_RESOLVE:
			rendereName = "RESOLVE";
			break;
		default:
			return "";
	}

	//

	std::string attributesName = "";

	if ((attributes & VKTS_ATTRIBUTE_POSITION) == VKTS_ATTRIBUTE_POSITION)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_NORMAL) == VKTS_ATTRIBUTE_NORMAL)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_TANGENT) == VKTS_ATTRIBUTE_TANGENT)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_TEXCOORD_0) == VKTS_ATTRIBUTE_TEXCOORD_0)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_TEXCOORD_1) == VKTS_ATTRIBUTE_TEXCOORD_1)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_COLOR_0) == VKTS_ATTRIBUTE_COLOR_0)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_COLOR_1) == VKTS_ATTRIBUTE_COLOR_1)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_JOINTS_0) == VKTS_ATTRIBUTE_JOINTS_0)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_JOINTS_1) == VKTS_ATTRIBUTE_JOINTS_1)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_WEIGHTS_0) == VKTS_ATTRIBUTE_WEIGHTS_0)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	if ((attributes & VKTS_ATTRIBUTE_WEIGHTS_1) == VKTS_ATTRIBUTE_WEIGHTS_1)
	{
		attributesName += "1";
	}
	else
	{
		attributesName += "0";
	}

	//

	std::string extensionName = "";

	switch (shaderStage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
		{
			extensionName = ".vert";
			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		{
			extensionName = ".tesc";
			break;
		}
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		{
			extensionName = ".tese";
			break;
		}
		case VK_SHADER_STAGE_GEOMETRY_BIT:
		{
			extensionName = ".geom";
			break;
		}
		case VK_SHADER_STAGE_FRAGMENT_BIT:
		{
			extensionName = ".frag";
			break;
		}
		default:
			return "";
	}

	return materialName + "_" + rendereName + "_" + attributesName + extensionName;
}

}
