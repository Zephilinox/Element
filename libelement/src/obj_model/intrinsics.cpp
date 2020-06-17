#include "intrinsics.hpp"

#include "functions.hpp"
#include "types.hpp"
#include "intermediaries.hpp"
#include "etree/expressions.hpp"

DEFINE_TYPE_ID(element::intrinsic_nullary, 1U << 0);
DEFINE_TYPE_ID(element::intrinsic_unary, 1U << 1);
DEFINE_TYPE_ID(element::intrinsic_binary, 1U << 2);
//DEFINE_TYPE_ID(element::intrinsic_if, 1U << 3);

namespace element
{
    const std::unordered_map<std::string, const std::shared_ptr<const intrinsic>> intrinsic::intrinsic_map
    {
        //types
        { "Num", nullptr },
        { "Bool", nullptr },
        { "List", nullptr },
        { "Tuple", nullptr },

        //functions
        { "Num.add", std::make_shared<intrinsic_binary>(element_binary_op::add) },
        { "Num.sub", std::make_shared<intrinsic_binary>(element_binary_op::sub) },
        { "Num.mul", std::make_shared<intrinsic_binary>(element_binary_op::mul) },
        { "Num.div", std::make_shared<intrinsic_binary>(element_binary_op::div) },

        { "Num.pow", std::make_shared<intrinsic_binary>(element_binary_op::pow) },
        { "Num.rem", std::make_shared<intrinsic_binary>(element_binary_op::rem) },

        { "Num.min", std::make_shared<intrinsic_binary>(element_binary_op::min) },
        { "Num.max", std::make_shared<intrinsic_binary>(element_binary_op::max) },

        { "Num.abs", std::make_shared<intrinsic_unary>(element_unary_op::abs) },
        { "Num.ceil", std::make_shared<intrinsic_unary>(element_unary_op::ceil) },
        { "Num.floor", std::make_shared<intrinsic_unary>(element_unary_op::floor) },

        { "Num.sin", std::make_shared<intrinsic_unary>(element_unary_op::sin) },
        { "Num.cos", std::make_shared<intrinsic_unary>(element_unary_op::cos) },
        { "Num.tan", std::make_shared<intrinsic_unary>(element_unary_op::tan) },

        { "Num.asin", std::make_shared<intrinsic_unary>(element_unary_op::asin) },
        { "Num.acos", std::make_shared<intrinsic_unary>(element_unary_op::acos) },
        { "Num.atan", std::make_shared<intrinsic_unary>(element_unary_op::atan) },

        { "Num.atan2", std::make_shared<intrinsic_binary>(element_binary_op::atan2) },

        { "Num.ln", std::make_shared<intrinsic_unary>(element_unary_op::ln) },
        { "Num.log", std::make_shared<intrinsic_binary>(element_binary_op::log) },

        { "Num.NaN", std::make_shared<intrinsic_nullary>(element_nullary_op::nan) },
        { "Num.PositiveInfinity", std::make_shared<intrinsic_nullary>(element_nullary_op::positive_infinity) },
        { "Num.NegativeInfinity", std::make_shared<intrinsic_nullary>(element_nullary_op::negative_infinity) },

        { "Num.eq", std::make_shared<intrinsic_binary>(element_binary_op::eq, type::boolean, type::num, type::num) },
        { "Num.neq", std::make_shared<intrinsic_binary>(element_binary_op::neq, type::boolean, type::num, type::num) },
        { "Num.lt", std::make_shared<intrinsic_binary>(element_binary_op::lt, type::boolean, type::num, type::num) },
        { "Num.leq", std::make_shared<intrinsic_binary>(element_binary_op::leq, type::boolean, type::num, type::num) },
        { "Num.gt", std::make_shared<intrinsic_binary>(element_binary_op::gt, type::boolean, type::num, type::num) },
        { "Num.geq", std::make_shared<intrinsic_binary>(element_binary_op::geq, type::boolean, type::num, type::num) },

        { "Bool.if", nullptr },
        { "Bool.not", std::make_shared<intrinsic_unary>(element_unary_op::not, type::boolean, type::boolean) },
        { "Bool.and", std::make_shared<intrinsic_binary>(element_binary_op::and, type::boolean, type::boolean) },
        { "Bool.or", std::make_shared<intrinsic_binary>(element_binary_op::or , type::boolean, type::boolean) },

        { "True", std::make_shared<intrinsic_nullary>(element_nullary_op::true_value, type::boolean) },
        { "False", std::make_shared<intrinsic_nullary>(element_nullary_op::false_value, type::boolean) },

        { "list", nullptr },
        { "List.fold", nullptr },

        { "memberwise", nullptr },
        { "for", nullptr },
        //{ "persist", nullptr }, //todo: unlikely to be part of the language

        //constraints
        { "Any", nullptr },
    };

    std::shared_ptr<const intrinsic> intrinsic::get_intrinsic(const declaration& declaration)
    {
        const auto location = declaration.location();

        const auto it = intrinsic_map.find(location);
        if (it != intrinsic_map.end())
            return it->second;

        return nullptr;
    }

    intrinsic::intrinsic(const element_type_id id)
        : rtti_type(id)
    {
    }

    intrinsic_function::intrinsic_function(const element_type_id id, type_const_shared_ptr return_type)
        : intrinsic(id), return_type(std::move(return_type))
    {
    }

    intrinsic_nullary::intrinsic_nullary(const element_nullary_op operation, type_const_shared_ptr return_type = type::num):
        intrinsic_function(type_id, std::move(return_type))
        , operation(operation)
    {
    }

    std::shared_ptr<object> intrinsic_nullary::call(const compilation_context& context, std::vector<std::shared_ptr<compiled_expression>> args) const
    {
        auto result = std::make_shared<compiled_expression>();
        result->creator = nullptr;

        result->expression_tree = std::make_shared<element_expression_nullary>(
            operation);

        return result;
    }

    intrinsic_unary::intrinsic_unary(element_unary_op operation, 
                                     type_const_shared_ptr return_type = type::num, 
                                     type_const_shared_ptr argument_type = type::num)
        : intrinsic_function(type_id, std::move(return_type))
        , operation(operation)
        , argument_type{std::move(argument_type)}
    {
    }

    std::shared_ptr<object> intrinsic_unary::call(const compilation_context& context, std::vector<std::shared_ptr<compiled_expression>> args) const
    {
        auto result = std::make_shared<compiled_expression>();
        result->creator = nullptr;

        result->expression_tree = std::make_shared<element_expression_unary>(
            operation,
            args[0]->expression_tree);

        return result;
    }

    intrinsic_binary::intrinsic_binary(const element_binary_op operation, type_const_shared_ptr return_type = type::num,
                                       type_const_shared_ptr first_argument_type = type::num,
                                       type_const_shared_ptr second_argument_type = type::num):
        intrinsic_function(type_id, std::move(return_type))
        , operation(operation)
        , first_argument_type{std::move(first_argument_type)}
        , second_argument_type{std::move(second_argument_type)}
    {
    }

    std::shared_ptr<object> intrinsic_binary::call(const compilation_context& context, std::vector<std::shared_ptr<compiled_expression>> args) const
    {
        auto result = std::make_shared<compiled_expression>();
        result->creator = nullptr;

        result->expression_tree = std::make_shared<element_expression_binary>(
            operation,
            args[0]->expression_tree,
            args[1]->expression_tree);

        return result;
    }
}
