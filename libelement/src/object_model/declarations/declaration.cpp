#include "declaration.hpp"

//LIBS
#include <fmt/format.h>

//SELF
#include "object_model/intermediaries/declaration_wrapper.hpp"
#include "object_model/scope.hpp"
#include "object_model/error.hpp"
#include "object_model/compilation_context.hpp"

using namespace element;

declaration::declaration(identifier name, const scope* parent)
    : name(std::move(name))
    , our_scope(std::make_unique<scope>(parent, this))
    , wrapper(std::make_shared<declaration_wrapper>(this))
{
    assert(parent);
}

bool declaration::has_scope() const
{
    return !our_scope->is_empty();
}

object_const_shared_ptr declaration::generate_placeholder(const compilation_context& context, int& placeholder_index, unsigned int boundary_scope) const
{
    auto err = std::make_shared<const error>(fmt::format("Tried to generate a placeholder for an unexpected declaration"), ELEMENT_ERROR_UNKNOWN, source_info);
    err->log_once(context.get_logger());
    return err;
}

std::string declaration::location() const
{
    return name.value;
    //assert(our_scope && our_scope->get_parent_scope());

    //if (our_scope->get_parent_scope()->is_root())
    //    return name.value;

    ////recursive construction
    //return fmt::format("{}.{}", our_scope->get_parent_scope()->location(), name.value);
}