#ifndef VKTS_VALIDATION_HELPERS_HPP_
#define VKTS_VALIDATION_HELPERS_HPP_


#define VKTS_REFLECT_TOKEN(Token) Token


#define VKTS_VALIDATE_CONDITION_WITH_DEFAULT(Condition, Message, Default) if (VKTS_REFLECT_TOKEN(Condition)) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return VKTS_REFLECT_TOKEN(Default); }

#define VKTS_VALIDATE_CONDITION(Condition, Message) VKTS_VALIDATE_CONDITION_WITH_DEFAULT(Condition, Message, VK_FALSE)


#define VKTS_VALIDATE_SUCCESS_WITH_DEFAULT(Result, Message, Default) if (Result != VK_SUCCESS) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return VKTS_REFLECT_TOKEN(Default); }

#define VKTS_VALIDATE_SUCCESS(Result, Message) VKTS_VALIDATE_SUCCESS_WITH_DEFAULT(Result, Message, VK_FALSE)


#define VKTS_VALIDATE_INSTANCE_WITH_DEFAULT(Ptr, Message, Default) 	if (!VKTS_REFLECT_TOKEN(Ptr).get()) { vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, #Message); return VKTS_REFLECT_TOKEN(Default); }

#define VKTS_VALIDATE_INSTANCE(Ptr, Message) VKTS_VALIDATE_INSTANCE_WITH_DEFAULT(Ptr, Message, VK_FALSE)


#endif /* VKTS_VALIDATION_HELPERS_HPP_ */