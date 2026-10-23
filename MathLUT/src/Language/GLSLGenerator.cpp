#include "GLSLGenerator.h"
#include "BuiltinRegistry.h"

#include <sstream>
#include <regex>

bool GLSLGenerator::Generate(const std::string& shaderTemplate, const std::vector<FunctionStatement>& statements, std::string& generatedSource, std::string& error) const
{
    const std::string marker = "//MATHLUT_FUNCTION";
    const std::string uniformMarker = "//MATHLUT_UNIFORMS";
    const size_t markerPosition = shaderTemplate.find(marker);
    const size_t uniformMarkerPosition = shaderTemplate.find(uniformMarker);

    if (markerPosition == std::string::npos || uniformMarkerPosition == std::string::npos)
    {
        error = "Shader template is missing a MathLUT marker.";
        return false;
    }

    if (shaderTemplate.find(marker, markerPosition + marker.size()) != std::string::npos ||
        shaderTemplate.find(uniformMarker, uniformMarkerPosition + uniformMarker.size()) != std::string::npos)
    {
        error = "Shader template contains multiple MathLUT markers.";
        return false;
    }

    std::string source = shaderTemplate;
    std::ostringstream uniformCode, functionCode;

    for (const BuiltinVariable& builtin : BuiltinRegistry::GetAll())
    {
        if (builtin.type == BuiltinType::Uniform)
            uniformCode << "uniform float " << builtin.glslName << ";\n";
    }

    const std::regex channelAssignment(R"(^\s*(rgba|rgb|r|g|b|a)\s*(\+=|-=|\*=|/=|=(?!=))\s*)");
    const std::regex channelDeclaration(R"(^\s*(float|int|uint|bool|vec[234]|ivec[234]|uvec[234]|bvec[234])\s+(rgba|rgb|r|g|b|a)\b)");
    const std::regex channelReference(R"(\b(rgba|rgb|r|g|b|a)\b)");

    for (const FunctionStatement& statement : statements)
    {
        std::string code = statement.source;
        std::smatch match;

        const bool isChannelAssignment = std::regex_search(code, match, channelAssignment);
        const bool isChannelDeclaration = std::regex_search(code, channelDeclaration);

        if (isChannelAssignment)
        {
            const std::string channel = match[1].str();
            const std::string target = channel == "rgba" ? "result" : "result." + channel;
            code.replace(match.position(1), match.length(1), target);
        }

        if (!isChannelDeclaration)
        {
            std::string convertedCode;
            std::sregex_iterator current(code.begin(), code.end(), channelReference), end;
            size_t lastPosition = 0;

            for (; current != end; ++current)
            {
                const std::smatch& channelMatch = *current;
                const size_t position = channelMatch.position();
                const size_t length = channelMatch.length();

                convertedCode.append(code, lastPosition, position - lastPosition);

                const std::string channel = channelMatch[1].str();
                const bool isAssignmentTarget = isChannelAssignment && position < code.find('=');

                if (isAssignmentTarget)
                    convertedCode += channel;
                else if (channel == "rgba")
                    convertedCode += "result";
                else if (channel == "rgb")
                    convertedCode += "result.rgb";
                else
                    convertedCode += "result." + channel;

                lastPosition = position + length;
            }

            convertedCode.append(code, lastPosition, std::string::npos);
            code = convertedCode;
        }

        for (const BuiltinVariable& builtin : BuiltinRegistry::GetAll())
        {
            if (builtin.type == BuiltinType::Constant)
            {
                const std::regex identifier("\\b" + builtin.name + "\\b");
                code = std::regex_replace(code, identifier, builtin.value);
            }
            else if (builtin.type == BuiltinType::Uniform)
            {
                const std::regex identifier("\\b" + builtin.name + "\\b");
                code = std::regex_replace(code, identifier, builtin.glslName);
            }
        }

        functionCode << "    " << code << ";\n";
    }

    source.replace(uniformMarkerPosition, uniformMarker.size(), uniformCode.str());

    const size_t updatedMarkerPosition = source.find(marker);
    source.replace(updatedMarkerPosition, marker.size(), functionCode.str());

    generatedSource = source;
    error.clear();
    return true;
}