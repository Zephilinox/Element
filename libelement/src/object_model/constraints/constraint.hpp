#pragma once

//SELF
#include "typeutil.hpp"
#include "object_model/object.hpp"

namespace element
{
    struct constraint : public rtti_type<constraint>
    {
    public:
        static const constraint_const_unique_ptr any;
        //todo: what is a function constraint and what uses it? not something a user has access to, so something internal?
        static const constraint_const_unique_ptr function;

        constraint(element_type_id id, const element::identity* declarer_identity);

        [[nodiscard]] virtual bool matches_constraint(const compilation_context& context, const constraint* constraint) const;
        [[nodiscard]] const constraint* get_constraint() const { return this; }

        const identity* identity;
    };
}