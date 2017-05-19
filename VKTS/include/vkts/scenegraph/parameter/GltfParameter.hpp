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

#ifndef VKTS_GLTFPARAMETER_HPP_
#define VKTS_GLTFPARAMETER_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class GltfParameter : public Parameter
{

private:

	std::vector<uint8_t> binary;

	JSONobjectSP glTF;

	JSONarraySP scenes_nodes;
	int32_t rootNodeCounter;

	JSONarraySP nodes;
	SmartPointerMap<std::string, JSONarraySP> nodeNameToChildren;
	int32_t nodeCounter;

	JSONarraySP meshes;

	JSONarraySP primitives;

	JSONarraySP materials;
	SmartPointerMap<std::string, JSONobjectSP> materialNameToMaterial;

	JSONarraySP images;
	Vector<std::string> storedImages;
	SmartPointerMap<std::string, IImageDataSP> storedImagesMap;

	JSONarraySP textures;
	JSONarraySP samplers;

	JSONarraySP accessors;
	Vector<std::string> storedAccessors;

	JSONarraySP bufferViews;

	JSONarraySP buffers;

	JSONarraySP animations;

	void writeBinaryBuffer(const IBinaryBufferSP& binaryBuffer, const uint32_t count, const uint8_t elementCount, const std::string& currentType, const int32_t currentComponentType, const int32_t target, const uint32_t offset, const uint32_t stride)
	{
		//
		// Create buffer view.
		//

    	auto currentBufferView = JSONobjectSP(new JSONobject());

    	if (!currentBufferView.get())
    	{
    		return;
    	}

    	bufferViews->addValue(currentBufferView);

    	//

    	auto bufferValue = JSONintegerSP(new JSONinteger(0));

    	if (!bufferValue.get())
    	{
    		return;
    	}

    	currentBufferView->addKeyValue("buffer", bufferValue);

    	//

    	uint32_t byteOffset = (uint32_t)binary.size();

    	auto byteOffsetValue = JSONintegerSP(new JSONinteger((int32_t)byteOffset));

    	if (!byteOffsetValue.get())
    	{
    		return;
    	}

    	currentBufferView->addKeyValue("byteOffset", byteOffsetValue);

    	//

    	if (target != 0)
    	{
			auto targetValue = JSONintegerSP(new JSONinteger(target));

			if (!targetValue.get())
			{
				return;
			}

			currentBufferView->addKeyValue("target", targetValue);
    	}

    	//

    	glm::uvec4 integerMin;
    	glm::uvec4 integerMax;

    	glm::vec4 floatMin;
    	glm::vec4 floatMax;

    	// We write unsigned short, but fallback to .
    	uint32_t byteSize = 0;

    	if (currentComponentType == 5126)
    	{
    		byteSize = sizeof(float);
    	}
    	else if (currentComponentType == 5125)
    	{
    		byteSize = sizeof(uint32_t);
    	}
    	else if (currentComponentType == 5123)
    	{
    		byteSize = sizeof(uint16_t);
    	}
    	else
    	{
    		return;
    	}

    	//

    	uint32_t byteLength = count * elementCount * byteSize;

    	auto byteLengthValue = JSONintegerSP(new JSONinteger((int32_t)byteLength));

    	if (!byteLengthValue.get())
    	{
    		return;
    	}

    	currentBufferView->addKeyValue("byteLength", byteLengthValue);

    	//

    	for (uint32_t i = 0; i < count; i++)
    	{
        	const uint8_t* data = &binaryBuffer->getByteData()[offset + i * stride];
        	const float* floatData = (const float*)data;
        	const int32_t* integerData = (const int32_t*)data;

        	for (uint32_t k = 0; k < elementCount; k++)
        	{
				float currentFloat = floatData[k];
				uint32_t currentInteger = (uint32_t)integerData[k];
				uint16_t currentShort = (uint16_t)integerData[k];

				uint8_t* currentData = nullptr;

				if (currentComponentType == 5126)
				{
					currentData = (uint8_t*)&currentFloat;

					if (i == 0 || currentFloat < floatMin[k])
					{
						floatMin[k] = currentFloat;
					}
					if (i == 0 || currentFloat > floatMax[k])
					{
						floatMax[k] = currentFloat;
					}
				}
				else if (currentComponentType == 5125)
				{
					currentData = (uint8_t*)&currentInteger;

					if (i == 0 || currentInteger < integerMin[k])
					{
						integerMin[k] = currentInteger;
					}
					if (i == 0 || currentInteger > integerMax[k])
					{
						integerMax[k] = currentInteger;
					}
				}
				else if (currentComponentType == 5123)
				{
					currentData = (uint8_t*)&currentShort;

					if (i == 0 || (uint32_t)currentShort < integerMin[k])
					{
						integerMin[k] = (uint32_t)currentShort;
					}
					if (i == 0 || (uint32_t)currentShort > integerMax[k])
					{
						integerMax[k] = (uint32_t)currentShort;
					}
				}

				for (uint32_t m = 0; m < byteSize; m++)
				{
					binary.push_back(currentData[m]);
				}
        	}
    	}

    	//
    	// Buffer will be written, when everything is saved.
    	//

		//
		// Create accessor.
		//

    	auto currentAccessor = JSONobjectSP(new JSONobject());

    	if (!currentAccessor.get())
    	{
    		return;
    	}

    	accessors->addValue(currentAccessor);

    	//

    	byteOffsetValue = JSONintegerSP(new JSONinteger(0));

    	if (!byteOffsetValue.get())
    	{
    		return;
    	}

    	currentAccessor->addKeyValue("byteOffset", byteOffsetValue);

    	//

    	auto bufferViewValue = JSONintegerSP(new JSONinteger((int32_t)(bufferViews->size() - 1)));

    	if (!bufferViewValue.get())
    	{
    		return;
    	}

    	currentAccessor->addKeyValue("bufferView", bufferViewValue);

    	//

    	auto minValue = JSONarraySP(new JSONarray());
    	auto maxValue = JSONarraySP(new JSONarray());

    	if (!minValue.get() || !maxValue.get())
    	{
    		return;
    	}

    	currentAccessor->addKeyValue("min", minValue);
    	currentAccessor->addKeyValue("max", maxValue);

    	for (uint32_t i = 0; i < elementCount; i++)
    	{
    		JSONvalueSP currentMin;
    		JSONvalueSP currentMax;

    		if (currentComponentType == 5126)
    		{
    			currentMin = JSONfloatSP(new JSONfloat(floatMin[i]));
    			currentMax = JSONfloatSP(new JSONfloat(floatMax[i]));
    		}
    		else
    		{
    			currentMin = JSONintegerSP(new JSONinteger(integerMin[i]));
    			currentMax = JSONintegerSP(new JSONinteger(integerMax[i]));
    		}

    		if (!currentMin.get() || !currentMax.get())
    		{
    			return;
    		}

    		minValue->addValue(currentMin);
    		maxValue->addValue(currentMax);
    	}

    	//

    	auto countValue = JSONintegerSP(new JSONinteger((int32_t)count));

    	if (!countValue.get())
    	{
    		return;
    	}

    	currentAccessor->addKeyValue("count", countValue);

    	//

    	auto typeValue = JSONstringSP(new JSONstring(currentType));

    	if (!typeValue.get())
    	{
    		return;
    	}

    	currentAccessor->addKeyValue("type", typeValue);

    	//

    	auto componentType = JSONintegerSP(new JSONinteger(currentComponentType));

    	if (!componentType.get())
    	{
    		return;
    	}

    	currentAccessor->addKeyValue("componentType", componentType);
	}

    VkBool32 addAnimation(JSONarraySP& channels, JSONarraySP& samplers, const char* path, const VkTsInterpolator interpolate)
    {
		auto channel = JSONobjectSP(new JSONobject());
		auto sampler = JSONobjectSP(new JSONobject());

    	if (!channel.get() || !sampler.get())
    	{
    		return VK_FALSE;
    	}

    	//

    	auto samplerIndex = JSONintegerSP(new JSONinteger((int32_t)samplers->size()));
    	auto targetObject = JSONobjectSP(new JSONobject());

    	auto nodeIndex = JSONintegerSP(new JSONinteger(nodeCounter));
    	auto targetPath = JSONstringSP(new JSONstring(path));

    	if (!samplerIndex.get() || !targetObject.get() || !nodeIndex.get() || !targetPath.get())
    	{
    		return VK_FALSE;
    	}

    	targetObject->addKeyValue("node", nodeIndex);
    	targetObject->addKeyValue("path", targetPath);

    	//

    	channel->addKeyValue("sampler", samplerIndex);
    	channel->addKeyValue("target", targetObject);

    	//

    	std::string interpolationString = interpolate == VKTS_INTERPOLATOR_LINEAR ? "LINEAR" : "STEP";

    	auto inputIndex = JSONintegerSP(new JSONinteger((int32_t)accessors->size()));

    	auto interpolation = JSONstringSP(new JSONstring(interpolationString));

    	auto outputIndex = JSONintegerSP(new JSONinteger((int32_t)accessors->size() + 1));

    	if (!inputIndex.get() || !interpolation.get() || !outputIndex.get())
    	{
    		return VK_FALSE;
    	}

    	sampler->addKeyValue("input", inputIndex);
    	sampler->addKeyValue("interpolation", interpolation);
    	sampler->addKeyValue("output", outputIndex);

    	//

    	channels->addValue(channel);
    	samplers->addValue(sampler);

    	return VK_TRUE;
    }

public:

	GltfParameter() :
		Parameter(), binary(), glTF(), scenes_nodes(), rootNodeCounter(0), nodes(), nodeCounter(0), meshes(), primitives(), materials(),  materialNameToMaterial(), images(), storedImages(), textures(), accessors(), storedAccessors(), bufferViews(), buffers(), animations()
    {
    }

    virtual ~GltfParameter()
    {
    }

	//

	VkBool32 save(const char* filename, const char* directory = nullptr)
	{
		if (!glTF.get())
		{
			return VK_FALSE;
		}

		char baseName[VKTS_MAX_BUFFER_CHARS];

		if (!fileGetBasename(baseName, filename))
		{
			return VK_FALSE;
		}

		//

		if (binary.size() > 0)
		{
	    	auto currentBuffer = JSONobjectSP(new JSONobject());

	    	if (!currentBuffer.get())
	    	{
	    		return VK_FALSE;
	    	}

	    	buffers->addValue(currentBuffer);

	    	//

	    	auto byteLengthValue = JSONintegerSP(new JSONinteger((int32_t)binary.size()));

	    	if (!byteLengthValue.get())
	    	{
	    		return VK_FALSE;
	    	}

	    	currentBuffer->addKeyValue("byteLength", byteLengthValue);

	    	//

	    	auto uriValue = JSONstringSP(new JSONstring(std::string(baseName) + ".bin"));

	    	if (!uriValue.get())
	    	{
	    		return VK_FALSE;
	    	}

	    	currentBuffer->addKeyValue("uri", uriValue);
		}

		//

		auto gltfEncoded = jsonEncode(glTF);

		if (gltfEncoded == "")
		{
			return VK_FALSE;
		}

		//

		std::string currentDirectory = "";

		if (directory && strlen(directory) > 0)
		{
			currentDirectory = std::string(directory);

			if (currentDirectory.back() != '/' && currentDirectory.back() != '\\')
			{
				currentDirectory += "/";
			}
		}

		//

		if (!fileSaveText((currentDirectory + filename).c_str(), textBufferCreate(gltfEncoded.c_str())))
		{
			return VK_FALSE;
		}

		for (uint32_t i = 0; i < storedImagesMap.size(); i++)
		{
			if (!imageDataSave((currentDirectory + storedImagesMap.keyAt(i)).c_str(), storedImagesMap.valueAt(i), 0, 0))
			{
				return VK_FALSE;
			}
		}

		//

		if (binary.size() > 0)
		{
			if (!fileSaveBinaryData((currentDirectory + baseName + ".bin").c_str(), (const void*)&binary[0], (uint32_t)binary.size()))
			{
				return VK_FALSE;
			}
		}

		return VK_TRUE;
	}

    //

    virtual void visit(IScene& scene)
    {
    	glTF = JSONobjectSP(new JSONobject());

    	if (!glTF.get())
    	{
    		return;
    	}

    	//
    	// asset
    	//

    	auto assetValue = JSONobjectSP(new JSONobject());

    	auto generatorValue = JSONstringSP(new JSONstring("VKTS glTF 2.0 exporter"));

    	auto versionValue = JSONstringSP(new JSONstring("2.0"));

    	if (!assetValue.get() || !generatorValue.get() || !versionValue.get())
    	{
    		return;
    	}

    	assetValue->addKeyValue("generator", generatorValue);
    	assetValue->addKeyValue("version", versionValue);

    	//

    	glTF->addKeyValue("asset", assetValue);

    	//
    	// scene
    	//

    	auto sceneValue = JSONintegerSP(new JSONinteger(0));

    	if (!sceneValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("scene", sceneValue);

    	//
    	// scenes
    	//

    	auto scenesValue = JSONarraySP(new JSONarray());

    	auto defaultScene = JSONobjectSP(new JSONobject());

    	auto scene_nodes = JSONarraySP(new JSONarray());

    	if (!scenesValue.get() || !defaultScene.get() || !scene_nodes.get())
    	{
    		return;
    	}

    	scenesValue->addValue(defaultScene);

    	defaultScene->addKeyValue("nodes", scene_nodes);

    	//

    	glTF->addKeyValue("scenes", scenesValue);

    	//

    	this->scenes_nodes = scene_nodes;

    	//
    	// nodes
    	//

    	auto nodesValue = JSONarraySP(new JSONarray());

    	if (!nodesValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("nodes", nodesValue);

    	//

    	this->nodes = nodesValue;

    	//
    	// meshes
    	//

    	auto meshesValue = JSONarraySP(new JSONarray());

    	if (!meshesValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("meshes", meshesValue);

    	//

    	this->meshes = meshesValue;

    	//
    	// images
    	//

    	auto imagesValue = JSONarraySP(new JSONarray());

    	if (!imagesValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("images", imagesValue);

    	//

    	this->images = imagesValue;

    	//
    	// textures
    	//

    	auto texturesValue = JSONarraySP(new JSONarray());

    	if (!texturesValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("textures", texturesValue);

    	//

    	this->textures = texturesValue;

    	//
    	// samplers
    	//

    	auto samplersValue = JSONarraySP(new JSONarray());

    	if (!samplersValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("samplers", samplersValue);

    	//

    	this->samplers = samplersValue;

    	//
    	// materials
    	//

    	auto materialsValue = JSONarraySP(new JSONarray());

    	if (!materialsValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("materials", materialsValue);

    	//

    	this->materials = materialsValue;

    	//
    	// accessors
    	//

    	auto accessorsValue = JSONarraySP(new JSONarray());

    	if (!accessorsValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("accessors", accessorsValue);

    	//

    	this->accessors = accessorsValue;

    	//
    	// bufferViews
    	//

    	auto bufferViewsValue = JSONarraySP(new JSONarray());

    	if (!bufferViewsValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("bufferViews", bufferViewsValue);

    	//

    	this->bufferViews = bufferViewsValue;

    	//
    	// buffers
    	//

    	auto buffersValue = JSONarraySP(new JSONarray());

    	if (!buffersValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("buffers", buffersValue);

    	//

    	this->buffers = buffersValue;

    	//
    	// animations
    	//

    	auto animationsValue = JSONarraySP(new JSONarray());

    	if (!animationsValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("animations", animationsValue);

    	//

    	this->animations = animationsValue;
    }

    virtual void visit(IObject& object)
    {
    	if (!scenes_nodes.get())
    	{
    		return;
    	}

    	//

    	auto rootNodeIndex = JSONintegerSP(new JSONinteger(rootNodeCounter));

    	if (!rootNodeIndex.get())
    	{
    		return;
    	}

    	//

    	scenes_nodes->addValue(rootNodeIndex);

    	//

    	rootNodeCounter++;
    }

    virtual void visit(INode& node)
    {
    	if (!nodes.get())
    	{
    		return;
    	}

    	//

    	auto currentNode = JSONobjectSP(new JSONobject());

    	if (!currentNode.get())
    	{
    		return;
    	}

    	nodes->addValue(currentNode);

    	//
    	//

    	auto childrenValue = JSONarraySP(new JSONarray());

    	if (!childrenValue.get())
    	{
    		return;
    	}

    	currentNode->addKeyValue("children", childrenValue);

    	nodeNameToChildren[node.getName()] = childrenValue;

    	//

		if (node.getMeshes().size() > 1)
		{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Node has %u meshes!", node.getMeshes().size());

			return;
		}

    	if (node.getMeshes().size() == 1)
    	{
        	auto meshValue = JSONintegerSP(new JSONinteger((int32_t)meshes->size()));

        	if (!meshValue.get())
        	{
        		return;
        	}

        	currentNode->addKeyValue("mesh", meshValue);
    	}

    	//

    	if (node.getRotate().x != 0.0f || node.getRotate().y != 0.0f || node.getRotate().z != 0.0f || node.getRotate().w != 1.0f)
    	{
    		Quat rotation = node.getRotate();

    		//

        	auto rotationValue = JSONarraySP(new JSONarray());

        	auto rotationX = JSONfloatSP(new JSONfloat(rotation.x));
        	auto rotationY = JSONfloatSP(new JSONfloat(rotation.y));
        	auto rotationZ = JSONfloatSP(new JSONfloat(rotation.z));
        	auto rotationW = JSONfloatSP(new JSONfloat(rotation.w));

        	if (!rotationValue.get() || !rotationX.get() || !rotationY.get() || !rotationZ.get() || !rotationW.get())
        	{
        		return;
        	}

        	rotationValue->addValue(rotationX);
        	rotationValue->addValue(rotationY);
        	rotationValue->addValue(rotationZ);
        	rotationValue->addValue(rotationW);

        	//

        	currentNode->addKeyValue("rotation", rotationValue);
    	}

    	if (node.getScale().x != 1.0f || node.getScale().y != 1.0f || node.getScale().z != 1.0f)
    	{
        	auto scaleValue = JSONarraySP(new JSONarray());

        	auto scaleX = JSONfloatSP(new JSONfloat(node.getScale().x));
        	auto scaleY = JSONfloatSP(new JSONfloat(node.getScale().y));
        	auto scaleZ = JSONfloatSP(new JSONfloat(node.getScale().z));

        	if (!scaleValue.get() || !scaleX.get() || !scaleY.get() || !scaleZ.get())
        	{
        		return;
        	}

        	scaleValue->addValue(scaleX);
        	scaleValue->addValue(scaleY);
        	scaleValue->addValue(scaleZ);

        	//

        	currentNode->addKeyValue("scale", scaleValue);
    	}

    	if (node.getTranslate().x != 0.0f || node.getTranslate().y != 0.0f || node.getTranslate().z != 0.0f)
    	{
        	auto translationValue = JSONarraySP(new JSONarray());

        	auto translationX = JSONfloatSP(new JSONfloat(node.getTranslate().x));
        	auto translationY = JSONfloatSP(new JSONfloat(node.getTranslate().y));
        	auto translationZ = JSONfloatSP(new JSONfloat(node.getTranslate().z));

        	if (!translationValue.get() || !translationX.get() || !translationY.get() || !translationZ.get())
        	{
        		return;
        	}

        	translationValue->addValue(translationX);
        	translationValue->addValue(translationY);
        	translationValue->addValue(translationZ);

        	//

        	currentNode->addKeyValue("translation", translationValue);
    	}

    	//

    	auto nameValue = JSONstringSP(new JSONstring(node.getName()));

    	if (!nameValue.get())
    	{
    		return;
    	}

    	currentNode->addKeyValue("name", nameValue);

    	//
    	// Process animation
    	//

    	if (node.getAnimations().size() > 0)
    	{
    		if (node.getAnimations().size() > 1)
    		{
				logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Node '%s' has '%u' animations. Storing only first one.", node.getName().c_str(), node.getAnimations().size());
    		}

    		auto& currentAnimation = node.getAnimations()[0];

    		SmartPointerVector<IChannelSP> allTranslates;
    		SmartPointerVector<IChannelSP> allScales;
    		SmartPointerVector<IChannelSP> allEulerRotations;
    		SmartPointerVector<IChannelSP> allRotations;

    		// Merging channels, targeting the same transform.
    		for (uint32_t channelIndex = 0; channelIndex < currentAnimation->getNumberChannels(); channelIndex++)
    		{
    			auto currentChannel = currentAnimation->getChannels()[channelIndex];

    			switch (currentChannel->getTargetTransform())
    			{
					case VKTS_TARGET_TRANSFORM_TRANSLATE:
						allTranslates.append(currentChannel);
						break;
					case VKTS_TARGET_TRANSFORM_EULER_ROTATE:
						allEulerRotations.append(currentChannel);
						break;
					case VKTS_TARGET_TRANSFORM_ROTATE:
						allRotations.append(currentChannel);
						break;
					case VKTS_TARGET_TRANSFORM_SCALE:
						allScales.append(currentChannel);
						break;
    			}
    		}

    		// Invalid combination, either Euler or Quaternion is valid.
    		if (allEulerRotations.size() > 0 && allRotations.size() > 0)
    		{
    			return;
    		}

    		//
    		// Merge all translations.
    		//

    		std::map<float, glm::vec3> translateValues;
    		VkTsInterpolator translateInterpolate;

    		for (uint32_t i = 0; i < allTranslates.size(); i++)
    		{
				uint32_t elementIndex = 0;

				switch (allTranslates[i]->getTargetTransformElement())
				{
					case VKTS_TARGET_TRANSFORM_ELEMENT_X:
						elementIndex = 0;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Y:
						elementIndex = 1;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Z:
						elementIndex = 2;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_W:
						return;
				}

				for (uint32_t k = 0; k < allTranslates[i]->getNumberEntries(); k++)
    			{
    				if (allTranslates[i]->getInterpolators()[k] == VKTS_INTERPOLATOR_BEZIER)
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has Bezier animation.");
    					return;
    				}

    				if (k == 0)
    				{
    					translateInterpolate = allTranslates[i]->getInterpolators()[k];
    				}
    				else if (translateInterpolate != allTranslates[i]->getInterpolators()[k])
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has mixed animation.");
    					return;
    				}

    				float key = allTranslates[i]->getKeys()[k];
    				float value = allTranslates[i]->getValues()[k];

    				if (translateValues.find(key) == translateValues.end())
    				{
    					translateValues[key] = glm::vec3(0.0f, 0.0f, 0.0f);
    				}

    				translateValues[key][elementIndex] = value;
    			}
    		}

    		//
    		// Merge all scales.
    		//

    		std::map<float, glm::vec3> scaleValues;
    		VkTsInterpolator scaleInterpolate;

    		for (uint32_t i = 0; i < allScales.size(); i++)
    		{
				uint32_t elementIndex = 0;

				switch (allScales[i]->getTargetTransformElement())
				{
					case VKTS_TARGET_TRANSFORM_ELEMENT_X:
						elementIndex = 0;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Y:
						elementIndex = 1;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Z:
						elementIndex = 2;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_W:
						return;
				}

				for (uint32_t k = 0; k < allScales[i]->getNumberEntries(); k++)
    			{
    				if (allScales[i]->getInterpolators()[k] == VKTS_INTERPOLATOR_BEZIER)
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has Bezier animation.");
    					return;
    				}

    				if (k == 0)
    				{
    					scaleInterpolate = allScales[i]->getInterpolators()[k];
    				}
    				else if (scaleInterpolate != allScales[i]->getInterpolators()[k])
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has mixed animation.");
    					return;
    				}

    				float key = allScales[i]->getKeys()[k];
    				float value = allScales[i]->getValues()[k];

    				if (scaleValues.find(key) == scaleValues.end())
    				{
    					scaleValues[key] = glm::vec3(1.0f, 1.0f, 1.0f);
    				}

    				scaleValues[key][elementIndex] = value;
    			}
    		}

    		//
    		// Merge all rotations.
    		//

    		std::map<float, glm::vec3> rotationValues;
    		VkTsInterpolator rotationInterpolate;

    		for (uint32_t i = 0; i < allEulerRotations.size(); i++)
    		{
				uint32_t elementIndex = 0;

				switch (allEulerRotations[i]->getTargetTransformElement())
				{
					case VKTS_TARGET_TRANSFORM_ELEMENT_X:
						elementIndex = 0;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Y:
						elementIndex = 1;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Z:
						elementIndex = 2;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_W:
						return;
				}

				for (uint32_t k = 0; k < allEulerRotations[i]->getNumberEntries(); k++)
    			{
    				if (allEulerRotations[i]->getInterpolators()[k] == VKTS_INTERPOLATOR_BEZIER)
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has Bezier animation.");
    					return;
    				}

    				if (k == 0)
    				{
    					rotationInterpolate = allEulerRotations[i]->getInterpolators()[k];
    				}
    				else if (rotationInterpolate != allEulerRotations[i]->getInterpolators()[k])
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has mixed animation.");
    					return;
    				}

    				float key = allEulerRotations[i]->getKeys()[k];
    				float value = allEulerRotations[i]->getValues()[k];

    				if (rotationValues.find(key) == rotationValues.end())
    				{
    					rotationValues[key] = glm::vec3(0.0f, 0.0f, 0.0f);
    				}

    				rotationValues[key][elementIndex] = value;
    			}
    		}

    		//
    		// Merge all quaternion rotations.
    		//

    		std::map<float, glm::vec4> quaternionValues;
    		VkTsInterpolator quaternionInterpolate;

    		for (uint32_t i = 0; i < allRotations.size(); i++)
    		{
				uint32_t elementIndex = 0;

				switch (allRotations[i]->getTargetTransformElement())
				{
					case VKTS_TARGET_TRANSFORM_ELEMENT_X:
						elementIndex = 0;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Y:
						elementIndex = 1;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_Z:
						elementIndex = 2;
						break;
					case VKTS_TARGET_TRANSFORM_ELEMENT_W:
						elementIndex = 3;
						break;
				}

				for (uint32_t k = 0; k < allRotations[i]->getNumberEntries(); k++)
    			{
    				if (allRotations[i]->getInterpolators()[k] == VKTS_INTERPOLATOR_BEZIER)
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has Bezier animation.");
    					return;
    				}

    				if (k == 0)
    				{
    					quaternionInterpolate = allRotations[i]->getInterpolators()[k];
    				}
    				else if (quaternionInterpolate != allRotations[i]->getInterpolators()[k])
    				{
    					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation has mixed animation.");
    					return;
    				}

    				float key = allRotations[i]->getKeys()[k];
    				float value = allRotations[i]->getValues()[k];

    				if (quaternionValues.find(key) == quaternionValues.end())
    				{
    					quaternionValues[key] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    				}

    				quaternionValues[key][elementIndex] = value;
    			}
    		}

    		//
    		// Convert rotations to quaternion rotations.
    		//

    		if (rotationValues.size() > 0)
    		{
	        	for (auto it :  rotationValues)
    			{
	        		Quat q = it.second;

    	        	quaternionValues[it.first] = glm::vec4(q.x, q.y, q.z, q.w);
    			}

	        	rotationValues.clear();

	        	//

	        	quaternionInterpolate = rotationInterpolate;
    		}

    		//
    		// Create animation
    		//

        	auto animation = JSONobjectSP(new JSONobject());

        	auto channels = JSONarraySP(new JSONarray());
        	auto samplers = JSONarraySP(new JSONarray());
        	auto name = JSONstringSP(new JSONstring(currentAnimation->getName()));

        	if (!animation.get() || !channels.get() || !samplers.get() || !name.get())
        	{
        		return;
        	}

        	animation->addKeyValue("channels", channels);
        	animation->addKeyValue("samplers", samplers);
        	animation->addKeyValue("name", name);

        	//

        	animations->addValue(animation);

        	//

        	if (translateValues.size() > 0)
        	{
        		if (!addAnimation(channels, samplers, "translation", translateInterpolate))
        		{
        			return;
        		}

        		//

        		IBinaryBufferSP keys = binaryBufferCreate((uint32_t)translateValues.size() * sizeof(float));
        		IBinaryBufferSP values = binaryBufferCreate((uint32_t)translateValues.size() * sizeof(float) * 3);

        		for (auto it : translateValues)
        		{
        			keys->write(&it.first, sizeof(float), 1);
        			values->write(glm::value_ptr(it.second), sizeof(float), 3);
        		}

        		writeBinaryBuffer(keys, (uint32_t)translateValues.size(), 1, "SCALAR", 5126, 0, 0, sizeof(float));
        		storedAccessors.append(currentAnimation->getName() + "_translation_keys");
        		writeBinaryBuffer(values, (uint32_t)translateValues.size(), 3, "VEC3", 5126, 0, 0, sizeof(float) * 3);
    			storedAccessors.append(currentAnimation->getName() + "_translation_values");
        	}

        	if (scaleValues.size() > 0)
        	{
        		if (!addAnimation(channels, samplers, "scale", scaleInterpolate))
        		{
        			return;
        		}

        		//

        		IBinaryBufferSP keys = binaryBufferCreate((uint32_t)scaleValues.size() * sizeof(float));
        		IBinaryBufferSP values = binaryBufferCreate((uint32_t)scaleValues.size() * sizeof(float) * 3);

        		for (auto it : scaleValues)
        		{
        			keys->write(&it.first, sizeof(float), 1);
        			values->write(glm::value_ptr(it.second), sizeof(float), 3);
        		}

        		writeBinaryBuffer(keys, (uint32_t)scaleValues.size(), 1, "SCALAR", 5126, 0, 0, sizeof(float));
        		storedAccessors.append(currentAnimation->getName() + "_scale_keys");
        		writeBinaryBuffer(values, (uint32_t)scaleValues.size(), 3, "VEC3", 5126, 0, 0, sizeof(float) * 3);
    			storedAccessors.append(currentAnimation->getName() + "_scale_values");
        	}

        	if (quaternionValues.size() > 0)
        	{
        		if (!addAnimation(channels, samplers, "rotation", quaternionInterpolate))
        		{
        			return;
        		}

        		//

        		IBinaryBufferSP keys = binaryBufferCreate((uint32_t)quaternionValues.size() * sizeof(float));
        		IBinaryBufferSP values = binaryBufferCreate((uint32_t)quaternionValues.size() * sizeof(float) * 4);

        		for (auto it : quaternionValues)
        		{
        			keys->write(&it.first, sizeof(float), 1);
        			values->write(glm::value_ptr(it.second), sizeof(float), 4);
        		}

        		writeBinaryBuffer(keys, (uint32_t)quaternionValues.size(), 1, "SCALAR", 5126, 0, 0, sizeof(float));
        		storedAccessors.append(currentAnimation->getName() + "_rotation_keys");
        		writeBinaryBuffer(values, (uint32_t)quaternionValues.size(), 4, "VEC4", 5126, 0, 0, sizeof(float) * 4);
    			storedAccessors.append(currentAnimation->getName() + "_rotation_values");
        	}
    	}

    	//
    	// Process children index.
    	//

    	if (node.getParentNode().get())
    	{
    		if (!nodeNameToChildren.contains(node.getParentNode()->getName()))
    		{
    			return;
    		}

    		auto parentChildren = nodeNameToChildren[node.getParentNode()->getName()];

    		//

        	auto nodeIndex = JSONintegerSP(new JSONinteger(nodeCounter));

        	if (!nodeIndex.get())
        	{
        		return;
        	}

        	parentChildren->addValue(nodeIndex);
    	}

    	//

    	nodeCounter++;
    }

    virtual void visit(IMesh& mesh)
    {
    	if (!meshes.get() || mesh.getNumberSubMeshes() == 0)
    	{
    		return;
    	}

    	auto currentMesh = JSONobjectSP(new JSONobject());

    	if (!currentMesh.get())
    	{
    		return;
    	}

    	meshes->addValue(currentMesh);

    	//

    	auto primitivesValue = JSONarraySP(new JSONarray());

    	if (!primitivesValue.get())
    	{
    		return;
    	}

    	currentMesh->addKeyValue("primitives", primitivesValue);

    	this->primitives = primitivesValue;

    	//

    	auto nameValue = JSONstringSP(new JSONstring(mesh.getName()));

    	if (!nameValue.get())
    	{
    		return;
    	}

    	currentMesh->addKeyValue("name", nameValue);
    }

    virtual void visit(ISubMesh& subMesh)
    {
    	if (!primitives.get())
    	{
    		return;
    	}

    	auto currentPrimitive = JSONobjectSP(new JSONobject());

    	if (!currentPrimitive.get())
    	{
    		return;
    	}

    	primitives->addValue(currentPrimitive);

    	//
    	// Mode
		//

    	int32_t mode = 0;

    	switch (subMesh.getPrimitiveTopology())
    	{
			case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
				mode = 0;
				break;
			case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
				mode = 1;
				break;
			case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
				mode = 3;
				break;
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
				mode = 4;
				break;
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
				mode = 5;
				break;
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
				mode = 6;
				break;
			default:
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "SubMesh has unsupported topology 0x%x!", (uint32_t)subMesh.getPrimitiveTopology());
				return;
    	}

    	auto modeValue = JSONintegerSP(new JSONinteger(mode));

    	if (!modeValue.get())
    	{
    		return;
    	}

    	currentPrimitive->addKeyValue("mode", modeValue);

    	//
    	// material
    	//

    	if (!subMesh.getBSDFMaterial().get() || !subMesh.getBSDFMaterial()->isSorted() || (subMesh.getBSDFMaterial()->isPacked() && (subMesh.getBSDFMaterial()->getTextureObjects().size() != 5)) || (!subMesh.getBSDFMaterial()->isPacked() && (subMesh.getBSDFMaterial()->getTextureObjects().size() != 6)))
    	{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "SubMesh has unsupported material!");
			return;
    	}

    	if (subMesh.getBSDFMaterial()->isSpecularGlossiness())
    	{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Specular Glossiness material can not yet be exported");
			return;
    	}

    	//

    	if (!materialNameToMaterial.contains(subMesh.getBSDFMaterial()->getName()))
    	{
        	auto currentMaterial = JSONobjectSP(new JSONobject());

        	if (!currentMaterial.get())
        	{
        		return;
        	}

        	materials->addValue(currentMaterial);

        	materialNameToMaterial[subMesh.getBSDFMaterial()->getName()] = currentMaterial;

        	//

        	if (subMesh.getDoubleSided())
        	{
            	auto doubleSidedValue = JSONtrueSP(new JSONtrue());

            	if (!doubleSidedValue.get())
            	{
            		return;
            	}

            	currentMaterial->addKeyValue("doubleSided", doubleSidedValue);
        	}
    	}

    	//

    	auto materialValue = JSONintegerSP(new JSONinteger((int32_t)materials->size() - 1));

    	if (!materialValue.get())
    	{
    		return;
    	}

    	currentPrimitive->addKeyValue("material", materialValue);

    	//

    	std::string accessorName = subMesh.getName() + "_indices";

		uint32_t indicesIndex = storedAccessors.index(accessorName);

		if (indicesIndex == storedAccessors.size())
		{
			uint32_t componentType = 5123;

			if (subMesh.getNumberIndices() > 65535)
			{
				componentType = 5125;
			}

			writeBinaryBuffer(subMesh.getIndicesBinaryBuffer(), subMesh.getNumberIndices(), 1, "SCALAR", componentType, 34963, 0, sizeof(int32_t));
			storedAccessors.append(accessorName);
		}

    	auto indicesValue = JSONintegerSP(new JSONinteger(indicesIndex));

    	if (!indicesValue.get())
    	{
    		return;
    	}

    	currentPrimitive->addKeyValue("indices", indicesValue);

    	//
    	//
    	//

    	auto attributesValue = JSONobjectSP(new JSONobject());

    	if (!attributesValue.get())
    	{
    		return;
    	}

    	currentPrimitive->addKeyValue("attributes", attributesValue);

    	//

    	if (subMesh.getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_VERTEX)
    	{
        	accessorName = subMesh.getName() + "_vertices";

    		uint32_t positionIndex = storedAccessors.index(accessorName);

    		if (positionIndex == storedAccessors.size())
    		{
    			writeBinaryBuffer(subMesh.getVertexBinaryBuffer(), subMesh.getNumberVertices(), 3, "VEC3", 5126, 34962, (uint32_t)subMesh.getVertexOffset(), (uint32_t)subMesh.getStrideInBytes());
    			storedAccessors.append(accessorName);
    		}

        	auto positionValue = JSONintegerSP(new JSONinteger(positionIndex));

        	if (!positionValue.get())
        	{
        		return;
        	}

        	attributesValue->addKeyValue("POSITION", positionValue);
    	}

    	if (subMesh.getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_NORMAL)
    	{
        	accessorName = subMesh.getName() + "_normals";

    		uint32_t normalIndex = storedAccessors.index(accessorName);

    		if (normalIndex == storedAccessors.size())
    		{
    			writeBinaryBuffer(subMesh.getVertexBinaryBuffer(), subMesh.getNumberVertices(), 3, "VEC3", 5126, 34962, (uint32_t)subMesh.getNormalOffset(), (uint32_t)subMesh.getStrideInBytes());
    			storedAccessors.append(accessorName);
    		}

        	auto normalValue = JSONintegerSP(new JSONinteger(normalIndex));

        	if (!normalValue.get())
        	{
        		return;
        	}

        	attributesValue->addKeyValue("NORMAL", normalValue);
    	}

    	if (subMesh.getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_TANGENTS)
    	{
        	accessorName = subMesh.getName() + "_tangents4";

    		uint32_t tangent4Index = storedAccessors.index(accessorName);

    		if (tangent4Index == storedAccessors.size())
    		{
    			auto tangent4Buffer = binaryBufferCreate(subMesh.getNumberVertices() * 4 * sizeof(float));

    			if (!tangent4Buffer.get())
    			{
    				return;
    			}

    			for (uint32_t i = 0; i < (uint32_t)subMesh.getNumberVertices(); i++)
    			{
    				const float* currentTangent = (const float*)(&subMesh.getVertexBinaryBuffer()->getByteData()[subMesh.getTangentOffset() + subMesh.getStrideInBytes() * i]);
    				const float* currentBitangent = (const float*)(&subMesh.getVertexBinaryBuffer()->getByteData()[subMesh.getBitangentOffset() + subMesh.getStrideInBytes() * i]);
    				const float* currentNormal = (const float*)(&subMesh.getVertexBinaryBuffer()->getByteData()[subMesh.getNormalOffset() + subMesh.getStrideInBytes() * i]);

    				//

    				glm::vec3 tangent(currentTangent[0], currentTangent[1], currentTangent[2]);
    				glm::vec3 bitangent(currentBitangent[0], currentBitangent[1], currentBitangent[2]);
    				glm::vec3 normal(currentNormal[0], currentNormal[1], currentNormal[2]);

    				float handedness = glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;

    				//

					float tangent4[4] = {currentTangent[0], currentTangent[1], currentTangent[2], handedness};

    				tangent4Buffer->write(tangent4, sizeof(float), 4);
    			}

    			writeBinaryBuffer(tangent4Buffer, subMesh.getNumberVertices(), 4, "VEC4", 5126, 34962, 0, 4 * sizeof(float));

    			storedAccessors.append(accessorName);
    		}

        	auto tangent4Value = JSONintegerSP(new JSONinteger(tangent4Index));

        	if (!tangent4Value.get())
        	{
        		return;
        	}

        	attributesValue->addKeyValue("TANGENT", tangent4Value);
    	}

    	if (subMesh.getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0)
    	{
        	accessorName = subMesh.getName() + "_texcoords0";

    		uint32_t texCoordIndex = storedAccessors.index(accessorName);

    		if (texCoordIndex == storedAccessors.size())
    		{
    			writeBinaryBuffer(subMesh.getVertexBinaryBuffer(), subMesh.getNumberVertices(), 2, "VEC2", 5126, 34962, (uint32_t)subMesh.getTexcoord0Offset(), (uint32_t)subMesh.getStrideInBytes());
    			storedAccessors.append(accessorName);
    		}

        	auto texCoordValue = JSONintegerSP(new JSONinteger(texCoordIndex));

        	if (!texCoordValue.get())
        	{
        		return;
        	}

        	attributesValue->addKeyValue("TEXCOORD_0", texCoordValue);
    	}

    	if (subMesh.getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD1)
    	{
        	accessorName = subMesh.getName() + "_texcoords1";

    		uint32_t texCoordIndex = storedAccessors.index(accessorName);

    		if (texCoordIndex == storedAccessors.size())
    		{
    			writeBinaryBuffer(subMesh.getVertexBinaryBuffer(), subMesh.getNumberVertices(), 2, "VEC2", 5126, 34962, (uint32_t)subMesh.getTexcoord1Offset(), (uint32_t)subMesh.getStrideInBytes());
    			storedAccessors.append(accessorName);
    		}

        	auto texCoordValue = JSONintegerSP(new JSONinteger(texCoordIndex));

        	if (!texCoordValue.get())
        	{
        		return;
        	}

        	attributesValue->addKeyValue("TEXCOORD_1", texCoordValue);
    	}
    }

    virtual void visit(IBSDFMaterial& material)
    {
    	if (!images.get() || !materials.get() || !materialNameToMaterial.contains(material.getName()))
    	{
    		return;
    	}

    	//

    	// Check, if values already gathered.
    	auto currentMaterial = materialNameToMaterial[material.getName()];

    	if (currentMaterial->size() != 0)
    	{
    		if (currentMaterial->size() != 1 || !currentMaterial->hasKey("doubleSided"))
    		{
    			return;
    		}
    	}

    	//

    	auto nameValue = JSONstringSP(new JSONstring(material.getName()));

    	if (!nameValue.get())
    	{
    		return;
    	}

    	currentMaterial->addKeyValue("name", nameValue);

    	//

    	if (material.isTransparent())
    	{
        	auto alphaModeValue = JSONstringSP(new JSONstring("BLEND"));

        	if (!alphaModeValue.get())
        	{
        		return;
        	}

        	currentMaterial->addKeyValue("alphaMode", alphaModeValue);
    	}
    	else if (material.getAlphaCutoff() > 0.0f)
    	{
        	auto alphaModeValue = JSONstringSP(new JSONstring("MASK"));

        	if (!alphaModeValue.get())
        	{
        		return;
        	}

        	currentMaterial->addKeyValue("alphaMode", alphaModeValue);

        	//

        	auto alphaCutoffValue = JSONfloatSP(new JSONfloat(material.getAlphaCutoff()));

        	if (!alphaCutoffValue.get())
        	{
        		return;
        	}

        	currentMaterial->addKeyValue("alphaCutoff", alphaCutoffValue);
    	}

    	//

    	auto pbrMetallicRoughness = JSONobjectSP(new JSONobject());

    	if (!pbrMetallicRoughness.get())
    	{
    		return;
    	}

    	currentMaterial->addKeyValue("pbrMetallicRoughness", pbrMetallicRoughness);

    	for (uint32_t i = 0; i < material.getTextureObjects().size(); i++)
    	{
        	auto textureValue = JSONobjectSP(new JSONobject());

        	if (!textureValue.get())
        	{
        		return;
        	}

    		//

    		IImageDataSP currentImageData;
    		uint32_t usedIndex = i;

    		if (material.isPacked() || i == 0)
    		{
    			currentImageData = material.getTextureObjects()[i]->getImageObject()->getImageData();
    		}
    		else
    		{
    			if (i == 1)
    			{
    				usedIndex = i;
    				i++;

    				//

    				std::string imageName = "";

    				std::string imageNameMetallic = material.getTextureObjects()[1]->getImageObject()->getImageData()->getName();
    				std::string imageNameRoughness = material.getTextureObjects()[2]->getImageObject()->getImageData()->getName();

    				// Gather common letters, that name looks identical.
    				uint32_t charIndex = 0;
    				while (charIndex < imageNameMetallic.size() && charIndex < imageNameRoughness.size() && imageNameMetallic[charIndex] == imageNameRoughness[charIndex])
    				{
    					imageName.push_back(imageNameMetallic[charIndex]);

						charIndex++;
    				}

    				// Fallback, if names did completely mismatch.
    				if (imageName.size() == 0)
    				{
    					imageName = material.getName() + "_";
    				}

    				imageName += "MetallicRoughness.png";

    				//

    				currentImageData = imageDataUnite(material.getTextureObjects()[1]->getImageObject()->getImageData(), material.getTextureObjects()[2]->getImageObject()->getImageData(), imageName, VKTS_SOURCE_ZERO, VKTS_SOURCE_1_RED, VKTS_SOURCE_0_RED, VKTS_SOURCE_ONE, VK_FORMAT_R8G8B8A8_UNORM);
    			}
    			else
    			{
    				usedIndex = i - 1;

    				currentImageData = material.getTextureObjects()[i]->getImageObject()->getImageData();
    			}
    		}

    		//

    		std::string storeImageName = currentImageData->getName();

    		auto index = storeImageName.find_last_of('/');

    		if (index != storeImageName.npos)
    		{
    			storeImageName = storeImageName.substr(index + 1);
    		}
    		else
    		{
    			index = storeImageName.find_last_of('\\');

        		if (index != storeImageName.npos)
        		{
        			storeImageName = storeImageName.substr(index + 1);
        		}
    		}

    		index = storeImageName.find_first_of('.');

    		if (index != storeImageName.npos)
    		{
    			storeImageName = storeImageName.substr(0, index);
    		}

    		storeImageName += ".png";

    		//

    		VkBool32 storeImage = VK_TRUE;

    		glm::vec4 texel;

    		switch (usedIndex)
    		{
    			case 0:
    				if (currentImageData->getWidth() == 1 && currentImageData->getHeight() == 1)
    				{
    					storeImage = VK_FALSE;

    					texel = currentImageData->getTexel(0, 0, 0, 0, 0);

    					//

    					auto baseColorFactor = JSONarraySP(new JSONarray());

    					if (!baseColorFactor.get())
    					{
    						return;
    					}

    					for (uint32_t k = 0; k < 4; k++)
    					{
    				    	auto floatValue = JSONfloatSP(new JSONfloat(texel[k]));

    				    	if (!floatValue.get())
    				    	{
    				    		return;
    				    	}

    				    	baseColorFactor->addValue(floatValue);
    					}

    					pbrMetallicRoughness->addKeyValue("baseColorFactor", baseColorFactor);
    				}
    				else
    				{
    					pbrMetallicRoughness->addKeyValue("baseColorTexture", textureValue);
    				}

    				break;
    			case 1:
    				if (currentImageData->getWidth() == 1 && currentImageData->getHeight() == 1)
    				{
    					storeImage = VK_FALSE;

    					texel = currentImageData->getTexel(0, 0, 0, 0, 0);

    					//

    					auto metallicFactor = JSONfloatSP(new JSONfloat(texel.x));

    					if (!metallicFactor.get())
    					{
    						return;
    					}

    					pbrMetallicRoughness->addKeyValue("metallicFactor", metallicFactor);

    					//

    					auto roughnessFactor = JSONfloatSP(new JSONfloat(texel.y));

    					if (!roughnessFactor.get())
    					{
    						return;
    					}

    					pbrMetallicRoughness->addKeyValue("roughnessFactor", roughnessFactor);
    				}
    				else
    				{
    					pbrMetallicRoughness->addKeyValue("metallicRoughnessTexture", textureValue);
    				}

    				break;
    			case 2:
    				if (currentImageData->getWidth() == 1 && currentImageData->getHeight() == 1)
    				{
    					texel = currentImageData->getTexel(0, 0, 0, 0, 0);

    					//

    					auto localNormal = texel * 2.0f - 1.0f;

    					if (localNormal.z >= 1.0f - 0.001f)
    					{
        					storeImage = VK_FALSE;
    					}
    				}

    				if (storeImage)
    				{
    					currentMaterial->addKeyValue("normalTexture", textureValue);
    				}

    				break;
    			case 3:
    				if (currentImageData->getWidth() == 1 && currentImageData->getHeight() == 1)
    				{
    					texel = currentImageData->getTexel(0, 0, 0, 0, 0);

    					//

    					if (texel.x == 1.0f)
    					{
        					storeImage = VK_FALSE;
    					}
    				}

    				if (storeImage)
    				{
    					currentMaterial->addKeyValue("occlusionTexture", textureValue);

    					//

    					if (material.getAmbientOcclusionStrength() < 1.0f)
    					{
    				    	auto strength = JSONfloatSP(new JSONfloat(material.getAmbientOcclusionStrength()));

    				    	if (!strength.get())
    				    	{
    				    		return;
    				    	}

    				    	textureValue->addKeyValue("strength", strength);
    					}
    				}

    				break;
    			case 4:
    				if (currentImageData->getWidth() == 1 && currentImageData->getHeight() == 1)
    				{
    					storeImage = VK_FALSE;

    					texel = currentImageData->getTexel(0, 0, 0, 0, 0);

    					//

    					if (texel.x != 0.0f || texel.y != 0.0f || texel.z != 0.0f)
    					{
							auto emissiveFactor = JSONarraySP(new JSONarray());

							if (!emissiveFactor.get())
							{
								return;
							}

							for (uint32_t k = 0; k < 3; k++)
							{
								auto floatValue = JSONfloatSP(new JSONfloat(texel[k]));

								if (!floatValue.get())
								{
									return;
								}

								emissiveFactor->addValue(floatValue);
							}

							currentMaterial->addKeyValue("emissiveFactor", emissiveFactor);
    					}
    				}
    				else
    				{
    					currentMaterial->addKeyValue("emissiveTexture", textureValue);
    				}

    				break;
    		}

    		if (storeImage)
    		{
    			// Create default sampler if it does not exist.
    			if (samplers->size() == 0)
    			{
    		    	auto currentSampler = JSONobjectSP(new JSONobject());

    		    	if (!currentSampler.get())
    		    	{
    		    		return;
    		    	}

    		    	samplers->addValue(currentSampler);
    			}

    			//

    			uint32_t imageIndex = storedImages.index(storeImageName);

    			if (imageIndex == storedImages.size())
    			{
    		    	auto currentImage = JSONobjectSP(new JSONobject());

    		    	if (!currentImage.get())
    		    	{
    		    		return;
    		    	}

    		    	images->addValue(currentImage);

    				//

    	        	auto uriValue = JSONstringSP(new JSONstring(storeImageName));

    	        	if (!uriValue.get())
    	        	{
    	        		return;
    	        	}

    	        	currentImage->addKeyValue("uri", uriValue);

    		    	//

    		    	storedImages.append(storeImageName);

    		    	//

    		    	storedImagesMap[storeImageName] = currentImageData;

    		    	//
    		    	// For now, for every image we have a texture.
    		    	//

    		    	auto currentTexture = JSONobjectSP(new JSONobject());

    		    	if (!currentTexture.get())
    		    	{
    		    		return;
    		    	}

    		    	textures->addValue(currentTexture);

    				//

    	        	auto samplerValue = JSONintegerSP(new JSONinteger(0));

    	        	if (!samplerValue.get())
    	        	{
    	        		return;
    	        	}

    	        	currentTexture->addKeyValue("sampler", samplerValue);

    	        	//

    	        	auto sourceValue = JSONintegerSP(new JSONinteger((int32_t)imageIndex));

    	        	if (!sourceValue.get())
    	        	{
    	        		return;
    	        	}

    	        	currentTexture->addKeyValue("source", sourceValue);
    			}

    			//
    			// Now, set reference to texture index, which is the same as image index.
    			//

		    	auto indexValue = JSONintegerSP(new JSONinteger((int32_t)imageIndex));

		    	if (!indexValue.get())
		    	{
		    		return;
		    	}

		    	textureValue->addKeyValue("index", indexValue);
    		}
    	}
    }

};

} /* namespace vkts */

#endif /* VKTS_GLTFPARAMETER_HPP_ */
