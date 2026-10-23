#include "GLSLGenerator.h"

#include <sstream>

bool GLSLGenerator::Generate(const std::string& shaderTemplate, const std::vector<FunctionAssignment>& assignments, std::string& generatedSource, std::string& error) const
{
    const std::string marker = "//MATHLUT_FUNCTION";

    const size_t markerPosition = shaderTemplate.find(marker);

    if (markerPosition == std::string::npos)
    {
        error = "Shader template is missing the function marker.";
        return false;
    }

    if (shaderTemplate.find(marker, markerPosition + marker.size()) != std::string::npos)
    {
        error = "Shader template contains multiple function markers.";
        return false;
    }

    std::ostringstream functionCode;

    for (const FunctionAssignment& assignment : assignments)
    {
        std::string target;

        if (assignment.channel == "rgba")
            target = "result";
        else
            target = "result." + assignment.channel;

        functionCode << "    " << target << " = " << assignment.expression << ";\n";
    }

    generatedSource = shaderTemplate;
    generatedSource.replace(markerPosition, marker.size(), functionCode.str());
    error.clear();
    return true;
}
