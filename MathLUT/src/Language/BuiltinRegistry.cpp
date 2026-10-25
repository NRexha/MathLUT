
#include "BuiltinRegistry.h"

const std::vector<BuiltinVariable>& BuiltinRegistry::GetAll()
{
    static const std::vector<BuiltinVariable> builtins =
    {
        { "x", "x", BuiltinType::Existing, "" },

        { "PI", "", BuiltinType::Constant, "3.14159265358979323846" },
        { "TAU", "", BuiltinType::Constant, "6.28318530717958647692" },
        { "E", "", BuiltinType::Constant, "2.71828182845904523536" },

        { "_Time", "uTime", BuiltinType::Uniform, "" }
    };

    return builtins;
}
