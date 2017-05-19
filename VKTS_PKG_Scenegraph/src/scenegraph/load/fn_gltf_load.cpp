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

#include <vkts/scenegraph/vkts_scenegraph.hpp>

#include "GltfVisitor.hpp"

#define VKTS_GLTF_MR_FORWARD_FRAGMENT_SHADER_NAME "shader/SPIR/V/glTF_mr_forward.frag.spv"
#define VKTS_GLTF_MR_FORWARD_NO_TEXCOORD_FRAGMENT_SHADER_NAME "shader/SPIR/V/glTF_mr_forward_no_texcoord.frag.spv"
#define VKTS_GLTF_MR_FORWARD_TWO_TEXCOORD_FRAGMENT_SHADER_NAME "shader/SPIR/V/glTF_mr_forward_two_texcoord.frag.spv"

#define VKTS_GLTF_SG_FORWARD_FRAGMENT_SHADER_NAME "shader/SPIR/V/glTF_sg_forward.frag.spv"
#define VKTS_GLTF_SG_FORWARD_NO_TEXCOORD_FRAGMENT_SHADER_NAME "shader/SPIR/V/glTF_sg_forward_no_texcoord.frag.spv"
#define VKTS_GLTF_SG_FORWARD_TWO_TEXCOORD_FRAGMENT_SHADER_NAME "shader/SPIR/V/glTF_sg_forward_two_texcoord.frag.spv"

namespace vkts
{

static ITextureObjectSP gltfProcessTextureObject(const GltfTexture* texture, const std::string& factorName, const float factor[4], const VkBool32 defaultWhite, const enum VkTsImageDataType imageDataType, const ISceneManagerSP& sceneManager)
{
	std::string textureObjectName;
	std::string imageObjectName;
	std::string imageDataName;

	if (texture && texture->source && texture->source->imageData.get())
	{
		textureObjectName = texture->name;

		imageObjectName = texture->source->name;

		imageDataName = texture->source->imageData->getName();
	}
	else if (imageDataType == VKTS_NORMAL_DATA)
	{
		textureObjectName = "INTERNAL_NORMAL";

		imageObjectName = "INTERNAL_NORMAL";

		imageDataName = "INTERNAL_NORMAL";
	}
	else if (defaultWhite)
	{
		textureObjectName = "INTERNAL_WHITE";

		imageObjectName = "INTERNAL_WHITE";

		imageDataName = "INTERNAL_WHITE";
	}
	else
	{
		textureObjectName = "INTERNAL_BLACK";

		imageObjectName = "INTERNAL_BLACK";

		imageDataName = "INTERNAL_BLACK";
	}

	textureObjectName += factorName;

	imageObjectName += factorName;

	imageDataName += factorName;

	//

	VkBool32 mipmap = texture ? VK_TRUE : VK_FALSE;

	//

	ITextureObjectSP textureObject = sceneManager->useTextureObject(textureObjectName);

	if (textureObject.get())
	{
		return textureObject;
	}

	//

	IImageObjectSP imageObject = sceneManager->useImageObject(imageObjectName);

	if (imageObject.get())
	{
		textureObject = createTextureObject(sceneManager->getAssetManager(), textureObjectName, mipmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

		if (!textureObject.get())
		{
			return ITextureObjectSP();
		}

		sceneManager->addTextureObject(textureObject);

		//

		textureObject = sceneManager->useTextureObject(textureObjectName);

		return textureObject;
	}

	//

	IImageDataSP imageData = sceneManager->useImageData(imageDataName);

	if (!imageData.get())
	{
		if (texture && texture->source && texture->source->imageData.get())
		{
			if (imageDataType == VKTS_NORMAL_DATA)
			{
				imageData = imageDataConvert(texture->source->imageData, texture->source->imageData->getFormat(), imageDataName, imageDataType, imageDataType, glm::vec4(factor[0], factor[1], 1.0f, 1.0f));
			}
			else
			{
				imageData = imageDataConvert(texture->source->imageData, texture->source->imageData->getFormat(), imageDataName, imageDataType, imageDataType, glm::vec4(factor[0], factor[1], factor[2], factor[3]));
			}
		}
		else
		{
			if (imageDataType == VKTS_NORMAL_DATA)
			{
				imageData = imageDataCreate(imageDataName, 1, 1, 1, glm::vec4(0.5f * factor[0], 0.5f * factor[1], 1.0f, 1.0f), VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM);
			}
			else
			{
				float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

				if (!defaultWhite)
				{
					color[0] = 0.0f;
					color[1] = 0.0f;
					color[2] = 0.0f;
					color[3] = 1.0f;
				}

				imageData = imageDataCreate(imageDataName, 1, 1, 1, glm::vec4(factor[0] * color[0], factor[1] * color[1], factor[2] * color[2], factor[3] * color[3]), VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM);
			}

		}

		if (!imageData.get())
		{
			return ITextureObjectSP();
		}

		//

		imageData = createDeviceImageData(sceneManager->getAssetManager(), imageData);

		//

		sceneManager->addImageData(imageData);

		//

		imageData = sceneManager->useImageData(imageDataName);
	}

	if (imageData.get())
	{
		imageObject = createImageObject(sceneManager->getAssetManager(), imageObjectName, imageData, VK_FALSE);

		if (!imageObject.get())
		{
			return ITextureObjectSP();
		}

		sceneManager->addImageObject(imageObject);

		imageObject = sceneManager->useImageObject(imageObjectName);

		//

		textureObject = createTextureObject(sceneManager->getAssetManager(), textureObjectName, mipmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

		if (!textureObject.get())
		{
			return ITextureObjectSP();
		}

		sceneManager->addTextureObject(textureObject);

		//

		textureObject = sceneManager->useTextureObject(textureObjectName);

		return textureObject;
	}

	return ITextureObjectSP();
}

static ITextureObjectSP gltfProcessTextureObject(const GltfTexture* texture, const float factor[4], const VkBool32 defaultWhite, const enum VkTsImageDataType imageDataType, const ISceneManagerSP& sceneManager)
{
	std::string factorName = "_" + std::to_string(factor[0]) + "_" + std::to_string(factor[1]) + "_" + std::to_string(factor[2]) + "_" + std::to_string(factor[3]);

	return gltfProcessTextureObject(texture, factorName, factor, defaultWhite, imageDataType, sceneManager);
}

static ITextureObjectSP gltfProcessTextureObject(const GltfTexture* texture, const float factor, const VkBool32 defaultWhite, const enum VkTsImageDataType imageDataType, const ISceneManagerSP& sceneManager)
{
	std::string factorName = "_" + std::to_string(factor);

	const float tempFactor[4] = {factor, factor, factor, factor};

	return gltfProcessTextureObject(texture, factorName, tempFactor, defaultWhite, imageDataType, sceneManager);
}

static VkBool32 gltfProcessSubMesh(ISubMeshSP& subMesh, const GltfVisitor& visitor, const GltfPrimitive& gltfPrimitive, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	if (!gltfPrimitive.position)
	{
		return VK_FALSE;
	}

	//

	VkBool32 useTangents = VK_FALSE;
	VkBool32 createTangents = VK_FALSE;

	if (gltfPrimitive.normal && (gltfPrimitive.texCoord0 || gltfPrimitive.tangent))
	{
		useTangents = VK_TRUE;
		createTangents = VK_TRUE;
	}

	//

	subMesh->setNumberVertices((int32_t)gltfPrimitive.position->count);

	uint32_t totalSize = 0;
	uint32_t strideInBytes = 0;

    VkTsVertexBufferType vertexBufferType = 0;

    //

    if (!visitor.isFloat(gltfPrimitive.position->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.position->type) == 3 || visitor.getComponentsPerType(gltfPrimitive.position->type) == 4))
    {
    	return VK_FALSE;
    }

    subMesh->setVertexOffset(strideInBytes);
    strideInBytes += 4 * sizeof(float);

    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_VERTEX;

	if (gltfPrimitive.normal)
	{
        if (gltfPrimitive.normal->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Normal has different size");

            return VK_FALSE;
        }

        if (!visitor.isFloat(gltfPrimitive.normal->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.normal->type) == 3))
        {
        	return VK_FALSE;
        }

        subMesh->setNormalOffset(strideInBytes);
        strideInBytes += 3 * sizeof(float);

        totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_NORMAL;
	}

    if (useTangents)
    {
        subMesh->setBitangentOffset(strideInBytes);
        strideInBytes += 3 * sizeof(float);

        totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

        subMesh->setTangentOffset(strideInBytes);
        strideInBytes += 3 * sizeof(float);

        totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

        //

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TANGENTS;
    }

	if (gltfPrimitive.texCoord0)
	{
        if (gltfPrimitive.texCoord0->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Texture coordinate has different size");

            return VK_FALSE;
        }

        // TODO: Support normalized UNSIGNET_SHORT and UNSIGNED_BYTE.
        if (!visitor.isFloat(gltfPrimitive.texCoord0->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.texCoord0->type) == 2))
        {
        	return VK_FALSE;
        }

        subMesh->setTexcoord0Offset(strideInBytes);
        strideInBytes += 2 * sizeof(float);

        totalSize += 2 * sizeof(float) * subMesh->getNumberVertices();

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0;
	}

