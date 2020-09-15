#include "call_stack.hpp"

//LIBS
#include <fmt/format.h>

//SELF
#include "declarations/declaration.hpp"
#include "compilation_context.hpp"
#include "error_map.hpp"

using namespace element;

call_stack::frame& call_stack::push(const identity* function_identity, std::vector<object_const_shared_ptr> compiled_arguments)
{
    return frames.emplace_back(frame{ function_identity, std::move(compiled_arguments) });
}

void call_stack::pop()
{
    frames.pop_back();
}

bool call_stack::is_recursive(const declaration* declaration) const
{
    for (auto it = frames.rbegin(); it != frames.rend(); ++it)
    {
        if (it->identity == &declaration->identity)
            return !declaration->is_intrinsic();
    }

    return false;
}

std::shared_ptr<error> call_stack::build_recursive_error(
    const declaration* decl,
    const compilation_context& context,
    const source_information& source_info)
{
    std::string trace;

    for (auto it = context.calls.frames.rbegin(); it < context.calls.frames.rend(); ++it)
    {
        auto& identity = it->identity;

        std::string params;
        for (unsigned i = 0; i < identity->inputs.size(); ++i)
        {
            const auto& input = identity->inputs[i];
            params += fmt::format("{}{} = {}",
                input.get_name(),
                input.has_annotation() ? ":" + input.get_annotation()->to_string() : "",
                it->compiled_arguments[i]->typeof_info());

            if (i != identity->inputs.size() - 1)
                params += ", ";
        }

        trace += fmt::format("{}:{} at {}({})",
            identity->source_info.filename,
            identity->source_info.line,
            decl->typeof_info(),
            params);

        if (identity == &decl->identity)
            trace += " <-- here";

        if (it != context.calls.frames.rend() - 1)
            trace += "\n";
    }

    return build_error_and_log(context, source_info, error_message_code::recursion_detected, decl->typeof_info(), trace);
}