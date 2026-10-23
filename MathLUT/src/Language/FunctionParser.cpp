
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
}

bool FunctionParser::Parse(const std::string& source, std::vector<FunctionStatement>& statements, std::string& error) const
{
    statements.clear();
    error.clear();

    std::istringstream input(source);
    std::string line, statement;
    int lineNumber = 0, statementStartLine = 1;

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

            if (!current.empty())
                statements.push_back({ current, statementStartLine });

            statementStartLine = lineNumber;
        }
    }

    if (!Trim(statement).empty())
    {
        error = "Missing semicolon at the end of the final statement.";
        return false;
    }

    if (statements.empty())
    {
        error = "The function contains no statements.";
        return false;
    }

    return true;
}
