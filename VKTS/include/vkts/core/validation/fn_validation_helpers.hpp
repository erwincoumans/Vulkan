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

/**
 * Additional authors:
 * skesgin https://github.com/skesgin
 */

#ifndef VKTS_FN_VALIDATION_HELPERS_HPP_
#define VKTS_FN_VALIDATION_HELPERS_HPP_


#define VKTS_REFLECT_TOKEN(Token) Token


#define VKTS_VALIDATE_CONDITION_WITH_DEFAULT(Condition, Message, Default) if (VKTS_REFLECT_TOKEN(Condition)) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return VKTS_REFLECT_TOKEN(Default); }

#define VKTS_VALIDATE_CONDITION(Condition, Message) VKTS_VALIDATE_CONDITION_WITH_DEFAULT(Condition, Message, VK_FALSE)


#define VKTS_VALIDATE_SUCCESS_WITH_DEFAULT(Result, Message, Default) if (Result != VK_SUCCESS) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return VKTS_REFLECT_TOKEN(Default); }

#define VKTS_VALIDATE_SUCCESS(Result, Message) VKTS_VALIDATE_SUCCESS_WITH_DEFAULT(Result, Message, VK_FALSE)


#define VKTS_VALIDATE_INSTANCE_WITH_DEFAULT(Ptr, Message, Default) 	if (!VKTS_REFLECT_TOKEN(Ptr).get()) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return VKTS_REFLECT_TOKEN(Default); }

#define VKTS_VALIDATE_INSTANCE(Ptr, Message) VKTS_VALIDATE_INSTANCE_WITH_DEFAULT(Ptr, Message, VK_FALSE)


#endif /* VKTS_FN_VALIDATION_HELPERS_HPP_ */
