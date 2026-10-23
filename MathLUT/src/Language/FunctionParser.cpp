#include "FunctionParser.h"

#include <sstream>
#include <cctype>

namespace
{
    std::string Trim(const std::string& text)
    {
        const std::string whitespace = " \t\r\n";

        const size_t start = text.find_first_not_of(whitespace);

        if (start == std::string::npos)
            return "";

        const size_t end = text.find_last_not_of(whitespace);

        return text.substr(start, end - start + 1);
    }

    bool IsValidChannel(const std::string& channel)
    {
        return channel == "r" || channel == "g" || channel == "b" || channel == "a" || channel == "rgb" || channel == "rgba";
    }
}

bool FunctionParser::Parse(const std::string& source, std::vector<FunctionAssignment>& assignments, std::string& error) const
{
    assignments.clear();
    error.clear();

    std::istringstream input(source);
    std::string line;
    std::string statement;
    int lineNumber = 0;
    int statementStartLine = 1;

    while (std::getline(input, line))
    {
        ++lineNumber;

        const size_t commentPosition = line.find("//");

        if (commentPosition != std::string::npos)
            line.erase(commentPosition);

        if (Trim(line).empty())
            continue;

        if (Trim(statement).empty())
            statementStartLine = lineNumber;

        statement += line;
        statement += '\n';

        size_t semicolonPosition = std::string::npos;

        while ((semicolonPosition = statement.find(';')) != std::string::npos)
        {
            std::string current = Trim(statement.substr(0, semicolonPosition));

            statement.erase(0, semicolonPosition + 1);

            if (current.empty())
                continue;

            const size_t equalsPosition = current.find('=');

            if (equalsPosition == std::string::npos)
            {
                error = "Expected an assignment at line " + std::to_string(statementStartLine) + ".";
                return false;
            }

            const std::string channel = Trim(current.substr(0, equalsPosition));
            const std::string expression = Trim(current.substr(equalsPosition + 1));

            if (!IsValidChannel(channel))
            {
                error = "Unknown channel '" + channel + "' at line " + std::to_string(statementStartLine) + ".";
                return false;
            }

            if (expression.empty() || expression.front() == '=')
            {
                error = "Invalid expression at line " + std::to_string(statementStartLine) + ".";
                return false;
            }

            assignments.push_back({ channel, expression });
            statementStartLine = lineNumber;
        }
    }

    if (!Trim(statement).empty())
    {
        error = "Missing semicolon at the end of the final statement.";
        return false;
    }

    if (assignments.empty())
    {
        error = "The function contains no assignments.";
        return false;
    }

    return true;
}
