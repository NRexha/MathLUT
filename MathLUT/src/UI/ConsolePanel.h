#pragma once

#include <string>
#include <vector>

enum class ConsoleMessageType
{
    Info,
    Success,
    Warning,
    Error
};

struct ConsoleMessage
{
    ConsoleMessageType type;
    std::string message;
};

class ConsolePanel
{
public:
    void Log(ConsoleMessageType type, const std::string& message);
    void Clear();

    void Render();

private:
    std::vector<ConsoleMessage> messages;
};
