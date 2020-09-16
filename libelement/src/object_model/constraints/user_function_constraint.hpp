#pragma once

//SELF
#include "constraint.hpp"

namespace element
{
    struct user_function_constraint : public constraint {
    public:
        DECLARE_TYPE_ID();
        user_function_constraint(const element::identity* declarer_identity)
            : constraint(type_id, declarer_identity)
        {
            
        }

        [[nodiscard]] bool matches_constraint(const compilation_context& context, const constraint* constraint) const override;
    };
}