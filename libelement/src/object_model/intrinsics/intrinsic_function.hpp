#pragma once

//SELF
#include "intrinsic.hpp"
#include "object_model/compilation_context.hpp"
#include "etree/evaluator.hpp"

namespace element
{
    class intrinsic_function : public intrinsic
    {
    public:
        intrinsic_function(element_type_id id, type_const_ptr return_type);
        [[nodiscard]] type_const_ptr get_type() const final { return return_type; };
    
    protected:
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_ptr return_type;
    };

    static std::shared_ptr<const element_expression> evaluate(const compilation_context& context, std::shared_ptr<const element_expression> expr)
    {
        std::vector<element_value> outputs = { 0 };
        const auto result = element_evaluate(*context.interpreter, expr, {}, outputs, {});
        if (result != ELEMENT_OK)
            return expr;

        auto new_expr = std::make_unique<element_expression_constant>(outputs[0]);
        new_expr->actual_type = expr->actual_type;
        return new_expr;
    }
}