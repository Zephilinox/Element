#include "intrinsic_constructor_bool.hpp"

//SELF
#include "object_model/constraints/type.hpp"

using namespace element;

intrinsic_constructor_bool::intrinsic_constructor_bool()
    : intrinsic_function(type_id, type::boolean.get())
{
}

object_const_shared_ptr intrinsic_constructor_bool::call(
    const compilation_context& context,
    std::vector<object_const_shared_ptr> compiled_args,
    const source_information& source_info) const
{
    auto& true_declaration = *context.get_global_scope()->find(identifier("True"), false);
    auto& false_declaration = *context.get_global_scope()->find(identifier("False"), false);

    const auto true_expression = get_intrinsic(context.interpreter, true_declaration.identity)->compile(context, source_info);
    const auto false_expression = get_intrinsic(context.interpreter, false_declaration.identity)->compile(context, source_info);

    auto expr = std::dynamic_pointer_cast<const element_expression>(compiled_args[0]);
    
    assert(expr); //todo: I think this is accurate
    assert(std::dynamic_pointer_cast<const element_expression>(true_expression));
    assert(std::dynamic_pointer_cast<const element_expression>(false_expression));

    auto new_expr = std::make_unique<element_expression_if>(
        expr,
        std::dynamic_pointer_cast<const element_expression>(true_expression),
        std::dynamic_pointer_cast<const element_expression>(false_expression));

    return evaluate(context, std::move(new_expr));
}