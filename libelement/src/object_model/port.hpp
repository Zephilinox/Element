#pragma once

//SELF
#include "type_annotation.hpp"
#include "fwd.hpp"

namespace element
{
    class port final
    {
    public:
        explicit port(const declaration* declarer, identifier name, std::unique_ptr<type_annotation> annotation, std::unique_ptr<expression_chain> expr_chain);

        [[nodiscard]] bool has_annotation() const { return annotation != nullptr; }

        [[nodiscard]] std::string typeof_info() const;
        [[nodiscard]] std::string to_code(const int depth) const;

        [[nodiscard]] const std::string& get_name() const { return name.value; }
        [[nodiscard]] const type_annotation* get_annotation() const { return annotation.get(); }

        [[nodiscard]] const declaration* resolve_annotation(const compilation_context& context) const;
        [[nodiscard]] object_const_shared_ptr generate_placeholder(const compilation_context& context, int& placeholder_index) const;

        [[nodiscard]] bool has_default() const { return get_default(); }
        [[nodiscard]] expression_chain* get_default() const { return expr_chain.get(); }

    private:
        const declaration* declarer;
        identifier name;
        std::unique_ptr<type_annotation> annotation;
        //todo: consider unique_ptr and custom deleter, shared_ptr for now so that it can be forward declared as including expression_chain causes circular inclusion
        std::shared_ptr<expression_chain> expr_chain;
    };
} // namespace element
