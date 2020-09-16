#pragma once

//SELF
#include "type.hpp"

namespace element
{
    class user_type : public type
    {
    public:
        DECLARE_TYPE_ID();
        user_type(identifier name, const element::identity* declarer_identity)
            : type(type_id, name, declarer_identity)
            , name(std::move(name))
        {}

    private:
        identifier name;
    };
}