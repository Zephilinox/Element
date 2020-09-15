#include "error.hpp"

using namespace element;

element_result error::get_result() const
{
    return code;
}

const std::string& error::get_message() const
{
    return message;
}

element_log_message error::get_log_message() const
{
    element_log_message msg;
    msg.filename = identity.source_info.filename;
    msg.line = identity.source_info.line;
    msg.character = identity.source_info.character_start;
    msg.message = message.c_str();
    msg.message_length = message.length();
    msg.length = identity.source_info.character_end - identity.source_info.character_start;
    msg.message_code = code;
    msg.related_log_message = nullptr;
    msg.stage = ELEMENT_STAGE_COMPILER;
    msg.line_in_source = identity.source_info.line_in_source ? identity.source_info.line_in_source->c_str() : nullptr;
    return msg;
}

element_result error::log_once(const element_log_ctx* logger) const
{
    if (!logged)
    {
        logged = true;
        logger->log(get_log_message());
    }

    return code;
}