#pragma once

//SELF
#include "intrinsic_function.hpp"

namespace element
{
    class intrinsic_constructor_bool final : public intrinsic_function
    {
    public:
        DECLARE_TYPE_ID();

        intrinsic_constructor_bool();

        [[nodiscard]] object_const_shared_ptr call(
            const compilation_context& context,
            std::vector<object_const_shared_ptr> compiled_args,
            const source_information& source_info) const override;
    };
} // namespace element