	if (gltfPrimitive.texCoord1)
	{
        if (gltfPrimitive.texCoord1->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Texture coordinate has different size");

            return VK_FALSE;
        }

        // TODO: Support normalized UNSIGNET_SHORT and UNSIGNED_BYTE.
        if (!visitor.isFloat(gltfPrimitive.texCoord1->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.texCoord1->type) == 2))
        {
        	return VK_FALSE;
        }

        subMesh->setTexcoord1Offset(strideInBytes);
        strideInBytes += 2 * sizeof(float);

        totalSize += 2 * sizeof(float) * subMesh->getNumberVertices();

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1;
	}

    if (gltfPrimitive.joints0 && gltfPrimitive.weights0)
    {
    	if (gltfPrimitive.joints0->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Joint has different size");

            return VK_FALSE;
        }

        if (!(visitor.isUnsignedShort(gltfPrimitive.joints0->componentType) || visitor.isUnsignedShort(gltfPrimitive.joints0->componentType)) || !(visitor.getComponentsPerType(gltfPrimitive.joints0->type) == 4))
        {
        	return VK_FALSE;
        }

        //

        if (gltfPrimitive.weights0->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Weight has different size");

            return VK_FALSE;
        }

        // TODO: Support normalized UNSIGNET_SHORT and UNSIGNED_BYTE.
        if (!visitor.isFloat(gltfPrimitive.weights0->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.weights0->type) == 4))
        {
        	return VK_FALSE;
        }

        //

        if (gltfPrimitive.joints1 && gltfPrimitive.weights1)
        {
        	if (gltfPrimitive.joints1->count != gltfPrimitive.position->count)
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Joint has different size");

                return VK_FALSE;
            }

            if (!(visitor.isUnsignedShort(gltfPrimitive.joints1->componentType) || visitor.isUnsignedShort(gltfPrimitive.joints1->componentType)) || !(visitor.getComponentsPerType(gltfPrimitive.joints1->type) == 4))
            {
            	return VK_FALSE;
            }

            //

            // TODO: Support normalized UNSIGNET_SHORT and UNSIGNED_BYTE.
            if (gltfPrimitive.weights1->count != gltfPrimitive.position->count)
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Weight has different size");

                return VK_FALSE;
            }

            if (!visitor.isFloat(gltfPrimitive.weights1->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.weights1->type) == 4))
            {
            	return VK_FALSE;
            }
        }

        subMesh->setBoneIndices0Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //

        subMesh->setBoneIndices1Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //
        //

        subMesh->setBoneWeights0Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //

        subMesh->setBoneWeights1Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //
        //

        subMesh->setNumberBonesOffset(strideInBytes);
        strideInBytes += 1 * sizeof(float);

        totalSize += 1 * sizeof(float) * subMesh->getNumberVertices();

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_BONES;
    }

    // TODO: Support COLOR_0 and COLOR_1

    subMesh->setStrideInBytes(strideInBytes);

	//
	// Indices
	//

	if (gltfPrimitive.indices)
	{
		if (visitor.getComponentsPerType(gltfPrimitive.indices->type) != 1)
		{
			return VK_FALSE;
		}

		subMesh->setNumberIndices((int32_t)gltfPrimitive.indices->count);
	}
	else
	{
		subMesh->setNumberIndices((int32_t)gltfPrimitive.position->count);
	}

	uint32_t size = sizeof(int32_t) * subMesh->getNumberIndices();

    auto indicesBinaryBuffer = binaryBufferCreate(size);

    if (!indicesBinaryBuffer.get() || indicesBinaryBuffer->getSize() != size)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create indices binary buffer");

        return VK_FALSE;
    }

    for (uint32_t i = 0; i < (uint32_t)subMesh->getNumberIndices(); i++)
    {
    	int32_t index = (int32_t)i;

    	if (gltfPrimitive.indices)
    	{
    		const void* currentVoidPointer = visitor.getBufferPointer(*gltfPrimitive.indices, i);

    		if (!currentVoidPointer)
    		{
    			return VK_FALSE;
    		}

    		switch (gltfPrimitive.indices->componentType)
    		{
    			case 5120:
    				index = (int32_t)*((const int8_t*)currentVoidPointer);
    				break;
    			case 5121:
    				index = (int32_t)*((const uint8_t*)currentVoidPointer);
    				break;
    			case 5122:
    				index = (int32_t)*((const int16_t*)currentVoidPointer);
    				break;
    			case 5123:
    				index = (int32_t)*((const uint16_t*)currentVoidPointer);
    				break;
    			case 5125:
    				index = (int32_t)*((const uint32_t*)currentVoidPointer);
    				break;
    			case 5126:
    				index = (int32_t)*((const float*)currentVoidPointer);
    				break;
    			default:
    				return VK_FALSE;
    		}
    	}

    	indicesBinaryBuffer->write((const void*)&index, 1, sizeof(int32_t));
    }

    //

    auto indexVertexBuffer = createIndexBufferObject(sceneManager->getAssetManager(), indicesBinaryBuffer);

    if (!indexVertexBuffer.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create indices vertex buffer");

        return VK_FALSE;
    }

    //

    subMesh->setIndexBuffer(indexVertexBuffer, indicesBinaryBuffer);

    //
    //

    if (totalSize > 0)
    {
        IBinaryBufferSP tempBinaryBuffer;

    	if (createTangents)
    	{
        	uint32_t size = sizeof(float) * 3 * 2 * subMesh->getNumberVertices();

            tempBinaryBuffer = binaryBufferCreate(size);

            if (!tempBinaryBuffer.get() || tempBinaryBuffer->getSize() != size)
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create temporary binary buffer");

                return VK_FALSE;
            }

            if (gltfPrimitive.tangent && gltfPrimitive.normal)
            {
				for (uint32_t i = 0; i < (uint32_t)subMesh->getNumberVertices(); i++)
				{
					auto* currentNormal = visitor.getFloatPointer(*gltfPrimitive.normal, i);

					if (!currentNormal)
					{
						return VK_FALSE;
					}

					auto* tangent4 = visitor.getFloatPointer(*gltfPrimitive.tangent, i);

					if (!tangent4)
					{
						return VK_FALSE;
					}

					glm::vec3 normal(currentNormal[0], currentNormal[1], currentNormal[2]);
					glm::vec3 tangent(tangent4[0], tangent4[1], tangent4[2]);
					glm::vec3 bitangent = glm::cross(normal, tangent) * tangent4[3];

					tempBinaryBuffer->seek(i * 3 * 2 * sizeof(float), VKTS_SEARCH_ABSOLUTE);
					tempBinaryBuffer->write(glm::value_ptr(bitangent), 1, 3 * sizeof(float));
					tempBinaryBuffer->write(glm::value_ptr(tangent), 1, 3 * sizeof(float));
				}
            }
            else
            {
				// Store all tangents and indices. Bitangents are calculated out of normals and tangents.
				std::vector<glm::vec3> tangents(subMesh->getNumberIndices());
				std::vector<int32_t> indicesToVertex(subMesh->getNumberIndices());

				for (uint32_t i = 0; i < (uint32_t)subMesh->getNumberIndices() / 3; i++)
				{
					int32_t index[3];

					const float* currentFloatData = nullptr;

					//

					glm::vec4 v[3];
					glm::vec2 uv[3];

					for (uint32_t k = 0; k < 3; k++)
					{
						index[k] = ((const int32_t*)indicesBinaryBuffer->getData())[i * 3 + k];

						indicesToVertex[i * 3 + k] = index[k];

						//

						currentFloatData = visitor.getFloatPointer(*gltfPrimitive.position, index[k]);

						if (!currentFloatData)
						{
							return VK_FALSE;
						}

						v[k] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

						for (uint32_t m = 0; m < visitor.getComponentsPerType(gltfPrimitive.position->type); m++)
						{
							v[k][m] = currentFloatData[m];
						}

						//

						currentFloatData = visitor.getFloatPointer(*gltfPrimitive.texCoord0, index[k]);

						if (!currentFloatData)
						{
							return VK_FALSE;
						}

						uv[k] = glm::vec2(0.0f, 0.0f);

						uv[k].s = currentFloatData[0];
						uv[k].t = currentFloatData[1];
					}

					//

					glm::vec3 deltaPos[2];

					deltaPos[0] = glm::vec3(v[1]) - glm::vec3(v[0]);
					deltaPos[1] = glm::vec3(v[2]) - glm::vec3(v[0]);

					glm::vec2 deltaUV[2];

					deltaUV[0] = uv[1] - uv[0];
					deltaUV[1] = uv[2] - uv[0];

					//

					float divisor = (deltaUV[0].x * deltaUV[1].y - deltaUV[0].y * deltaUV[1].x);

					if (divisor == 0.0f)
					{
						continue;
					}

					float r = 1.0f / divisor;

					glm::vec3 tangent = glm::normalize((deltaPos[0] * deltaUV[1].y - deltaPos[1] * deltaUV[0].y) * r);

					//

					for (uint32_t k = 0; k < 3; k++)
					{
						tangents[i * 3 + k] = tangent;
					}
				}

				// Accumulate all calculated tangents per vertex ...
				std::vector<glm::vec3> tangentsPerVertex(subMesh->getNumberVertices());

				for (uint32_t i = 0; i < (uint32_t)subMesh->getNumberIndices(); i++)
				{
					tangentsPerVertex[indicesToVertex[i]] += tangents[i];
				}

				//

				// ... and write the normalized result. Also, calculate and write bitangent.
				for (uint32_t i = 0; i < (uint32_t)subMesh->getNumberVertices(); i++)
				{
					auto* normal = visitor.getFloatPointer(*gltfPrimitive.normal, i);

					if (!normal)
					{
						return VK_FALSE;
					}

					glm::vec3 tangent = glm::vec3(normal[2], normal[0], normal[1]);

					if (tangentsPerVertex[i].x != 0.0f || tangentsPerVertex[i].y != 0.0f || tangentsPerVertex[i].z != 0.0f)
					{
						tangent = tangentsPerVertex[i];
					}

					tangent = glm::normalize(tangent);

					auto bitangent = glm::cross(glm::normalize(glm::vec3(normal[0], normal[1], normal[2])), tangent);

					tempBinaryBuffer->seek(i * 3 * 2 * sizeof(float), VKTS_SEARCH_ABSOLUTE);
					tempBinaryBuffer->write(glm::value_ptr(bitangent), 1, 3 * sizeof(float));
					tempBinaryBuffer->write(glm::value_ptr(tangent), 1, 3 * sizeof(float));
				}
            }

            tempBinaryBuffer->seek(0, VKTS_SEARCH_ABSOLUTE);
    	}

    	//

        auto vertexBinaryBuffer = binaryBufferCreate((uint32_t)totalSize);

        if (!vertexBinaryBuffer.get() || vertexBinaryBuffer->getSize() != (uint32_t)totalSize)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex binary buffer");

            return VK_FALSE;
        }

        for (int32_t currentVertexElement = 0; currentVertexElement < subMesh->getNumberVertices(); currentVertexElement++)
        {
        	const float* currentFloatData = nullptr;

            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_VERTEX)
            {
            	float vertex[4] = {0.0f, 0.0f, 0.0f, 1.0f};

            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.position, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

            	for (uint32_t i = 0; i < visitor.getComponentsPerType(gltfPrimitive.position->type); i++)
            	{
            		vertex[i] = currentFloatData[i];
            	}

                vertexBinaryBuffer->write((const void*)vertex, 1, 4 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_NORMAL)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.normal, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

            	glm::vec3 normal = glm::normalize(glm::vec3(currentFloatData[0], currentFloatData[1], currentFloatData[2]));

                vertexBinaryBuffer->write(glm::value_ptr(normal), 1, 3 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BITANGENT)
            {
            	float binormal[3];

            	tempBinaryBuffer->read((void*)binormal, 1, 3 * sizeof(float));

            	glm::vec3 binormalNormalized = glm::normalize(glm::vec3(binormal[0], binormal[1], binormal[2]));

                vertexBinaryBuffer->write(glm::value_ptr(binormalNormalized), 1, 3 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TANGENT)
            {
            	float tangent[3];

            	tempBinaryBuffer->read((void*)tangent, 1, 3 * sizeof(float));

            	glm::vec3 tangentNormalized = glm::normalize(glm::vec3(tangent[0], tangent[1], tangent[2]));

                vertexBinaryBuffer->write(glm::value_ptr(tangentNormalized), 1, 3 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.texCoord0, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)currentFloatData, 1, 2 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.texCoord1, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)currentFloatData, 1, 2 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0)
            {
				const uint8_t* currentUnsignedBytePointer = visitor.getUnsignedBytePointer(*gltfPrimitive.joints0, currentVertexElement);
				const uint16_t* currentUnsignedShortPointer = visitor.getUnsignedShortPointer(*gltfPrimitive.joints0, currentVertexElement);

				if (!currentUnsignedBytePointer && !currentUnsignedShortPointer)
				{
					return VK_FALSE;
				}

            	for (uint32_t i = 0; i < 4; i++)
            	{
					float value = 0.0;

					if (currentUnsignedBytePointer)
					{
						value = (float)currentUnsignedBytePointer[i];
					}
					else
					{
						value = (float)currentUnsignedShortPointer[i];
					}

					vertexBinaryBuffer->write((const void*)&value, 1, 1 * sizeof(float));
            	}
	        }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1)
            {
            	if (gltfPrimitive.joints1)
            	{
    				const uint8_t* currentUnsignedBytePointer = visitor.getUnsignedBytePointer(*gltfPrimitive.joints1, currentVertexElement);
    				const uint16_t* currentUnsignedShortPointer = visitor.getUnsignedShortPointer(*gltfPrimitive.joints1, currentVertexElement);

    				if (!currentUnsignedBytePointer && !currentUnsignedShortPointer)
    				{
    					return VK_FALSE;
    				}

                	for (uint32_t i = 0; i < 4; i++)
                	{
    					float value = 0.0;

    					if (currentUnsignedBytePointer)
    					{
    						value = (float)currentUnsignedBytePointer[i];
    					}
    					else
    					{
    						value = (float)currentUnsignedShortPointer[i];
    					}

    					vertexBinaryBuffer->write((const void*)&value, 1, 1 * sizeof(float));
                	}
            	}
            	else
            	{
					float boneIndices1[4] = {-1.0f, -1.0f, -1.0f, -1.0f};

					// Do not change, as data not given.

					vertexBinaryBuffer->write((const void*)boneIndices1, 1, 4 * sizeof(float));
            	}
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.weights0, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)currentFloatData, 1, 4 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1)
            {
            	if (gltfPrimitive.weights1)
            	{
                	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.weights1, currentVertexElement);

                	if (!currentFloatData)
                	{
                		return VK_FALSE;
                	}

                    vertexBinaryBuffer->write((const void*)currentFloatData, 1, 4 * sizeof(float));
            	}
            	else
            	{
					float boneWeights1[4] = {0.0f, 0.0f, 0.0f, 0.0f};

					// Do not change, as data not given.

					vertexBinaryBuffer->write((const void*)boneWeights1, 1, 4 * sizeof(float));
            	}
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS)
            {
            	float numberBones[1] = {4};

            	if (gltfPrimitive.joints1 && gltfPrimitive.weights1)
            	{
            		numberBones[0] += 4;
            	}

                vertexBinaryBuffer->write((const void*)numberBones, 1, 1 * sizeof(float));
            }
        }

        //

        auto vertexBuffer = createVertexBufferObject(sceneManager->getAssetManager(), vertexBinaryBuffer);

        if (!vertexBuffer.get())
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex buffer");

            return VK_FALSE;
        }

        //

        subMesh->setVertexBuffer(vertexBuffer, vertexBufferType, Aabb((const float*)vertexBinaryBuffer->getData(), subMesh->getNumberVertices(), subMesh->getStrideInBytes()), vertexBinaryBuffer);
    }
    else
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh incomplete");

        return VK_FALSE;
    }

    //

	switch (gltfPrimitive.mode)
	{
		case 0:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
			break;
		case 1:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
			break;
		case 2:
			return VK_FALSE;
			break;
		case 3:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
			break;
		case 4:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
			break;
		case 5:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
			break;
		case 6:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);
			break;
		default:
			return VK_FALSE;
	}

	// Optional

	GltfMaterial defaultMaterial;

	defaultMaterial.alphaMode = "OPAQUE";
	defaultMaterial.alphaCutoff = 0.5f;
	defaultMaterial.doubleSided = VK_FALSE;

	defaultMaterial.useSpecularGlossiness = VK_FALSE;

	//

	defaultMaterial.pbrMetallicRoughness.baseColorFactor[0] = 1.0f;
	defaultMaterial.pbrMetallicRoughness.baseColorFactor[1] = 1.0f;
	defaultMaterial.pbrMetallicRoughness.baseColorFactor[2] = 1.0f;
	defaultMaterial.pbrMetallicRoughness.baseColorFactor[3] = 1.0f;
	defaultMaterial.pbrMetallicRoughness.baseColorTexture = nullptr;

	defaultMaterial.pbrMetallicRoughness.metallicFactor = 1.0f;

	defaultMaterial.pbrMetallicRoughness.roughnessFactor = 1.0f;
	defaultMaterial.pbrMetallicRoughness.metallicRoughnessTexture = nullptr;

	//

	defaultMaterial.pbrSpecularGlossiness.diffuseFactor[0] = 1.0f;
	defaultMaterial.pbrSpecularGlossiness.diffuseFactor[1] = 1.0f;
	defaultMaterial.pbrSpecularGlossiness.diffuseFactor[2] = 1.0f;
	defaultMaterial.pbrSpecularGlossiness.diffuseFactor[3] = 1.0f;
	defaultMaterial.pbrSpecularGlossiness.diffuseTexture = nullptr;

	defaultMaterial.pbrSpecularGlossiness.specularFactor[0] = 1.0f;
	defaultMaterial.pbrSpecularGlossiness.specularFactor[1] = 1.0f;
	defaultMaterial.pbrSpecularGlossiness.specularFactor[2] = 1.0f;

	defaultMaterial.pbrSpecularGlossiness.glossinessFactor = 1.0f;

	defaultMaterial.pbrSpecularGlossiness.specularGlossinessTexture = nullptr;

	//

	defaultMaterial.normalScale = 1.0f;
	defaultMaterial.normalTexture = nullptr;

	defaultMaterial.occlusionStrength = 1.0f;
	defaultMaterial.occlusionTexture = nullptr;

	defaultMaterial.emissiveFactor[0] = 0.0f;
	defaultMaterial.emissiveFactor[1] = 0.0f;
	defaultMaterial.emissiveFactor[2] = 0.0f;
	defaultMaterial.emissiveTexture = nullptr;

	defaultMaterial.name = "VKTS_DEFAULT_MATERIAL";

	//

	GltfMaterial* material = gltfPrimitive.material;

	if (!material)
	{
		material = &defaultMaterial;
	}

	//

	auto bsdfMaterial = sceneManager->useBSDFMaterial(material->name);

	if (!bsdfMaterial.get())
	{
		bsdfMaterial = sceneFactory->createBSDFMaterial(sceneManager, VK_TRUE);

		if (!bsdfMaterial.get())
		{
			return VK_FALSE;
		}

		bsdfMaterial->setName(material->name);

		bsdfMaterial->setSorted(VK_TRUE);
		bsdfMaterial->setPacked(VK_TRUE);

		if (material->alphaMode == "BLEND")
		{
			bsdfMaterial->setTransparent(VK_TRUE);
		}
		else if (material->alphaMode == "MASK")
		{
			bsdfMaterial->setAlphaCutoff(material->alphaCutoff);
		}

		if (material->doubleSided)
		{
			subMesh->setDoubleSided(VK_TRUE);
		}

		if (material->useSpecularGlossiness)
		{
			//
			// Diffuse
			//

			ITextureObjectSP diffuse = gltfProcessTextureObject(material->pbrSpecularGlossiness.diffuseTexture, material->pbrSpecularGlossiness.diffuseFactor, VK_TRUE, VKTS_LDR_COLOR_DATA, sceneManager);

			if (!diffuse.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(diffuse);
			if (material->pbrSpecularGlossiness.diffuseTexture)
			{
				bsdfMaterial->setTexCoordIndex(0, material->pbrSpecularGlossiness.diffuseTexture->texCoord);
			}

			//
			// Specular glossiness
			//

			float specularGlossinessFactors[] = {material->pbrSpecularGlossiness.specularFactor[0], material->pbrSpecularGlossiness.specularFactor[1], material->pbrSpecularGlossiness.specularFactor[2], material->pbrSpecularGlossiness.glossinessFactor};

			ITextureObjectSP specularGlossiness = gltfProcessTextureObject(material->pbrSpecularGlossiness.specularGlossinessTexture, specularGlossinessFactors, VK_TRUE, VKTS_NON_COLOR_DATA, sceneManager);

			if (!specularGlossiness.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(specularGlossiness);
			if (material->pbrSpecularGlossiness.specularGlossinessTexture)
			{
				bsdfMaterial->setTexCoordIndex(1, material->pbrSpecularGlossiness.specularGlossinessTexture->texCoord);
			}
		}
		else
		{
			//
			// Base color
			//

			ITextureObjectSP baseColor = gltfProcessTextureObject(material->pbrMetallicRoughness.baseColorTexture, material->pbrMetallicRoughness.baseColorFactor, VK_TRUE, VKTS_LDR_COLOR_DATA, sceneManager);

			if (!baseColor.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(baseColor);
			if (material->pbrMetallicRoughness.baseColorTexture)
			{
				bsdfMaterial->setTexCoordIndex(0, material->pbrMetallicRoughness.baseColorTexture->texCoord);
			}

			//
			// Metallic roughness
			//

			float metallicRoughnessFactors[] = {1.0f, material->pbrMetallicRoughness.roughnessFactor, material->pbrMetallicRoughness.metallicFactor, 1.0f};

			ITextureObjectSP metallicRoughness = gltfProcessTextureObject(material->pbrMetallicRoughness.metallicRoughnessTexture, metallicRoughnessFactors, VK_TRUE, VKTS_NON_COLOR_DATA, sceneManager);

			if (!metallicRoughness.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(metallicRoughness);
			if (material->pbrMetallicRoughness.metallicRoughnessTexture)
			{
				bsdfMaterial->setTexCoordIndex(1, material->pbrMetallicRoughness.metallicRoughnessTexture->texCoord);
			}
		}

		//
		// Normal
		//

		ITextureObjectSP normal = gltfProcessTextureObject(material->normalTexture, material->normalScale, VK_TRUE, VKTS_NORMAL_DATA, sceneManager);

		if (!normal.get())
		{
			return VK_FALSE;
		}

		bsdfMaterial->addTextureObject(normal);
		if (material->normalTexture)
		{
			bsdfMaterial->setTexCoordIndex(2, material->normalTexture->texCoord);
		}

		//
		// Ambient occlusion
		//

		ITextureObjectSP ambientOcclusion = gltfProcessTextureObject(material->occlusionTexture, 1.0f, VK_TRUE, VKTS_NON_COLOR_DATA, sceneManager);

		if (!ambientOcclusion.get())
		{
			return VK_FALSE;
		}

		bsdfMaterial->setAmbientOcclusionStrength(material->occlusionStrength);

		bsdfMaterial->addTextureObject(ambientOcclusion);
		if (material->occlusionTexture)
		{
			bsdfMaterial->setTexCoordIndex(3, material->occlusionTexture->texCoord);
		}

		//
		// Emissive
		//

		float emissiveFactors[] = {material->emissiveFactor[0], material->emissiveFactor[1], material->emissiveFactor[2], 1.0f};

		ITextureObjectSP emissive = gltfProcessTextureObject(material->emissiveTexture, emissiveFactors, VK_TRUE, VKTS_LDR_COLOR_DATA, sceneManager);

		if (!emissive.get())
		{
			return VK_FALSE;
		}

		bsdfMaterial->addTextureObject(emissive);
		if (material->emissiveTexture)
		{
			bsdfMaterial->setTexCoordIndex(4, material->emissiveTexture->texCoord);
		}

		//
		// Attribute
		//

		bsdfMaterial->setAttributes(subMesh->getVertexBufferType());

		//
		// Shader
		//

		const char* fragmentShader = nullptr;

		if (material->useSpecularGlossiness)
		{
			if ((bsdfMaterial->getAttributes() & (VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0 | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1)) == (VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0 | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1))
			{
				fragmentShader = VKTS_GLTF_SG_FORWARD_TWO_TEXCOORD_FRAGMENT_SHADER_NAME;
			}
			else if ((bsdfMaterial->getAttributes() & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0) == VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0)
			{
				fragmentShader = VKTS_GLTF_SG_FORWARD_FRAGMENT_SHADER_NAME;
			}
			else
			{
				fragmentShader = VKTS_GLTF_SG_FORWARD_NO_TEXCOORD_FRAGMENT_SHADER_NAME;
			}
		}
		else
		{
			if ((bsdfMaterial->getAttributes() & (VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0 | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1)) == (VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0 | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1))
			{
				fragmentShader = VKTS_GLTF_MR_FORWARD_TWO_TEXCOORD_FRAGMENT_SHADER_NAME;
			}
			else if ((bsdfMaterial->getAttributes() & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0) == VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0)
			{
				fragmentShader = VKTS_GLTF_MR_FORWARD_FRAGMENT_SHADER_NAME;
			}
			else
			{
				fragmentShader = VKTS_GLTF_MR_FORWARD_NO_TEXCOORD_FRAGMENT_SHADER_NAME;
			}
		}

		auto shaderModule = sceneManager->useFragmentShaderModule(fragmentShader);

		if (!shaderModule.get())
		{
			std::string finalFilename = visitor.getDirectory() + std::string(fragmentShader);

			auto shaderBinary = fileLoadBinary(finalFilename.c_str());

			if (!shaderBinary.get())
			{
				shaderBinary = fileLoadBinary(fragmentShader);

				if (!shaderBinary.get())
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", fragmentShader, VKTS_MAX_TOKEN_CHARS);

					return VK_FALSE;
				}
			}

			//

			shaderModule = createShaderModule(sceneManager->getAssetManager(), fragmentShader, shaderBinary);

			if (!shaderModule.get())
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

				return VK_FALSE;
			}

			sceneManager->addFragmentShaderModule(shaderModule);
		}

		bsdfMaterial->setFragmentShader(shaderModule);
		bsdfMaterial->setSpecularGlossiness(material->useSpecularGlossiness);
	}

	subMesh->setBSDFMaterial(bsdfMaterial);

	if (!sceneFactory->getSceneRenderFactory()->prepareBSDFMaterial(sceneManager, subMesh))
	{
		return VK_FALSE;
	}

	return VK_TRUE;
}

static VkBool32 gltfProcessNode(INodeSP& node, const GltfVisitor& visitor, const GltfNode& gltfNode, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	// Process translation, rotation and scale.

	node->setTranslate(glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]));

	Quat quat(gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2], gltfNode.rotation[3]);

	node->setRotate(normalize(quat));

	node->setScale(glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]));

	// TODO: Process camera.

    // Process mesh.
    if (gltfNode.mesh)
    {
    	if (!sceneFactory->getSceneRenderFactory()->prepareTransformUniformBuffer(sceneManager, node))
    	{
            return VK_FALSE;
    	}

        auto currentMesh = sceneManager->useMesh(gltfNode.mesh->name);

        if (!currentMesh.get())
        {
			auto mesh = sceneFactory->createMesh(sceneManager);

			if (!mesh.get())
			{
				return VK_FALSE;
			}

			mesh->setName(gltfNode.mesh->name);

			sceneManager->addMesh(mesh);

			//

			for (uint32_t k = 0; k < gltfNode.mesh->primitives.size(); k++)
			{
				auto subMesh = sceneFactory->createSubMesh(sceneManager);

				if (!subMesh.get())
				{
					return VK_FALSE;
				}

				subMesh->setName(gltfNode.mesh->name + "_" + gltfNode.mesh->primitives[k].name);

				sceneManager->addSubMesh(subMesh);

				//

				if (!gltfProcessSubMesh(subMesh, visitor, gltfNode.mesh->primitives[k], sceneManager, sceneFactory))
				{
					return VK_FALSE;
				}

				//

				auto currentSubMesh = sceneManager->useSubMesh(subMesh->getName());

				mesh->addSubMesh(currentSubMesh);
			}

			currentMesh = sceneManager->useMesh(mesh->getName());

			if (!currentMesh.get())
	        {
				return VK_FALSE;
	        }
        }

        node->addMesh(currentMesh);
    }

    //

    for (uint32_t skinIndex = 0; skinIndex < visitor.getAllGltfSkins().size(); skinIndex++)
    {
    	const auto& skin = visitor.getAllGltfSkins()[skinIndex];

    	if (&visitor.getAllGltfNodes()[skin.skeleton] == &gltfNode)
    	{
        	// Armature.
        	if (!sceneFactory->getSceneRenderFactory()->prepareJointsUniformBuffer(sceneManager, node, (int32_t)skin.joints.size()))
        	{
                return VK_FALSE;
        	}
    	}

    	uint32_t newIndex = 0;

    	std::map<uint32_t, uint32_t> glToNewIndex;

    	for (uint32_t jointLoopIndex = 0; jointLoopIndex < skin.joints.size(); jointLoopIndex++)
    	{
    		glToNewIndex[skin.joints[jointLoopIndex]] = newIndex;

    		newIndex++;
    	}

    	for (uint32_t jointLoopIndex = 0; jointLoopIndex < skin.joints.size(); jointLoopIndex++)
    	{
    		uint32_t jointIndex = skin.joints[jointLoopIndex];

        	if (&visitor.getAllGltfNodes()[jointIndex] == &gltfNode)
        	{
            	// Bone.
        		node->setJointIndex((int32_t)glToNewIndex[jointIndex]);

        		if (skin.inverseBindMatrices != nullptr)
        		{
        			const float* m = visitor.getFloatPointer(*skin.inverseBindMatrices, (uint32_t)node->getJointIndex());

        			if (m != nullptr)
					{

						glm::mat4 inverseBindMatrix(1.0f);

						for (uint32_t mi = 0; mi < 16; mi++)
						{
							inverseBindMatrix[mi / 4][mi % 4] = m[mi];
						}

						node->setInverseBindMatrix(inverseBindMatrix);
					}
        		}
        	}
    	}
    }

    //

    for (uint32_t i = 0; i < gltfNode.children.size(); i++)
    {
        auto childNode = sceneFactory->createNode(sceneManager);

        if (!childNode.get())
        {
            return VK_FALSE;
        }

        const auto& gltfChildNode = visitor.getAllGltfNodes()[gltfNode.children[i]];

        childNode->setName(gltfChildNode.name);

        childNode->setParentNode(node);

        //

        node->addChildNode(childNode);

        //

        if (!gltfProcessNode(childNode, visitor, gltfChildNode, sceneManager, sceneFactory))
        {
        	return VK_FALSE;
        }
    }

    node->sortChildNodes();

    //
    // Process animations.
    //

    for (uint32_t animationIndex = 0; animationIndex < visitor.getAllGltfAnimations().size(); animationIndex++)
    {
    	const auto& gltfAnimation = visitor.getAllGltfAnimations()[animationIndex];

    	for (uint32_t channelIndex = 0; channelIndex < gltfAnimation.channels.size(); channelIndex++)
    	{
    		if (gltfAnimation.channels[channelIndex].targetNode->name == gltfNode.name)
    		{
    			//
    			// Only create animation, when really needed.
    			//

    			IAnimationSP animation;

    	    	if (node->getNumberAnimations() == 0)
    	    	{
    	            animation = sceneFactory->createAnimation(sceneManager);

    	            if (!animation.get())
    	            {
    	                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation not created: '%s'", visitor.getAllGltfAnimations()[animationIndex].name.c_str(), VKTS_MAX_TOKEN_CHARS);

    	                return VK_FALSE;
    	            }

    	            std::string animationName = gltfAnimation.name + "_" + gltfNode.name;

    	            animation->setName(animationName);

    	            sceneManager->addAnimation(animation);

    	            //

    	            node->addAnimation(sceneManager->useAnimation(animation->getName()));
    	    	}

    	    	animation = node->getAnimations()[0];

    			//
    	    	//
    	    	//

    			const auto& gltfChannel = gltfAnimation.channels[channelIndex];

    			if (!gltfChannel.sampler)
    			{
    				return VK_FALSE;
    			}

                //

                auto* timeAccessor = gltfChannel.sampler->input;
                auto* targetAccessor = gltfChannel.sampler->output;

                if (!timeAccessor || !targetAccessor)
                {
                	return VK_FALSE;
                }

                if (timeAccessor->min.size() == 0 || timeAccessor->max.size() == 0)
                {
                	return VK_FALSE;
                }

				if (timeAccessor->count != targetAccessor->count)
				{
                	return VK_FALSE;
				}

				if (!visitor.isFloat(timeAccessor->componentType) || !visitor.isFloat(targetAccessor->componentType))
				{
                	return VK_FALSE;
				}

                //

                if (1 != visitor.getComponentsPerType(timeAccessor->type))
                {
                	return VK_FALSE;
                }


                VkTsTargetTransform targetTransform;

                uint32_t targetTransformElementCount = 0;

                if (gltfChannel.targetPath == "translation")
                {
                	targetTransform = VKTS_TARGET_TRANSFORM_TRANSLATE;

                	targetTransformElementCount = 3;
                }
                else if (gltfChannel.targetPath == "rotation")
                {
                	targetTransform = VKTS_TARGET_TRANSFORM_ROTATE;

                	targetTransformElementCount = 4;
                }
                else if (gltfChannel.targetPath == "scale")
                {
                	targetTransform = VKTS_TARGET_TRANSFORM_SCALE;

                	targetTransformElementCount = 3;
                }
                else
                {
                	return VK_FALSE;
                }

                if (targetTransformElementCount != visitor.getComponentsPerType(targetAccessor->type))
                {
                	return VK_FALSE;
                }

                //

                VkTsInterpolator interpolator;

                uint32_t entryIndexMultiply = 1;
                uint32_t targetTransformElementIndexMultiply = 0;

                if (gltfChannel.sampler->interpolation == "STEP")
                {
                	interpolator = VKTS_INTERPOLATOR_CONSTANT;
                }
                else if (gltfChannel.sampler->interpolation == "LINEAR")
                {
                	interpolator = VKTS_INTERPOLATOR_LINEAR;
                }
                else if (gltfChannel.sampler->interpolation == "CATMULLROMSPLINE")
                {
                	interpolator = VKTS_INTERPOLATOR_CATMULLROMSPLINE;
                }
                else if (gltfChannel.sampler->interpolation == "CUBICSPLINE")
                {
                	interpolator = VKTS_INTERPOLATOR_CUBICSPLINE;

                	entryIndexMultiply = 3;
                	targetTransformElementIndexMultiply = 1;
                }
                else
                {
                	return VK_FALSE;
                }

                //

                animation->setStart(glm::min(animation->getStart(), timeAccessor->min[0]));
                animation->setStop(glm::max(animation->getStop(), timeAccessor->max[0]));

    			//
    			// Process channel.
    			//

                for (uint32_t targetTransformElementIndex = 0; targetTransformElementIndex < targetTransformElementCount; targetTransformElementIndex++)
                {
                	IChannelSP channel;

                	for (uint32_t channelIndex = 0; channelIndex < animation->getNumberChannels(); channelIndex++)
                	{
                		if (animation->getChannels()[channelIndex]->getTargetTransform() == targetTransform && animation->getChannels()[channelIndex]->getTargetTransformElement() == (VkTsTargetTransformElement)targetTransformElementIndex)
                		{
                			channel = animation->getChannels()[channelIndex];

                			break;
                		}
                	}

                	if (!channel.get())
                	{
						channel = sceneFactory->createChannel(sceneManager);

						std::string channelName = gltfAnimation.name + "_" + gltfNode.name + "_" + gltfChannel.name + "_" + gltfChannel.targetPath + "_" + std::to_string(targetTransformElementIndex);

						if (!channel.get())
						{
							logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Channel not created: '%s'", channelName.c_str(), VKTS_MAX_TOKEN_CHARS);

							return VK_FALSE;
						}

						channel->setName(channelName);

						channel->setTargetTransform(targetTransform);

						channel->setTargetTransformElement((VkTsTargetTransformElement)targetTransformElementIndex);

						//

						sceneManager->addChannel(channel);

						animation->addChannel(sceneManager->useChannel(channel->getName()));
                	}

					//



					for (uint32_t entryIndex = 0; entryIndex < gltfChannel.sampler->input->count; entryIndex++)
					{
						const float* key = visitor.getFloatPointer(*gltfChannel.sampler->input, entryIndex);
						const float* value = visitor.getFloatPointer(*gltfChannel.sampler->output, entryIndex * entryIndexMultiply);

						if (!key || !value)
						{
							return VK_FALSE;
						}

						channel->addEntry(*key, value[targetTransformElementIndex], glm::vec4(*key - 0.1f, value[targetTransformElementIndex + 1 * targetTransformElementIndexMultiply], *key + 0.1f, value[targetTransformElementIndex + 2 * targetTransformElementIndexMultiply]), interpolator);
					}
                }
    		}
    	}
    }

	return VK_TRUE;
}

