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

namespace vkts
{

static VkBool32 sceneLoadImageObjects(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    VkBool32 bdata;

    std::string imageObjectName;
    VkBool32 mipMap = VK_FALSE;
    VkBool32 environment = VK_FALSE;
    VkTsEnvironmentType environmentType = VKTS_ENVIRONMENT_PANORAMA;
    VkBool32 preFiltered = VK_FALSE;
    IImageDataSP imageData;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }
        if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid name");

                return VK_FALSE;
            }

            imageObjectName = sdata;

            mipMap = VK_FALSE;
            environment = VK_FALSE;
            environmentType = VKTS_ENVIRONMENT_PANORAMA;
            preFiltered = VK_FALSE;
        }
        else if (parseIsToken(buffer, "mipmap"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            mipMap = bdata;
        }
        else if (parseIsToken(buffer, "environment"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            environment = bdata;
        }
        else if (parseIsToken(buffer, "environment_type"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            environmentType = VKTS_ENVIRONMENT_PANORAMA;

            if (strcmp(sdata, "panorama") == 0)
            {
            	environmentType = VKTS_ENVIRONMENT_PANORAMA;
            }
            else if (strcmp(sdata, "mirror_sphere") == 0)
            {
            	environmentType = VKTS_ENVIRONMENT_MIRROR_SPHERE;
            }
            else if (strcmp(sdata, "mirror_dome") == 0)
            {
            	environmentType = VKTS_ENVIRONMENT_MIRROR_DOME;
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown environment type '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "pre_filtered"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            preFiltered = bdata;
        }
        else if (parseIsToken(buffer, "image_data"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            auto imageDataFilename = std::string(sdata);

			// Check, if combination is supported.

			if (mipMap && environment)
			{
				return VK_FALSE;
			}

			if (preFiltered && !environment)
			{
				return VK_FALSE;
			}

			// Check, if image object name is valid.

			if (imageObjectName == "")
			{
				return VK_FALSE;
			}

			//

			std::string finalImageDataFilename = directory + imageDataFilename;

			auto imageData = sceneManager->useImageData(finalImageDataFilename.c_str());

			if (!imageData.get())
			{
				imageData = sceneManager->useImageData(imageDataFilename.c_str());

				if (!imageData.get())
				{
					// Load image data.

					imageData = imageDataLoad(finalImageDataFilename.c_str());

					if (!imageData.get())
					{
						std::string textureImageDataFilename = std::string(VKTS_TEXTURE_DIRECTORY) + imageDataFilename;

						imageData = imageDataLoad(textureImageDataFilename.c_str());

						if (!imageData.get())
						{
							imageData = imageDataLoad(imageDataFilename.c_str());

							if (!imageData.get())
							{
								logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load image data '%s'", finalImageDataFilename.c_str());

								return VK_FALSE;
							}
						}
					}

					//

					if (mipMap && imageData->getMipLevels() == 1 && (imageData->getExtent3D().width > 1 || imageData->getExtent3D().height > 1 || imageData->getExtent3D().depth > 1))
					{
						//
						// Mip map image creation.
						//

						auto dotIndex = finalImageDataFilename.rfind(".");

						if (dotIndex == finalImageDataFilename.npos)
						{
							logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No valid image filename '%s'", finalImageDataFilename.c_str());

							return VK_FALSE;
						}

						auto sourceImageName = finalImageDataFilename.substr(0, dotIndex);
						auto sourceImageExtension = finalImageDataFilename.substr(dotIndex);

						int32_t width = imageData->getWidth();
						int32_t height = imageData->getHeight();
						int32_t depth = imageData->getDepth();

						SmartPointerVector<IImageDataSP> allMipMaps;

						if (cacheGetEnabled())
						{
							allMipMaps.append(imageData);

							int32_t level = 1;

							while (width > 1 || height > 1 || depth > 1)
							{
								width = glm::max(width / 2, 1);
								height = glm::max(height / 2, 1);
								depth = glm::max(depth / 2, 1);

								auto targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(level++) + sourceImageExtension;

								auto targetImage = cacheLoadImageData(targetImageFilename.c_str());

								if (!targetImage.get())
								{
									allMipMaps.clear();

									break;
								}

								allMipMaps.append(targetImage);

								//

								width = targetImage->getWidth();
								height = targetImage->getHeight();
								depth = targetImage->getDepth();
							}
						}

						//

						if (allMipMaps.size() == 0)
						{
							allMipMaps = imageDataMipmap(imageData, VK_FALSE, finalImageDataFilename);

							if (allMipMaps.size() == 0)
							{
								logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create mip maps for '%s'", finalImageDataFilename.c_str());

								return VK_FALSE;
							}

							if (cacheGetEnabled())
							{
								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

								// Only cache mip maps sub levels.
								for (uint32_t i = 1; i < allMipMaps.size(); i++)
								{
									cacheSaveImageData(allMipMaps[i]);
								}
							}
						}
						else
						{
							logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
						}

						for (uint32_t i = 0; i < allMipMaps.size(); i++)
						{
							allMipMaps[i] = createDeviceImageData(sceneManager->getAssetManager(), allMipMaps[i]);
						}

						imageData = imageDataMerge(allMipMaps, finalImageDataFilename, allMipMaps.size(), 1);

						if (!imageData.get())
						{
							logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

							return VK_FALSE;
						}
					}
					else if (environment)
					{
						//
						// Cube map image creation.
						//

						auto dotIndex = finalImageDataFilename.rfind(".");

						if (dotIndex == finalImageDataFilename.npos)
						{
							logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No valid image filename '%s'", finalImageDataFilename.c_str());

							return VK_FALSE;
						}

						auto sourceImageName = finalImageDataFilename.substr(0, dotIndex);
						auto sourceImageExtension = finalImageDataFilename.substr(dotIndex);

						if (imageData->getArrayLayers() % 6 != 0)
						{
							SmartPointerVector<IImageDataSP> allCubeMaps;

							if (cacheGetEnabled())
							{
								uint32_t levelCount = (uint32_t)log2f((float)(imageData->getHeight()));

								if (cacheGetEnabled())
								{
									for (uint32_t layer = 0; layer < 6; layer++)
									{
										for (uint32_t level = 0; level < levelCount; level++)
										{
											auto targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(level) + "_LAYER" + std::to_string(layer) + sourceImageExtension;

											auto targetImageData = cacheLoadImageData(targetImageFilename.c_str());

											if (!targetImageData.get())
											{
												allCubeMaps.clear();

												break;
											}

											allCubeMaps.append(targetImageData);
										}
									}
								}
							}

							//

							if (allCubeMaps.size() == 0)
							{
								uint32_t currentMapLength = imageData->getHeight() / 2;

								uint32_t cubeMapLength = 1;
								while (cubeMapLength * 2 <= currentMapLength)
								{
									cubeMapLength *= 2;
								}

								if (environmentType == VKTS_ENVIRONMENT_MIRROR_DOME)
								{
									cubeMapLength *= 2;
								}

								auto oldAllCubeMaps = imageDataCubemap(imageData, cubeMapLength, finalImageDataFilename, environmentType);

								if (oldAllCubeMaps.size() != 6)
								{
									logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cube maps for '%s'", finalImageDataFilename.c_str());

									return VK_FALSE;
								}

						        for (uint32_t layer = 0; layer < 6; layer++)
						        {
									auto tempMipMaps = imageDataMipmap(oldAllCubeMaps[layer], VK_FALSE, finalImageDataFilename);

									if (tempMipMaps.size() == 0)
									{
										logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create mip maps for '%s'", finalImageDataFilename.c_str());

										return VK_FALSE;
									}

									for (uint32_t mipLevel = 0; mipLevel < tempMipMaps.size(); mipLevel++)
									{
										allCubeMaps.append(tempMipMaps[mipLevel]);
									}
						        }

								if (cacheGetEnabled())
								{
									logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

									uint32_t mipLevels = allCubeMaps.size() / 6;

									for (uint32_t i = 0; i < allCubeMaps.size(); i++)
									{
										auto targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(i % mipLevels) + "_LAYER" + std::to_string(i / mipLevels) + sourceImageExtension;

										cacheSaveImageData(allCubeMaps[i], targetImageFilename);
									}
								}
							}
							else
							{
								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
							}

							for (uint32_t i = 0; i < allCubeMaps.size(); i++)
							{
								allCubeMaps[i] = createDeviceImageData(sceneManager->getAssetManager(), allCubeMaps[i]);
							}

							imageData = imageDataMerge(allCubeMaps, finalImageDataFilename, allCubeMaps.size() / 6, 6);

							if (!imageData.get())
							{
								logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

								return VK_FALSE;
							}
						}

						if (preFiltered)
						{
							//
							// Pre-filtered diffuse cube map creation.
							//

							SmartPointerVector<IImageDataSP> allDiffuseCubeMaps;

							if (cacheGetEnabled())
							{
								for (uint32_t layer = 0; layer < 6; layer++)
								{
									auto targetImageFilename = sourceImageName + "_LEVEL0_LAYER" + std::to_string(layer) + "_LAMBERT" + sourceImageExtension;

									auto targetImage = cacheLoadImageData(targetImageFilename.c_str());

									if (!targetImage.get())
									{
										allDiffuseCubeMaps.clear();

										break;
									}

									allDiffuseCubeMaps.append(targetImage);
								}
							}

							if (allDiffuseCubeMaps.size() == 0)
							{
								if (sceneFactory->useGPU())
								{
									allDiffuseCubeMaps = sceneFactory->getSceneRenderFactory()->prefilterLambert(sceneManager, imageData, VKTS_BSDF_SAMPLES_GPU_CUBE_MAP, finalImageDataFilename);
								}
								else
								{
									allDiffuseCubeMaps = imageDataPrefilterLambert(imageData, VKTS_BSDF_SAMPLES_CPU_CUBE_MAP, finalImageDataFilename);
								}

								if (allDiffuseCubeMaps.size() == 0)
								{
									logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create diffuse cube maps for '%s'", finalImageDataFilename.c_str());

									return VK_FALSE;
								}

								if (cacheGetEnabled())
								{
									logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

									for (uint32_t i = 0; i < allDiffuseCubeMaps.size(); i++)
									{
										if (allDiffuseCubeMaps[i]->getFormat() == VK_FORMAT_R32G32B32A32_SFLOAT)
										{
											auto tempCubeMap = imageDataConvert(allDiffuseCubeMaps[i], VK_FORMAT_R32G32B32_SFLOAT, allDiffuseCubeMaps[i]->getName());

											cacheSaveImageData(tempCubeMap);
										}
										else
										{
											cacheSaveImageData(allDiffuseCubeMaps[i]);
										}
									}
								}
							}
							else
							{
								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
							}

							for (uint32_t i = 0; i < allDiffuseCubeMaps.size(); i++)
							{
								allDiffuseCubeMaps[i] = createDeviceImageData(sceneManager->getAssetManager(), allDiffuseCubeMaps[i]);
							}

							auto diffuseImageData = imageDataMerge(allDiffuseCubeMaps, finalImageDataFilename, 1, allDiffuseCubeMaps.size());

							if (!diffuseImageData.get())
							{
								logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

								return VK_FALSE;
							}

							sceneManager->addImageData(diffuseImageData);

							//

							auto imageObject = createImageObject(sceneManager->getAssetManager(), imageObjectName + "_LAMBERT", diffuseImageData, environment);

							if (!imageObject.get())
							{
								return VK_FALSE;
							}

							sceneManager->addImageObject(imageObject);

							//
							// Pre-filtered cook torrance cube map creation.
							//

							SmartPointerVector<IImageDataSP> allCookTorranceCubeMaps;

							uint32_t levelCount = 1;

							if (cacheGetEnabled())
							{
								for (uint32_t layer = 0; layer < 6; layer++)
								{
									for (uint32_t level = 0; level < levelCount; level++)
									{
										auto targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(level) + "_LAYER" + std::to_string(layer) + "_COOKTORRANCE" + sourceImageExtension;

										auto targetImageData = cacheLoadImageData(targetImageFilename.c_str());

										if (!targetImageData.get())
										{
											allCookTorranceCubeMaps.clear();

											break;
										}

										// Gather level count iterations by first image.
										if (layer == 0 && level == 0)
										{
											uint32_t testWidth = targetImageData->getWidth();

											while (testWidth > 1)
											{
												testWidth /= 2;

												levelCount++;
											}
										}

										allCookTorranceCubeMaps.append(targetImageData);
									}
								}
							}

							if (allCookTorranceCubeMaps.size() == 0)
							{
								if (sceneFactory->useGPU())
								{
									allCookTorranceCubeMaps = sceneFactory->getSceneRenderFactory()->prefilterCookTorrance(sceneManager, imageData, VKTS_BSDF_SAMPLES_GPU_CUBE_MAP, finalImageDataFilename);
								}
								else
								{
									allCookTorranceCubeMaps = imageDataPrefilterCookTorrance(imageData, VKTS_BSDF_SAMPLES_CPU_CUBE_MAP, finalImageDataFilename);
								}

								if (allCookTorranceCubeMaps.size() == 0)
								{
									logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cook torrance cube maps for '%s'", finalImageDataFilename.c_str());

									return VK_FALSE;
								}

								levelCount = allCookTorranceCubeMaps.size() / 6;

								if (cacheGetEnabled())
								{
									logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

									for (uint32_t i = 0; i < allCookTorranceCubeMaps.size(); i++)
									{
										if (allCookTorranceCubeMaps[i]->getFormat() == VK_FORMAT_R32G32B32A32_SFLOAT)
										{
											auto tempCubeMap = imageDataConvert(allCookTorranceCubeMaps[i], VK_FORMAT_R32G32B32_SFLOAT, allCookTorranceCubeMaps[i]->getName());

											cacheSaveImageData(tempCubeMap);
										}
										else
										{
											cacheSaveImageData(allCookTorranceCubeMaps[i]);
										}
									}
								}
							}
							else
							{
								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
							}

							for (uint32_t i = 0; i < allCookTorranceCubeMaps.size(); i++)
							{
								allCookTorranceCubeMaps[i] = createDeviceImageData(sceneManager->getAssetManager(), allCookTorranceCubeMaps[i]);
							}

							auto cookTorranceImageData = imageDataMerge(allCookTorranceCubeMaps, finalImageDataFilename, levelCount, 6);

							if (!cookTorranceImageData.get())
							{
								logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

								return VK_FALSE;
							}

							sceneManager->addImageData(cookTorranceImageData);

							//

							imageObject = createImageObject(sceneManager->getAssetManager(), imageObjectName + "_COOKTORRANCE", cookTorranceImageData, environment);

							if (!imageObject.get())
							{
								return VK_FALSE;
							}

							sceneManager->addImageObject(imageObject);

							//
							// BSDF environment look up table generation.
							//

							auto lutImageObjectName = "BSDF_LUT_" + std::to_string(VKTS_BSDF_LENGTH) + "_" + std::to_string(VKTS_BSDF_SAMPLES);

							auto lutImageFilename = std::string(VKTS_TEXTURE_DIRECTORY) + lutImageObjectName + ".data";

							IImageDataSP lutImageData = imageDataLoadRaw(lutImageFilename.c_str(), VKTS_BSDF_LENGTH, VKTS_BSDF_LENGTH, VK_FORMAT_R32G32_SFLOAT);

							if (!lutImageData.get())
							{
								lutImageData = imageDataEnvironmentBRDF(VKTS_BSDF_LENGTH, VKTS_BSDF_SAMPLES, "BSDF_LUT.data");

								if (!lutImageData.get())
								{
									logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not generate BSDF lut");

									return VK_FALSE;
								}

								if (!imageDataSave(lutImageFilename.c_str(), lutImageData))
								{
									logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not save BSDF lut");

									return VK_FALSE;
								}
							}

							lutImageData = createDeviceImageData(sceneManager->getAssetManager(), lutImageData);

							if (!lutImageData.get())
							{
								logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not modify BSDF lut");

								return VK_FALSE;
							}

							sceneManager->addImageData(lutImageData);

							//

							imageObject = createImageObject(sceneManager->getAssetManager(), lutImageObjectName, lutImageData, VK_FALSE);

							if (!imageObject.get())
							{
								return VK_FALSE;
							}

							sceneManager->addImageObject(imageObject);
						}
					}
					else
					{
						imageData = createDeviceImageData(sceneManager->getAssetManager(), imageData);

						if (!imageData.get())
						{
							return VK_FALSE;
						}
					}

					//

					sceneManager->addImageData(imageData);
				}
			}

			//
			// ImageObject creation.
			//

			auto imageObject = createImageObject(sceneManager->getAssetManager(), imageObjectName, imageData, environment);

            if (!imageObject.get())
            {
            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No memory image for '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            sceneManager->addImageObject(imageObject);
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 sceneLoadTextureObjects(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    VkBool32 bdata;

    std::string textureObjectName;
    VkBool32 mipMap = VK_FALSE;
    VkBool32 environment = VK_FALSE;
    VkBool32 preFiltered = VK_FALSE;
    IImageObjectSP imageObject;

    ITextureObjectSP textureObject;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "image_library"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadImageObjects(directory, sdata, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load images: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObjectName = sdata;

            mipMap = VK_FALSE;
            environment = VK_FALSE;
            preFiltered = VK_FALSE;
        }
        else if (parseIsToken(buffer, "mipmap"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            mipMap = bdata;
        }
        else if (parseIsToken(buffer, "environment"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            environment = bdata;
        }
        else if (parseIsToken(buffer, "pre_filtered"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            preFiltered = bdata;
        }
        else if (parseIsToken(buffer, "image"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (mipMap && environment)
            {
            	return VK_FALSE;
            }

            if (!preFiltered && environment)
            {
            	return VK_FALSE;
            }

            //

            imageObject = sceneManager->useImageObject(sdata);

			if (!imageObject.get())
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

				return VK_FALSE;
			}

            // TextureObject creation.

            if (textureObjectName == "")
            {
                return VK_FALSE;
            }

            auto textureObject = createTextureObject(sceneManager->getAssetManager(), textureObjectName, mipMap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            sceneManager->addTextureObject(textureObject);

            //

            if (preFiltered)
            {
            	auto lambertName = std::string(sdata) + "_LAMBERT";

                imageObject = sceneManager->useImageObject(lambertName);

    			if (!imageObject.get())
    			{
    				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", lambertName.c_str());

    				return VK_FALSE;
    			}

                textureObject = createTextureObject(sceneManager->getAssetManager(), textureObjectName + "_LAMBERT", VK_FALSE, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

                if (!textureObject.get())
                {
                    return VK_FALSE;
                }

                sceneManager->addTextureObject(textureObject);

                //

            	auto cookTorranceName = std::string(sdata) + "_COOKTORRANCE";

                imageObject = sceneManager->useImageObject(cookTorranceName);

    			if (!imageObject.get())
    			{
    				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", cookTorranceName.c_str());

    				return VK_FALSE;
    			}

    			textureObject = createTextureObject(sceneManager->getAssetManager(), textureObjectName + "_COOKTORRANCE", VK_TRUE, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

                if (!textureObject.get())
                {
                    return VK_FALSE;
                }

                sceneManager->addTextureObject(textureObject);

                //

            	auto lutName = "BSDF_LUT_" + std::to_string(VKTS_BSDF_LENGTH) + "_" + std::to_string(VKTS_BSDF_SAMPLES);

                imageObject = sceneManager->useImageObject(lutName);

    			if (!imageObject.get())
    			{
    				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", lutName.c_str());

    				return VK_FALSE;
    			}

    			//

    			textureObject = createTextureObject(sceneManager->getAssetManager(), lutName, VK_FALSE, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, imageObject);

                if (!textureObject.get())
                {
                    return VK_FALSE;
                }

                sceneManager->addTextureObject(textureObject);
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 sceneLoadMaterials(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];
    uint32_t uidata;
    VkBool32 bdata;

    auto bsdfMaterial = IBSDFMaterialSP();
    auto phongMaterial = IPhongMaterialSP();

    auto textureObject = ITextureObjectSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "texture_library"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadTextureObjects(directory, sdata, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load textureObjects: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "shading"))
        {
            if (!parseStringBool(buffer, sdata, VKTS_MAX_TOKEN_CHARS, &bdata))
            {
                return VK_FALSE;
            }

            if (strncmp(sdata, "BSDF", 4) == 0)
            {
                bsdfMaterial = sceneFactory->createBSDFMaterial(sceneManager, bdata);
                phongMaterial = IPhongMaterialSP();

                if (!bsdfMaterial.get())
                {
                	return VK_FALSE;
                }
            }
            else if (strncmp(sdata, "Phong", 5) == 0)
            {
                bsdfMaterial = IBSDFMaterialSP();
                phongMaterial = sceneFactory->createPhongMaterial(sceneManager, bdata);

                if (!phongMaterial.get())
                {
                	return VK_FALSE;
                }

                if (sceneFactory->getSceneRenderFactory().get())
                {
					if (!sceneFactory->getSceneRenderFactory()->preparePhongMaterial(sceneManager, phongMaterial))
					{
						return VK_FALSE;
					}
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown shading: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->setName(sdata);

                sceneManager->addBSDFMaterial(bsdfMaterial);
            }
            else if (phongMaterial.get())
            {
                phongMaterial->setName(sdata);

                sceneManager->addPhongMaterial(phongMaterial);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "transparent"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setTransparent(bdata);
            }
            else if (bsdfMaterial.get())
            {
            	bsdfMaterial->setTransparent(bdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "sorted"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->setSorted(bdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "emissive_color"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], fdata[1], fdata[2], 1.0f), VK_FORMAT_R8G8B8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setEmissive(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "alpha_value"))
        {
            if (!parseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], 0.0f, 0.0f, 1.0f), VK_FORMAT_R8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setAlpha(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "displacement_value"))
        {
            if (!parseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], 0.0f, 0.0f, 1.0f), VK_FORMAT_R8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setDisplacement(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "normal_vector"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0] * 0.5f + 0.5f, fdata[1] * 0.5f + 0.5f, fdata[2] * 0.5f + 0.5f, 0.5f), VK_FORMAT_R8G8B8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setNormal(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "emissive_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setEmissive(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "alpha_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setAlpha(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "displacement_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setDisplacement(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "normal_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setNormal(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_ambient_color"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], fdata[1], fdata[2], 1.0f), VK_FORMAT_R8G8B8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setAmbient(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_diffuse_color"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], fdata[1], fdata[2], 1.0f), VK_FORMAT_R8G8B8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setDiffuse(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_specular_color"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], fdata[1], fdata[2], 1.0f), VK_FORMAT_R8G8B8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecular(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_specular_shininess_value"))
        {
            if (!parseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], 0.0f, 0.0f, 1.0f), VK_FORMAT_R8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecularShininess(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_mirror_color"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], fdata[1], fdata[2], 1.0f), VK_FORMAT_R8G8B8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirror(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_mirror_reflectivity_value"))
        {
            if (!parseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = createTextureObject(sceneManager->getAssetManager(), glm::vec4(fdata[0], 0.0f, 0.0f, 1.0f), VK_FORMAT_R8_UNORM);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirrorReflectivity(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_ambient_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setAmbient(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_diffuse_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setDiffuse(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_specular_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecular(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_specular_shininess_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecularShininess(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_mirror_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirror(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "phong_mirror_reflectivity_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirrorReflectivity(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "fragment_shader"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            auto shaderModule = sceneManager->useFragmentShaderModule(sdata);

            if (!shaderModule.get())
            {
                std::string finalFilename = std::string(directory) + std::string(sdata);

                auto shaderBinary = fileLoadBinary(finalFilename.c_str());

				if (!shaderBinary.get())
				{
					finalFilename = std::string(VKTS_SHADER_DIRECTORY) + std::string(sdata);

					shaderBinary = fileLoadBinary(finalFilename.c_str());

					if (!shaderBinary.get())
					{
						shaderBinary = fileLoadBinary(filename);

						if (!shaderBinary.get())
						{
							logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

							return VK_FALSE;
						}
					}
				}

				//

				shaderModule = createShaderModule(sceneManager->getAssetManager(), sdata, shaderBinary);

				if (!shaderModule.get())
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

					return VK_FALSE;
				}

				sceneManager->addFragmentShaderModule(shaderModule);
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->setFragmentShader(shaderModule);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "attributes"))
        {
            if (!parseUIntHex(buffer, &uidata))
            {
                return VK_FALSE;
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->setAttributes((VkTsVertexBufferType)uidata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "add_texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->addTextureObject(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 sceneLoadSubMeshes(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[8];
    int32_t idata[3];
    VkBool32 bdata;

    auto subMesh = ISubMeshSP();

    std::vector<float> vertex;
    std::vector<float> normal;
    std::vector<float> bitangent;
    std::vector<float> tangent;
    std::vector<float> texcoord;

    std::vector<float> boneIndices0;
    std::vector<float> boneIndices1;
    std::vector<float> boneWeights0;
    std::vector<float> boneWeights1;
    std::vector<float> numberBones;

    std::vector<int32_t> indices;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "material_library"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadMaterials(directory, sdata, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load materials: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            subMesh = sceneFactory->createSubMesh(sceneManager);

            if (!subMesh.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh not created: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            subMesh->setName(sdata);
        }
        else if (parseIsToken(buffer, "double_sided"))
        {
            if (!parseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
            	subMesh->setDoubleSided(bdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "vertex"))
        {
            if (!parseVec4(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 4; i++)
                {
                    vertex.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "normal"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    normal.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "bitangent"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    bitangent.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "tangent"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    tangent.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "texcoord"))
        {
            if (!parseVec2(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 2; i++)
                {
                    texcoord.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "boneIndex"))
        {
            if (!parseVec8(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 8; i++)
                {
                    if (i < 4)
                    {
                        boneIndices0.push_back(fdata[i]);
                    }
                    else
                    {
                        boneIndices1.push_back(fdata[i]);
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "boneWeight"))
        {
            if (!parseVec8(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 8; i++)
                {
                    if (i < 4)
                    {
                        boneWeights0.push_back(fdata[i]);
                    }
                    else
                    {
                        boneWeights1.push_back(fdata[i]);
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "numberBones"))
        {
            if (!parseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                numberBones.push_back(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "face"))
        {
            if (!parseIVec3(buffer, idata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    indices.push_back(idata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "material"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                const auto phongMaterial = sceneManager->usePhongMaterial(sdata);

                if (phongMaterial.get())
                {
                    subMesh->setPhongMaterial(phongMaterial);
                }
                else
                {
                    const auto bsdfMaterial = sceneManager->useBSDFMaterial(sdata);

                    if (bsdfMaterial.get())
                    {
                        subMesh->setBSDFMaterial(bsdfMaterial);
                    }
                    else
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Material not found: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                        return VK_FALSE;
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }

            //
            // Sub mesh creation.
            //

            if (subMesh.get())
            {
                subMesh->setNumberVertices((int32_t)(vertex.size() / 4));
                subMesh->setNumberIndices((int32_t) indices.size());

                int32_t totalSize = 0;
                uint32_t strideInBytes = 0;

                VkTsVertexBufferType vertexBufferType = 0;

                if (vertex.size() > 0)
                {
                    subMesh->setVertexOffset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_VERTEX;
                }

                if (normal.size() > 0)
                {
                    if (normal.size() / 3 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Normal has different size");

                        return VK_FALSE;
                    }

                    subMesh->setNormalOffset(strideInBytes);
                    strideInBytes += 3 * sizeof(float);

                    totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_NORMAL;
                }

                if (normal.size() > 0 && bitangent.size() > 0 && tangent.size() > 0)
                {
                    if (bitangent.size() / 3 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bitangent has different size");

                        return VK_FALSE;
                    }

                    subMesh->setBitangentOffset(strideInBytes);
                    strideInBytes += 3 * sizeof(float);

                    totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

                    //

                    if (tangent.size() / 3 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Tangent has different size");

                        return VK_FALSE;
                    }

                    subMesh->setTangentOffset(strideInBytes);
                    strideInBytes += 3 * sizeof(float);

                    totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

                    //

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TANGENTS;
                }

                if (texcoord.size() > 0)
                {
                    if (texcoord.size() / 2 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "TextureObject coordinate has different size");

                        return VK_FALSE;
                    }

                    subMesh->setTexcoord0Offset(strideInBytes);
                    strideInBytes += 2 * sizeof(float);

                    totalSize += 2 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0;
                }

                if (boneIndices0.size() > 0 && boneIndices1.size() > 0 && boneWeights0.size() > 0 && boneWeights1.size() > 0 && numberBones.size() > 0)
                {
                    if (boneIndices0.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone indices0 has different size: %u != %u", boneIndices0.size(), vertex.size());

                        return VK_FALSE;
                    }

                    subMesh->setBoneIndices0Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (boneIndices1.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone indices1 has different size %u != %u", boneIndices1.size(), vertex.size());

                        return VK_FALSE;
                    }

                    subMesh->setBoneIndices1Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (boneWeights0.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone weights0 has different size");

                        return VK_FALSE;
                    }

                    subMesh->setBoneWeights0Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (boneWeights1.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone weights has different size");

                        return VK_FALSE;
                    }

                    subMesh->setBoneWeights1Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (numberBones.size() != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Number bones has different size");

                        return VK_FALSE;
                    }

                    subMesh->setNumberBonesOffset(strideInBytes);
                    strideInBytes += 1 * sizeof(float);

                    totalSize += 1 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_BONES;
                }

                subMesh->setStrideInBytes(strideInBytes);

                if (totalSize > 0)
                {
                    auto vertexBinaryBuffer = binaryBufferCreate((uint32_t)totalSize);

                    if (!vertexBinaryBuffer.get() || vertexBinaryBuffer->getSize() != (uint32_t)totalSize)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex binary buffer");

                        return VK_FALSE;
                    }

                    for (int32_t currentVertexElement = 0; currentVertexElement < subMesh->getNumberVertices(); currentVertexElement++)
                    {
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_VERTEX)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&vertex[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_NORMAL)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&normal[currentVertexElement * 3]), 1, 3 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BITANGENT)
                        {
                            vertexBinaryBuffer->write( reinterpret_cast<const uint8_t*>(&bitangent[currentVertexElement * 3]), 1, 3 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TANGENT)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&tangent[currentVertexElement * 3]), 1, 3 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD0)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&texcoord[currentVertexElement * 2]), 1, 2 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneIndices0[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneIndices1[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneWeights0[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneWeights1[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&numberBones[currentVertexElement * 1]), 1, 1 * sizeof(float));
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

                if (indices.size() > 0)
                {
                	uint32_t size = sizeof(int32_t) * subMesh->getNumberIndices();

                    auto indicesBinaryBuffer = binaryBufferCreate(reinterpret_cast<const uint8_t*>(&indices[0]), size);

                    if (!indicesBinaryBuffer.get() || indicesBinaryBuffer->getSize() != size)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create indices binary buffer");

                        return VK_FALSE;
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
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh incomplete");

                    return VK_FALSE;
                }

                //

                if (subMesh->getBSDFMaterial().get() && sceneFactory->getSceneRenderFactory().get())
                {
                	if (!sceneFactory->getSceneRenderFactory()->prepareBSDFMaterial(sceneManager, subMesh))
                	{
                		return VK_FALSE;
                	}
                }

                //

                sceneManager->addSubMesh(subMesh);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }

            vertex.clear();
            normal.clear();
            bitangent.clear();
            tangent.clear();
            texcoord.clear();

            boneIndices0.clear();
            boneIndices1.clear();
            boneWeights0.clear();
            boneWeights1.clear();
            numberBones.clear();

            indices.clear();
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 sceneLoadMeshes(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[6];

    auto mesh = IMeshSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "submesh_library"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadSubMeshes(directory, sdata, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load sub meshes: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            mesh = sceneFactory->createMesh(sceneManager);

            if (!mesh.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Mesh not created: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            mesh->setName(sdata);

            sceneManager->addMesh(mesh);
        }
        else if (parseIsToken(buffer, "submesh"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (mesh.get())
            {
                const auto& subMesh = sceneManager->useSubMesh(sdata);

                if (!subMesh.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh not found: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                mesh->addSubMesh(subMesh);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "displace"))
        {
            if (!parseVec2(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (mesh.get())
            {
                mesh->setDisplace(glm::vec2(fdata[0], fdata[1]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No mesh");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "aabb"))
        {
            if (!parseVec6(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (mesh.get())
            {
                mesh->setAABB(Aabb(glm::vec3(fdata[0], fdata[1], fdata[2]), glm::vec3(fdata[3], fdata[4], fdata[5])));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No mesh");

                return VK_FALSE;
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 sceneLoadChannels(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[6];

    auto channel = IChannelSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "name"))
        {
        	if (channel.get() && VKTS_CONVERT_BEZIER)
        	{
        		auto optimizedChannel = sceneFactory->createChannel(sceneManager);

                if (!optimizedChannel.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Optimized channel not created: '%s'", channel->getName().c_str());

                    return VK_FALSE;
                }

                if (!interpolateConvert(optimizedChannel, channel, VKTS_CONVERT_SAMPLING))
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not optimize channel: '%s'", channel->getName().c_str());

                    return VK_FALSE;
                }

                sceneManager->removeChannel(channel);
                sceneManager->addChannel(optimizedChannel);
        	}

            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            channel = sceneFactory->createChannel(sceneManager);

            if (!channel.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Channel not created: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            channel->setName(sdata);

            sceneManager->addChannel(channel);
        }
        else if (parseIsToken(buffer, "target_transform"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (channel.get())
            {
                if (strncmp(sdata, "TRANSLATE", 9) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_TRANSLATE);
                }
                else if (strncmp(sdata, "ROTATE", 6) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_ROTATE);
                }
                else if (strncmp(sdata, "QUATERNION_ROTATE", 17) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_QUATERNION_ROTATE);
                }
                else if (strncmp(sdata, "SCALE", 5) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_SCALE);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown target transform: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No channel");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "target_element"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (channel.get())
            {
                if (strncmp(sdata, "X", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_X);
                }
                else if (strncmp(sdata, "Y", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_Y);
                }
                else if (strncmp(sdata, "Z", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_Z);
                }
                else if (strncmp(sdata, "W", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_W);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown target transform element: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No channel");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "keyframe"))
        {
            char token[VKTS_MAX_TOKEN_CHARS + 1];

            auto numberItems = sscanf(buffer, "%256s %f %f %256s %f %f %f %f", token, &fdata[0], &fdata[1], sdata, &fdata[2], &fdata[3], &fdata[4], &fdata[5]);

            if (numberItems != 4 && numberItems != 8)
            {
                return VK_FALSE;
            }

            if (numberItems == 4)
            {
                fdata[2] = fdata[0] - 0.1f;
                fdata[3] = fdata[1];
                fdata[4] = fdata[0] + 0.1f;
                fdata[5] = fdata[1];
            }

            if (channel.get())
            {
                VkTsInterpolator interpolator = VKTS_INTERPOLATOR_CONSTANT;

                if (strncmp(sdata, "CONSTANT", 8) == 0)
                {
                    // Do nothing.
                }
                else if (strncmp(sdata, "LINEAR", 6) == 0)
                {
                    interpolator = VKTS_INTERPOLATOR_LINEAR;
                }
                else if (strncmp(sdata, "BEZIER", 6) == 0)
                {
                    interpolator = VKTS_INTERPOLATOR_BEZIER;
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown interpolator: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                channel->addEntry(fdata[0], fdata[1], glm::vec4(fdata[2], fdata[3], fdata[4], fdata[5]), interpolator);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No channel");

                return VK_FALSE;
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

	if (channel.get() && VKTS_CONVERT_BEZIER)
	{
		auto optimizedChannel = sceneFactory->createChannel(sceneManager);

        if (!optimizedChannel.get())
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Optimized channel not created: '%s'", channel->getName().c_str());

            return VK_FALSE;
        }

        if (!interpolateConvert(optimizedChannel, channel, VKTS_CONVERT_SAMPLING))
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not optimize channel: '%s'", channel->getName().c_str());

            return VK_FALSE;
        }

        sceneManager->removeChannel(channel);
        sceneManager->addChannel(optimizedChannel);
	}

    return VK_TRUE;
}

static VkBool32 sceneLoadAnimations(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[1];

    auto animation = IAnimationSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "channel_library"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadChannels(directory, sdata, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load channels: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            animation = sceneFactory->createAnimation(sceneManager);

            if (!animation.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation not created: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            animation->setName(sdata);

            sceneManager->addAnimation(animation);
        }
        else if (parseIsToken(buffer, "start"))
        {
            if (!parseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                animation->setStart(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "stop"))
        {
            if (!parseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                animation->setStop(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "marker"))
        {
            if (!parseStringFloat(buffer, sdata, VKTS_MAX_TOKEN_CHARS, fdata))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                auto currentMarker = sceneFactory->createMarker(sceneManager);

                if (!currentMarker.get())
                {
                	return VK_FALSE;
                }

                currentMarker->setName(sdata);
                currentMarker->setTime(fdata[0]);

                animation->addMarker(currentMarker);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "channel"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                const auto& channel = sceneManager->useChannel(sdata);

                if (!channel.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Channel not found: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                animation->addChannel(channel);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 sceneLoadCameras(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata;

    auto camera = ICameraSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            camera = sceneFactory->createCamera(sceneManager);

            if (!camera.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Camera not created: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            camera->setName(sdata);

            //

            sceneManager->addCamera(camera);
        }
        else if (parseIsToken(buffer, "type"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (camera.get())
            {
                if (strcmp(sdata, "Perspective") == 0)
                {
                	camera->setCameraType(PerspectiveCamera);
                }
                else if (strcmp(sdata, "Orhtogonal") == 0)
                {
                	camera->setCameraType(OrthogonalCamera);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid camera type '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No Camera");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "znear"))
        {
            if (!parseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get())
            {
            	camera->setZNear(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "zfar"))
        {
            if (!parseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get())
            {
            	camera->setZFar(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "fovy"))
        {
            if (!parseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get() && camera->getCameraType() == PerspectiveCamera)
            {
            	camera->setFovY(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera or invalid type");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "ortho_scale"))
        {
            if (!parseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get() && camera->getCameraType() == OrthogonalCamera)
            {
            	camera->setOrthoScale(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera or invalid type");

                return VK_FALSE;
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}


static VkBool32 sceneLoadLights(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];

    auto light = ILightSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            light = sceneFactory->createLight(sceneManager);

            if (!light.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Light not created: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            light->setName(sdata);

            sceneManager->addLight(light);
        }
        else if (parseIsToken(buffer, "type"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
                if (strcmp(sdata, "Point") == 0)
                {
                	light->setLightType(PointLight);
                }
                else if (strcmp(sdata, "Directional") == 0)
                {
                	light->setLightType(DirectionalLight);
                }
                else if (strcmp(sdata, "Spot") == 0)
                {
                	light->setLightType(SpotLight);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid  light type '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "falloff"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
                if (strcmp(sdata, "Quadratic") == 0 && light->getLightType() != DirectionalLight)
                {
                	light->setFalloffType(QuadraticFalloff);
                }
                else if (strcmp(sdata, "Linear") == 0 && light->getLightType() != DirectionalLight)
                {
                	light->setFalloffType(LinearFalloff);
                }
                else if (strcmp(sdata, "Constant") == 0)
                {
                	light->setFalloffType(ConstantFalloff);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid fall off type '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "strength"))
        {
            if (!parseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
            	light->setStrength(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "outer_angle"))
        {
            if (!parseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (light.get() && light->getLightType() == SpotLight)
            {
            	light->setOuterAngle(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No or invalid light");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "inner_angle"))
        {
            if (!parseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (light.get() && light->getLightType() == SpotLight)
            {
            	light->setInnerAngle(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No or invalid light");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "color"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
            	light->setColor(glm::vec3(fdata[0], fdata[1], fdata[2]));

            	//

            	sceneManager->addLight(light);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 sceneLoadObjects(const char* directory, const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!directory || !filename || !sceneManager.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata0[VKTS_MAX_TOKEN_CHARS + 1];
    char sdata1[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];
    VkBool32 bdata[3];
    int32_t idata;
    uint32_t uidata;
    glm::mat4 mat4;

    auto object = IObjectSP();

    auto node = INodeSP();

    IConstraintSP constraint;
    ICopyConstraint* copyConstraint = nullptr;
    ILimitConstraint* limitConstraint = nullptr;

    SmartPointerMap<std::string, INodeSP> allNodes;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "mesh_library"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadMeshes(directory, sdata0, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load meshes: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "animation_library"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadAnimations(directory, sdata0, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load animations: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "camera_library"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadCameras(directory, sdata0, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load cameras: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "light_library"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (!sceneLoadLights(directory, sdata0, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load lights: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            object = sceneFactory->createObject(sceneManager);

            if (!object.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Object not created: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                return VK_FALSE;
            }

            object->setName(sdata0);

            sceneManager->addObject(object);
        }
        else if (parseIsToken(buffer, "node"))
        {
            if (!parseStringTuple(buffer, sdata0, VKTS_MAX_TOKEN_CHARS, sdata1, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (object.get())
            {
                node = sceneFactory->createNode(sceneManager);

                if (!node.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Node not created: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                node->setName(sdata0);

                //

                allNodes[sdata0] = node;

                if (strncmp(sdata1, "-", 1) == 0)
                {
                    object->setRootNode(node);
                }
                else
                {
                    auto parentNodeIndex = allNodes.find(sdata1);

                    if (parentNodeIndex != allNodes.size() && allNodes.valueAt(parentNodeIndex).get())
                    {
                        node->setParentNode(allNodes.valueAt(parentNodeIndex));

                        allNodes.valueAt(parentNodeIndex)->addChildNode(node);
                    }
                    else
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Parent node not found: '%s'", sdata1);

                        return VK_FALSE;
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "layers"))
        {
            if (!parseUIntHex(buffer, &uidata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setLayers(uidata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "constraint"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
            	constraint = IConstraintSP();

            	if (strcmp(sdata0, "COPY_LOCATION") == 0)
            	{
            		constraint = sceneFactory->createCopyConstraint(sceneManager, COPY_LOCATION);
            		if (constraint.get())
            		{
            			copyConstraint = static_cast<ICopyConstraint*>(constraint.get());
            		}
            	}
            	else if (strcmp(sdata0, "COPY_ROTATION") == 0)
            	{
            		constraint = sceneFactory->createCopyConstraint(sceneManager, COPY_ROTATION);
            		if (constraint.get())
            		{
            			copyConstraint = static_cast<ICopyConstraint*>(constraint.get());
            		}
            	}
            	else if (strcmp(sdata0, "COPY_SCALE") == 0)
            	{
            		constraint = sceneFactory->createCopyConstraint(sceneManager, COPY_SCALE);
            		if (constraint.get())
            		{
            			copyConstraint = static_cast<ICopyConstraint*>(constraint.get());
            		}
            	}
            	else if (strcmp(sdata0, "LIMIT_LOCATION") == 0)
            	{
            		constraint = sceneFactory->createLimitConstraint(sceneManager, LIMIT_LOCATION);
            		if (constraint.get())
            		{
            			limitConstraint = static_cast<ILimitConstraint*>(constraint.get());
            		}
            	}
            	else if (strcmp(sdata0, "LIMIT_ROTATION") == 0)
            	{
            		constraint = sceneFactory->createLimitConstraint(sceneManager, LIMIT_ROTATION);
            		if (constraint.get())
            		{
            			limitConstraint = static_cast<ILimitConstraint*>(constraint.get());
            		}
            	}
            	else if (strcmp(sdata0, "LIMIT_SCALE") == 0)
            	{
            		constraint = sceneFactory->createLimitConstraint(sceneManager, LIMIT_SCALE);
            		if (constraint.get())
            		{
            			limitConstraint = static_cast<ILimitConstraint*>(constraint.get());
            		}
            	}
				else
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unsupported constraint");

					return VK_FALSE;
				}

            	if (constraint.get())
            	{
            		node->addConstraint(constraint);
            	}
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "target"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	INodeSP targetNode;

            	for (uint32_t i = 0; i < sceneManager->getAllObjects().values().size(); i++)
            	{
            		if (!sceneManager->getAllObjects().values()[i]->getRootNode().get())
            		{
            			continue;
            		}

            		targetNode = sceneManager->getAllObjects().values()[i]->getRootNode()->findNodeRecursiveFromRoot(sdata0);

            		if (targetNode.get())
            		{
            			break;
            		}
            	}

                if (!targetNode.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No target node found");

                    return VK_FALSE;
                }

                copyConstraint->setTarget(targetNode);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "use"))
        {
            if (!parseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setUse(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "invert"))
        {
            if (!parseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setInvert(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "use_offset"))
        {
            if (!parseBool(buffer, &bdata[0]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setOffset(bdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "use_min"))
        {
            if (!parseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setUseMin(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "use_max"))
        {
            if (!parseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setUseMax(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "min"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setMin(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "max"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setMax(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "influence"))
        {
            if (!parseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setInfluence(fdata[0]);
            }
            else if (node.get() && limitConstraint)
            {
            	limitConstraint->setInfluence(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or constraint");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "translate"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setTranslate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "rotate"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setRotate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "scale"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setScale(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "jointIndex"))
        {
            if (!parseInt(buffer, &idata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setJointIndex(idata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "joints"))
        {
            if (!parseInt(buffer, &idata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
            	if (!sceneFactory->getSceneRenderFactory()->prepareJointsUniformBuffer(sceneManager, node, idata))
            	{
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create joint uniform buffer for node: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
            	}
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "bind_translate"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setBindTranslate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "bind_rotate"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setBindRotate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "bind_scale"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setBindScale(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "mesh"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
            	if (!sceneFactory->getSceneRenderFactory()->prepareTransformUniformBuffer(sceneManager, node))
            	{
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create uniform buffer for node: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
            	}

            	//

                const auto& mesh = sceneManager->useMesh(sdata0);

                if (!mesh.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Mesh not found: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                node->addMesh(mesh);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "camera"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                const auto& camera = sceneManager->useCamera(sdata0);

                if (!camera.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Camera not found: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                node->addCamera(camera);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "light"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                const auto& light = sceneManager->useLight(sdata0);

                if (!light.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Light not found: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                node->addLight(light);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (parseIsToken(buffer, "animation"))
        {
            if (!parseString(buffer, sdata0, VKTS_MAX_TOKEN_CHARS))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                const auto& animation = sceneManager->useAnimation(sdata0);

                if (!animation.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation not found: '%s'", sdata0, VKTS_MAX_TOKEN_CHARS);

                    return VK_FALSE;
                }

                node->addAnimation(animation);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

ISceneSP VKTS_APIENTRY sceneLoad(const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory, const VkBool32 freeHostMemory)
{
    if (!filename || !sceneManager.get() || !sceneFactory.get())
    {
        return ISceneSP();
    }

    auto textBuffer = fileLoadText(filename);

    if (!textBuffer.get())
    {
        return ISceneSP();
    }

    char directory[VKTS_MAX_BUFFER_CHARS] = "";

    fileGetDirectory(directory, filename);

    auto scene = sceneFactory->createScene(sceneManager);

    if (!scene.get())
    {
        return ISceneSP();
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];

    auto object = IObjectSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (parseSkipBuffer(buffer))
        {
            continue;
        }

        if (parseIsToken(buffer, "scene_name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return ISceneSP();
            }

            scene->setName(std::string(sdata));
        }
        else if (parseIsToken(buffer, "object_library"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return ISceneSP();
            }

            if (!sceneLoadObjects(directory, sdata, sceneManager, sceneFactory))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load objects: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return ISceneSP();
            }
        }
        else if (parseIsToken(buffer, "object"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return ISceneSP();
            }

            object = sceneManager->useObject(sdata);

            if (!object.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Object not found: '%s'", sdata, VKTS_MAX_TOKEN_CHARS);

                return ISceneSP();
            }

            scene->addObject(object);
        }
        else if (parseIsToken(buffer, "name"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setName(std::string(sdata));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (parseIsToken(buffer, "translate"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setTranslate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (parseIsToken(buffer, "rotate"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setRotate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (parseIsToken(buffer, "scale"))
        {
            if (!parseVec3(buffer, fdata))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setScale(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (parseIsToken(buffer, "environment"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return ISceneSP();
            }

            // Nothing for now.
        }
        else if (parseIsToken(buffer, "environment_strength"))
        {
            if (!parseFloat(buffer, &fdata[0]))
            {
                return ISceneSP();
            }

            scene->setEnvironmentStrength(fdata[0]);
        }
        else if (parseIsToken(buffer, "texture"))
        {
            if (!parseString(buffer, sdata, VKTS_MAX_TOKEN_CHARS))
            {
                return ISceneSP();
            }

            auto textureObject = sceneManager->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return ISceneSP();
            }

            if (textureObject->getImageObject()->getImageView()->getViewType() != VK_IMAGE_VIEW_TYPE_CUBE || textureObject->getImageObject()->getImageView()->getLayerCount() != 6)
            {
            	return ISceneSP();
            }

            scene->setEnvironment(textureObject);

            // Setting the maximum luminance
            if (textureObject->getImageObject()->getImageData()->isSFLOAT())
            {
            	scene->setMaxLuminance(textureObject->getImageObject()->getImageData()->getMaxLuminance());
            }

            //

            textureObject = sceneManager->useTextureObject(std::string(sdata) + "_LAMBERT");

            if (textureObject.get())
            {
                scene->setDiffuseEnvironment(textureObject);

                //

                textureObject = sceneManager->useTextureObject(std::string(sdata) + "_COOKTORRANCE");

                if (!textureObject.get())
                {
                	return ISceneSP();
                }

                scene->setSpecularEnvironment(textureObject);

                //

                textureObject = sceneManager->useTextureObject("BSDF_LUT_" + std::to_string(VKTS_BSDF_LENGTH) + "_" + std::to_string(VKTS_BSDF_SAMPLES));

                if (!textureObject.get())
                {
                	return ISceneSP();
                }

                scene->setLut(textureObject);
            }
        }
        else
        {
            parseUnknownBuffer(buffer);
        }
    }

    // Gather all cameras and add to scene.

    for (uint32_t i = 0; i < sceneManager->getAllCameras().values().size(); i++)
    {
    	scene->addCamera(sceneManager->getAllCameras().valueAt(i));
    }

    // Gather all lights and add to scene.

    for (uint32_t i = 0; i < sceneManager->getAllLights().values().size(); i++)
    {
    	scene->addLight(sceneManager->getAllLights().valueAt(i));
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
