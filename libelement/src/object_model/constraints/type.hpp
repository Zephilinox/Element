#pragma once

//SELF
#include "constraint.hpp"
#include "object_model/object.hpp"

namespace element
{
    struct type : public constraint
    {
    public:
        DECLARE_TYPE_ID();

        static const type_const_unique_ptr num;      // the absolute unit
        static const type_const_unique_ptr boolean; 

        [[nodiscard]] identifier get_identifier() const { return name; }

    protected:
        type(element_type_id id, identifier name, const  element::identity* declarer_identity);

        identifier name;
    };
}