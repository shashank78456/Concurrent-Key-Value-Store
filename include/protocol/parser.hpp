#pragma once

#include "command.hpp"

#include <string>

class Parser {
public:
    static Command parse(const string& request);

    static CommandType parseCommandType(const string& command);
};