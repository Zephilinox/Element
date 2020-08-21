#include "scope.hpp"

namespace element
{
    scope::scope(const scope* parent_scope, const declaration* const declarer)
        : declarer(declarer)
        , parent_scope(parent_scope)
    {
    }

    element_result scope::merge(std::unique_ptr<scope>&& other)
    {
        if (!is_root() || !other->is_root())
            return ELEMENT_ERROR_UNKNOWN;

        for (auto& [identifier, declaration] : other->declarations)
        {
            if (declarations.count(identifier))
                return ELEMENT_ERROR_MULTIPLE_DEFINITIONS;

            declarations[identifier] = std::move(declaration);
            if (declarations[identifier]->our_scope)
                declarations[identifier]->our_scope->parent_scope = this;
        }

        return ELEMENT_OK;
    }

    std::string scope::location() const
    {
        return declarer->location();
    }

    bool scope::add_declaration(std::unique_ptr<declaration> declaration)
    {
        const auto& [it, success] = declarations.try_emplace(declaration->name.value, std::move(declaration));
        return success;
    }

    const declaration* scope::find(const identifier& name, const bool recurse = false) const
    {
        const auto name_it = declarations.find(name);
        if (name_it != declarations.end())
            return name_it->second.get();

        return (recurse && parent_scope) ? parent_scope->find(name, true) : nullptr;
    }

    const scope* scope::get_global() const
    {
        const auto* global = this;
        while (global->parent_scope)
            global = global->parent_scope;

        return global;
    }
}