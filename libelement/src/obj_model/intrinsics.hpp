#pragma once 

#include <memory>
#include <unordered_map>

#include "ast/fwd.hpp"
#include "fwd.hpp"
#include "object.hpp"
#include "declarations.hpp"
#include "typeutil.hpp"
#include "errors.hpp"
#include "log_errors.hpp"

namespace element
{
    class intrinsic : public object, public rtti_type<intrinsic>
    {
    public:
        intrinsic(element_type_id id);

        [[nodiscard]] std::string typeof_info() const override;
        [[nodiscard]] std::string to_code(int depth = 0) const override;

    public:
        template <typename T>
        static bool register_intrinsic(const element_interpreter_ctx* context, const element_ast* ast, const declaration& declaration)
        {
            if (!declaration.is_intrinsic()) 
            {
                const auto error = element::build_log_error(context->src_context.get(),
                    ast, log_error_message_code::intrinsic_not_implemented, declaration.name.value);
                context->logger->log(error);

                return false;
            }

            const auto location = declaration.location();
            const auto it = validation_func_map.find(location);
            if (it == validation_func_map.end())
            {
                const auto error = element::build_log_error(context->src_context.get(),
                    ast, log_error_message_code::intrinsic_not_implemented, declaration.name.value);
                context->logger->log(error);

                return false;
            }

            const auto validation_func = it->second;
            auto intrinsic = validation_func(&declaration);
            if (!intrinsic)
            {
                const auto error = element::build_log_error(context->src_context.get(),
                    ast, log_error_message_code::intrinsic_type_mismatch, declaration.name.value);
                context->logger->log(error);

                return false;
            }

            intrinsic_map.insert(std::unordered_map<const element::declaration*, const std::shared_ptr<const element::intrinsic>>::value_type { &declaration, std::move(intrinsic) });
            return true;
        }

        static std::shared_ptr<const intrinsic> get_intrinsic(const declaration& declaration)
        {
            const auto it = intrinsic_map.find(&declaration);
            if (it != intrinsic_map.end())
                return it->second;

            return nullptr;
        }

        static inline std::unordered_map<const declaration*, const std::shared_ptr<const intrinsic>> intrinsic_map;

    private:
        const static std::unordered_map<std::string, std::function<std::shared_ptr<const intrinsic>(const declaration*)>> validation_func_map;
    };

    class intrinsic_function : public intrinsic
    {
    protected:
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_ptr return_type;

    public:
        intrinsic_function(element_type_id id, type_const_ptr return_type);
    };

    class intrinsic_nullary final : public intrinsic_function
    {
    public:
        DECLARE_TYPE_ID();

    private:
        element_nullary_op operation;

    public:
        explicit intrinsic_nullary(element_nullary_op operation, type_const_ptr return_type);

        [[nodiscard]] std::shared_ptr<object> compile(const compilation_context& context, const source_information& source_info) const override;
        [[nodiscard]] element_nullary_op get_operation() const { return operation; }
    };

    class intrinsic_unary final : public intrinsic_function
    {
    public:
        DECLARE_TYPE_ID();

    private:
        element_unary_op operation;
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_ptr argument_type;

    public:
        explicit intrinsic_unary(element_unary_op operation, type_const_ptr return_type, type_const_ptr argument_type);

        [[nodiscard]] std::shared_ptr<object> compile(const compilation_context& context, const source_information& source_info) const override;
        [[nodiscard]] element_unary_op get_operation() const { return operation; }
    };

    class intrinsic_binary final : public intrinsic_function
    {
    public:
        DECLARE_TYPE_ID();

    private:
        element_binary_op operation;
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_ptr first_argument_type;
        //TODO: this might need to be a constraint_const_shared_ptr
        type_const_ptr second_argument_type;

    public:
        intrinsic_binary(element_binary_op operation, type_const_ptr return_type, type_const_ptr first_argument_type, type_const_ptr second_argument_type);

        [[nodiscard]] std::shared_ptr<object> compile(const compilation_context& context, const source_information& source_info) const override;
        [[nodiscard]] element_binary_op get_operation() const { return operation; }
    };

    //TODO: Needs to be handled via list with dynamic indexing, this will be insufficient for when we have user input
    class intrinsic_if final : public intrinsic_function
    {
    public:
        DECLARE_TYPE_ID();

        intrinsic_if();

        [[nodiscard]] std::shared_ptr<object> compile(const compilation_context& context, const source_information& source_info) const override;
    };

    class intrinsic_num_constructor final : public intrinsic_function
    {
    public:
        DECLARE_TYPE_ID();

        intrinsic_num_constructor();

        [[nodiscard]] std::shared_ptr<object> call(const compilation_context& context, std::vector<std::shared_ptr<object>> compiled_args, const source_information&
                                                   source_info) const override;
    };

    class intrinsic_bool_constructor final : public intrinsic_function
    {
    public:
        DECLARE_TYPE_ID();

        intrinsic_bool_constructor();

        [[nodiscard]] std::shared_ptr<object> call(const compilation_context& context, std::vector<std::shared_ptr<object>> compiled_args, const source_information&
                                                   source_info) const override;
    };

    class intrinsic_not_implemented final : public intrinsic {
    public:
        DECLARE_TYPE_ID();

        intrinsic_not_implemented() : intrinsic(0)
        {
        }
    };
}