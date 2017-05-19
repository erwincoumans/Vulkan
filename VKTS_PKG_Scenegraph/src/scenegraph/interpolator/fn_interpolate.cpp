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

#define VKTS_BEZIER_TOLERANCE 0.1f
#define VKTS_BEZIER_LOOPS 10

namespace vkts
{

static float interpolateLinear(const uint32_t currentIndex, const float key, const IChannelSP& channel)
{
    float beforeKey = channel->getKeys()[currentIndex];
    float beforeValue = channel->getValues()[currentIndex];

    float afterKey = channel->getKeys()[currentIndex + 1];
    float afterValue = channel->getValues()[currentIndex + 1];

    float deltaKey = afterKey - beforeKey;
    if (deltaKey == 0.0f)
    {
        return afterValue;
    }

    return (afterValue - beforeValue) * (key - beforeKey) / deltaKey + beforeValue;
}

// see https://en.wikipedia.org/wiki/Cubic_Hermite_spline
static float interpolateCatmullRom(const uint32_t currentIndex, const float key, const IChannelSP& channel)
{
    float K0 = channel->getKeys()[currentIndex];
    float V0 = channel->getValues()[currentIndex];

    float K1 = channel->getKeys()[currentIndex + 1];
    float V1 = channel->getValues()[currentIndex + 1];

    float M0 = (channel->getValues()[currentIndex + 1] - channel->getValues()[currentIndex - 1]) / (channel->getKeys()[currentIndex + 1] - channel->getKeys()[currentIndex - 1]);
    float M1 = (channel->getValues()[currentIndex + 1 + 1] - channel->getValues()[currentIndex + 1 - 1]) / (channel->getKeys()[currentIndex + 1 + 1] - channel->getKeys()[currentIndex + 1 - 1]);

    //

    float t = (key - K0) / (K1 - K0);
    float t2 = t * t;
    float t3 = t2 * t;

    //

    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h01 = t3 - 2.0f * t2 + t;
    float h10 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    return h00 * V0 + h01 * M0 + h10 * V1 + h11 * M1;
}

static float interpolateCubicSpline(const uint32_t currentIndex, const float key, const IChannelSP& channel)
{
    float K0 = channel->getKeys()[currentIndex];
    float V0 = channel->getValues()[currentIndex];

    float K1 = channel->getKeys()[currentIndex + 1];
    float V1 = channel->getValues()[currentIndex + 1];

    float M0 = channel->getHandles()[currentIndex].y;
    float M1 = channel->getHandles()[currentIndex + 1].w;

    //

    float t = (key - K0) / (K1 - K0);
    float t2 = t * t;
    float t3 = t2 * t;

    //

    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h01 = t3 - 2.0f * t2 + t;
    float h10 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    return h00 * V0 + h01 * M0 + h10 * V1 + h11 * M1;
}

static float interpolateBezier(const uint32_t currentIndex, const float key, const IChannelSP& channel)
{
    float beforeKey = channel->getKeys()[currentIndex];
    float beforeValue = channel->getValues()[currentIndex];

    float afterKey = channel->getKeys()[currentIndex + 1];
    float afterValue = channel->getValues()[currentIndex + 1];

    float deltaKey = afterKey - beforeKey;
    if (deltaKey == 0.0f)
    {
        return afterValue;
    }

    float beforeRightHandleKey = channel->getHandles()[currentIndex].z;
    float afterLeftHandleKey = channel->getHandles()[currentIndex + 1].x;

    float t;
    float t2;
    float t3;
    float ot;
    float ot2;
    float ot3;

    float currentKey = key;

    bool doBinarySearch = true;

    int32_t counter = 0;

    do
    {
        t = (currentKey - beforeKey) / deltaKey;
        t2 = t * t;
        t3 = t2 * t;
        ot = 1.0f - t;
        ot2 = ot * ot;
        ot3 = ot2 * ot;

        float newKey = ot3 * beforeKey + 3.0f * ot2 * t * beforeRightHandleKey + 3.0f * ot * t2 * afterLeftHandleKey + t3 * afterKey;

        if (newKey > key + VKTS_BEZIER_TOLERANCE || newKey < key - VKTS_BEZIER_TOLERANCE)
        {
        	if ((newKey < currentKey && newKey > key) || (newKey > currentKey && newKey < key))
        	{
        		currentKey = (key + newKey) * 0.5f;
        	}
        	else if (newKey > currentKey)
        	{
        		currentKey -= VKTS_BEZIER_TOLERANCE;
        	}
        	else
        	{
        		currentKey += VKTS_BEZIER_TOLERANCE;
        	}
        }
        else
        {
        	currentKey = newKey;

            doBinarySearch = false;
        }

        counter++;
    }
    while (doBinarySearch && counter < VKTS_BEZIER_LOOPS);

    float beforeRightHandleValue = channel->getHandles()[currentIndex].w;
    float afterLeftHandleValue = channel->getHandles()[currentIndex + 1].y;

    t = (currentKey - beforeKey) / deltaKey;
    t2 = t * t;
    t3 = t2 * t;
    ot = 1.0f - t;
    ot2 = ot * ot;
    ot3 = ot2 * ot;

    return ot3 * beforeValue + 3.0f * ot2 * t * beforeRightHandleValue + 3.0f * ot * t2 * afterLeftHandleValue + t3 * afterValue;
}

float VKTS_APIENTRY interpolate(const float key, const IChannelSP& channel)
{
    if (!channel.get())
    {
        return 0.0f;
    }

    if (channel->getNumberEntries() == 0)
    {
        return 0.0f;
    }

    if (channel->getNumberEntries() == 1)
    {
        return channel->getValues()[0];
    }

    if (key <= channel->getKeys()[0])
    {
        return channel->getValues()[0];
    }

    auto lastIndex = channel->getNumberEntries() - 1;

    if (key >= channel->getKeys()[lastIndex])
    {
        return channel->getValues()[lastIndex];
    }

    // Key is in between the available keys now.

    uint32_t currentIndex = 0;
    while (currentIndex < channel->getNumberEntries())
    {
        if (key < channel->getKeys()[currentIndex])
        {
        	currentIndex--;

            break;
        }

        currentIndex++;
    }

    if (channel->getInterpolators()[currentIndex] == VKTS_INTERPOLATOR_CATMULLROMSPLINE)
    {
        if (currentIndex > 0 && currentIndex < channel->getNumberEntries() - 2)
        {
            return interpolateCatmullRom(currentIndex, key, channel);
        }

        return interpolateLinear(currentIndex, key, channel);
    }

    if (channel->getInterpolators()[currentIndex] == VKTS_INTERPOLATOR_CUBICSPLINE)
    {
        if (currentIndex > 0 && currentIndex < channel->getNumberEntries() - 2)
        {
            return interpolateCubicSpline(currentIndex, key, channel);
        }

        return interpolateLinear(currentIndex, key, channel);
    }

    if (channel->getInterpolators()[currentIndex] == VKTS_INTERPOLATOR_BEZIER)
    {
        if (channel->getInterpolators()[currentIndex + 1] == VKTS_INTERPOLATOR_BEZIER)
        {
            return interpolateBezier(currentIndex, key, channel);
        }

        return interpolateLinear(currentIndex, key, channel);
    }

    if (channel->getInterpolators()[currentIndex] == VKTS_INTERPOLATOR_LINEAR)
    {
        return interpolateLinear(currentIndex, key, channel);
    }

    // VKTS_INTERPOLATOR_CONSTANT
    return channel->getValues()[currentIndex];
}

VkBool32 VKTS_APIENTRY interpolateConvert(IChannelSP& converted, const IChannelSP& channel, const float sampleTime)
{
    if (!converted.get() || !channel.get() || sampleTime <= 0.0f)
    {
        return VK_FALSE;
    }

    converted->setName(channel->getName());
    converted->setTargetTransform(channel->getTargetTransform());
    converted->setTargetTransformElement(channel->getTargetTransformElement());

    for (uint32_t i = 0; i < channel->getNumberEntries(); i++)
    {
        if (channel->getInterpolators()[i] != VKTS_INTERPOLATOR_BEZIER)
        {
            converted->addEntry(channel->getKeys()[i], channel->getValues()[i], channel->getHandles()[i], channel->getInterpolators()[i]);
        }
        else
        {
            if (i == channel->getNumberEntries() - 1)
            {
                converted->addEntry(channel->getKeys()[i], channel->getValues()[i], channel->getHandles()[i], VKTS_INTERPOLATOR_LINEAR);
            }
            else
            {
                converted->addEntry(channel->getKeys()[i], channel->getValues()[i], channel->getHandles()[i], VKTS_INTERPOLATOR_LINEAR);

                auto currentKey = channel->getKeys()[i];
                auto nextKey = channel->getKeys()[i + 1];

                currentKey += sampleTime;

                while (currentKey < nextKey)
                {
                    auto currentValue = interpolate(currentKey, channel);

                    converted->addEntry(currentKey, currentValue, glm::vec4(currentKey - 0.1f, currentValue, currentKey + 0.1f, currentValue), VKTS_INTERPOLATOR_LINEAR);

                    currentKey += sampleTime;
                }
            }
        }
    }

    return VK_TRUE;
}

}