static VkBool32 gltfProcessObject(IObjectSP& object, const GltfVisitor& visitor, const GltfScene& gltfScene, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	// Process root node.

    for (uint32_t i = 0; i < gltfScene.nodes.size(); i++)
    {
    	auto* gltfNode = gltfScene.nodes[i];

    	if (gltfNode->name == object->getName())
    	{
            auto node = sceneFactory->createNode(sceneManager);

            if (!node.get())
            {
                return VK_FALSE;
            }

            node->setName(gltfNode->name);

            //

            if (!gltfProcessNode(node, visitor, *gltfNode, sceneManager, sceneFactory))
            {
            	return VK_FALSE;
            }

            //

            object->setRootNode(node);
    	}
    }

	return VK_TRUE;
}


ISceneSP VKTS_APIENTRY gltfLoad(const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory, const VkBool32 freeHostMemory)
{
    if (!filename || !sceneManager.get() || !sceneFactory.get())
    {
        return ISceneSP();
    }

    //

    std::string lowerCaseFilename(filename);
    std::transform(lowerCaseFilename.begin(), lowerCaseFilename.end(), lowerCaseFilename.begin(), ::tolower);

    auto dotPos = lowerCaseFilename.rfind('.');
    if (dotPos == lowerCaseFilename.npos)
    {
        return ISceneSP();
    }

    std::string lowerCaseExtension = lowerCaseFilename.substr(dotPos);

    //

    std::string gltfString = "";
    IBinaryBufferSP binaryBuffer;

    if (lowerCaseExtension == ".gltf")
    {
    	auto textFile = fileLoadText(filename);

    	if (!textFile.get())
    	{
    		return ISceneSP();
    	}

    	gltfString = textFile->getString();
    }
    else if (lowerCaseExtension == ".glb")
    {
    	auto binaryFile = fileLoadBinary(filename);

    	if (!binaryFile.get())
    	{
    		return ISceneSP();
    	}

    	const uint8_t* data = binaryFile->getByteData();

    	if (!data)
    	{
    		return ISceneSP();
    	}

    	//
    	// Header.
    	//

    	if (binaryFile->getSize() < 12)
    	{
    		return ISceneSP();
    	}

    	// Magic
    	if (!(data[0] == 'g' && data[1] == 'l' && data[2] == 'T' && data[3] == 'F'))
    	{
    		return ISceneSP();
    	}

    	// Version
    	const uint32_t version = *(const uint32_t*)&data[4];
    	if (version != 2)
    	{
    		return ISceneSP();
    	}

    	// Length
    	const uint32_t length = *(const uint32_t*)&data[8];
    	if (length != binaryFile->getSize())
    	{
    		return ISceneSP();
    	}

    	//
    	// Chunk 0.
    	//

    	if (binaryFile->getSize() < 12 + 8)
    	{
    		return ISceneSP();
    	}

    	// Chunk 0 length
    	const uint32_t chunk0Length = *(const uint32_t*)&data[12];
    	if (chunk0Length > length - 12 - 8)
    	{
    		return ISceneSP();
    	}

    	// Chunk 0 type
    	const uint32_t chunk0Type = *(const uint32_t*)&data[12 + 4];
    	if (chunk0Type != 0x4E4F534A)
    	{
    		return ISceneSP();
    	}

    	// Chunk 0 data
    	gltfString = std::string((char*)&data[12 + 8], 0, chunk0Length);

    	if (length > chunk0Length + 12 + 8)
    	{
			//
			// Chunk 1.
			//

			// Chunk 1 length
			const uint32_t chunk1Length = *(const uint32_t*)&data[12 + 8 + chunk0Length];
	    	if (chunk1Length > length - 12 - 8 - 8 - chunk0Length)
	    	{
	    		return ISceneSP();
	    	}

			// Chunk 1 type
			const uint32_t chunk1Type = *(const uint32_t*)&data[12 + 8 + chunk0Length + 4];
			if (chunk1Type != 0x004E4942)
			{
				return ISceneSP();
			}

	    	// Chunk 1 data
			binaryBuffer = binaryBufferCreate(&data[12 + 8 + chunk0Length + 8], chunk1Length);
    	}
    }
    else
    {
    	return ISceneSP();
    }

    //

	auto json = jsonDecode(gltfString);

	if (!json.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Parsing JSON failed");

		return ISceneSP();
	}

    char directory[VKTS_MAX_BUFFER_CHARS] = "";

    fileGetDirectory(directory, filename);

	GltfVisitor visitor(directory, binaryBuffer);

	json->visit(visitor);

	if (visitor.getState() != GltfState_End)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Processing glTF failed");

		return ISceneSP();
	}

	logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Processing glTF succeeded");

	//
	// Scene.
	//

	const GltfScene* gltfScene = visitor.getDefaultScene();

	if (!gltfScene && visitor.getAllGltfScenes().size() > 0)
	{
		gltfScene = &(visitor.getAllGltfScenes()[0]);
	}

	if (!gltfScene)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No glTF scene found");

		return ISceneSP();
	}

    auto scene = sceneFactory->createScene(sceneManager);

    if (!scene.get())
    {
        return ISceneSP();
    }

    scene->setName(gltfScene->name);

    //
    // Objects: Create out of every root node an object, that it can be moved around independently.
    //

    for (uint32_t i = 0; i < gltfScene->nodes.size(); i++)
    {
    	auto* gltfNode = gltfScene->nodes[i];

    	//

        auto object = sceneFactory->createObject(sceneManager);

        if (!object.get())
        {
        	return ISceneSP();
        }

        object->setName(gltfNode->name);

        sceneManager->addObject(object);

        //

        if (!gltfProcessObject(object, visitor, *gltfScene, sceneManager, sceneFactory))
        {
        	return ISceneSP();
        }

        //

        auto currentObject = sceneManager->useObject(gltfNode->name);

        if (!currentObject.get())
        {
            return ISceneSP();
        }

        scene->addObject(currentObject);
    }

    //
    // Free host memory if wanted.
    //
    if (freeHostMemory)
    {
        for (uint32_t i = 0; i < sceneManager->getAllImageDatas().values().size(); i++)
        {
        	sceneManager->getAllImageDatas().values()[i]->freeHostMemory();
        }

        for (uint32_t i = 0; i < sceneManager->getAllSubMeshes().values().size(); i++)
        {
        	sceneManager->getAllSubMeshes().values()[i]->freeHostMemory();
        }
    }

    return scene;
}

}
