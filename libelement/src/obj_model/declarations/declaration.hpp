#pragma once

#include <memory>
#include <memory>
#include <string>
#include <vector>


#include "obj_model/port.hpp"
#include "obj_model/element_object.hpp"
#include "obj_model/expressions/expression.hpp"

namespace element
{
    struct scope;

    static const std::string intrinsic_qualifier = "intrinsic";
	static const std::string namespace_qualifier = "namespace";
    static const std::string constraint_qualifier = "constraint";
    static const std::string struct_qualifier = "struct";
    static const std::string function_qualifier; //empty string
    static const std::string return_keyword = "return";
	
	struct declaration : element_object
	{
        std::vector<port> inputs;
        std::unique_ptr<port> output;
        std::unique_ptr<element_constraint> constraint;
		
		std::string qualifier;
        std::string identifier;
        bool intrinsic = false;

        explicit declaration();
		
        [[nodiscard]] bool has_inputs() const;
        [[nodiscard]] bool has_output() const;
        [[nodiscard]] bool has_constraint() const;
        [[nodiscard]] bool is_intrinsic() const;
        [[nodiscard]] virtual std::string location() const;
    };

    struct scoped_declaration : declaration
	{
        std::unique_ptr<scope> scope;

        [[nodiscard]] bool has_scope() const;
    	
        explicit scoped_declaration(const element::scope* parent_scope);

    	void add_declaration(std::unique_ptr<declaration> declaration) const;

        [[nodiscard]] std::string location() const override;
    };

    struct struct_declaration final : scoped_declaration
	{
        struct_declaration(const element::scope* parent_scope, bool is_intrinsic);
    	
        [[nodiscard]] std::string to_string() const override;
    };
	
    struct constraint_declaration final : declaration
	{
		constraint_declaration(bool is_intrinsic);
    };

    struct function_declaration final : scoped_declaration
	{
        function_declaration(const element::scope* parent_scope, bool is_intrinsic);

        [[nodiscard]] std::string to_string() const override;
    };

    struct expression_bodied_function_declaration final : scoped_declaration {

        std::shared_ptr<expression> expression;
    	
        expression_bodied_function_declaration(const element::scope* parent_scope);

        [[nodiscard]] std::string to_string() const override;
    };

    struct namespace_declaration final : scoped_declaration
	{
        namespace_declaration(const element::scope* parent_scope);

        [[nodiscard]] std::string to_string() const override;
    };
}