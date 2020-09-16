#pragma once

//SELF
#include "type.hpp"
#include "object_model/compilation_context.hpp"
#include "any_constraint.hpp"

using namespace element;

type::type(element_type_id id, identifier name, const  element::identity* declarer_identity):
    constraint(type_id, declarer_identity)
    , name(std::move(name))
{
}
