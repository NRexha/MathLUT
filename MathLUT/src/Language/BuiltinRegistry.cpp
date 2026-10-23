
#include "BuiltinRegistry.h"

const std::vector<BuiltinVariable>& BuiltinRegistry::GetAll()
{
    static const std::vector<BuiltinVariable> builtins =
    {
        { "x", "x", BuiltinType::Existing, "" },
        { "PI", "", BuiltinType::Constant, "3.14159265358979323846" },
        { "_Time", "uTime", BuiltinType::Uniform, "" }
    };

    return builtins;
}
