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

#ifndef VKTS_VKTS_CORE_HPP_
#define VKTS_VKTS_CORE_HPP_

/**
 *
 * C++11 Libraries.
 *
 */

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <cmath>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

/**
 *
 * GLM Library.
 * https://www.opengl.org/sdk/libs/GLM/
 *
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

 /**
 * Validation Helpers.
 */

#include <vkts/core/validation/ValidationHelpers.hpp>

/**
 * OS platform.
 */

#include <vkts/core/fn_os_platform.hpp>

/**
 *
 * Vulkan Library.
 * https://www.khronos.org/vulkan
 *
 */

#include <vulkan/vulkan.h>

/**
 *
 * VKTS Start.
 *
 */

/**
 * API platform.
 */

#include <vkts/core/fn_api_platform.hpp>

//

/**
 * Defines.
 */

#define VKTS_MAX_LOG_CHARS 2048

#define VKTS_MAX_BUFFER_CHARS 2048
#define VKTS_MAX_TOKEN_CHARS 256

/**
 * Types.
 */

typedef enum VkTsSearch_
{
    VKTS_SEARCH_ABSOLUTE = 0, VKTS_SEARCH_RELATVE = 1
} VkTsSearch;

typedef struct VkTsDynamicOffset_
{
    uint32_t offset;
    uint32_t stride;
} VkTsDynamicOffset;

/**
 * Interface.
 */

#include <vkts/core/interface/ICloneable.hpp>
#include <vkts/core/interface/IDestroyable.hpp>
#include <vkts/core/interface/IResetable.hpp>
#include <vkts/core/interface/IUpdateable.hpp>

/**
 * Container.
 */

#include <vkts/core/container/List.hpp>
#include <vkts/core/container/SmartPointerList.hpp>
#include <vkts/core/container/SmartPointerVector.hpp>
#include <vkts/core/container/ThreadsafeQueue.hpp>
#include <vkts/core/container/Vector.hpp>

#include <vkts/core/container/Map.hpp>
#include <vkts/core/container/SmartPointerMap.hpp>

/**
 * Log.
 */

#include <vkts/core/log/fn_log.hpp>

/**
 * Parameter.
 */

#include <vkts/core/parameter/fn_parameter.hpp>

/**
 * Parse.
 */

#include <vkts/core/parse/fn_parse.hpp>

/**
 * Time.
 */

#include <vkts/core/time/fn_time.hpp>

/**
 * Processor.
 */

#include <vkts/core/processor/fn_processor.hpp>

/**
 * Profile.
 */

#include <vkts/core/profile/fn_profile.hpp>

/**
 * Binary buffer.
 */

#include <vkts/core/binary_buffer/IBinaryBuffer.hpp>

#include <vkts/core/binary_buffer/fn_binary_buffer.hpp>

/**
 * Text buffer.
 */

#include <vkts/core/text_buffer/ITextBuffer.hpp>

#include <vkts/core/text_buffer/fn_text_buffer.hpp>

/**
 * File.
 */

#include <vkts/core/file/fn_file.hpp>

/**
 * BASE64.
 */

#include <vkts/core/base64/fn_base64.hpp>

/**
 * JSON.
 */

#include <vkts/core/json/fn_json_tokens.hpp>

#include <vkts/core/json/JsonVisitor.hpp>

#include <vkts/core/json/JSONvalue.hpp>
#include <vkts/core/json/JSONnumber.hpp>

#include <vkts/core/json/JSONnull.hpp>
#include <vkts/core/json/JSONtrue.hpp>
#include <vkts/core/json/JSONfalse.hpp>
#include <vkts/core/json/JSONstring.hpp>
#include <vkts/core/json/JSONobject.hpp>
#include <vkts/core/json/JSONarray.hpp>
#include <vkts/core/json/JSONfloat.hpp>
#include <vkts/core/json/JSONinteger.hpp>

#include <vkts/core/json/fn_json.hpp>

#endif /* VKTS_VKTS_CORE_HPP_ */
