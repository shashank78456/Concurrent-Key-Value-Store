#pragma once

#include <string>
using namespace std;

enum class CommandType {
    SET, GET, DELETE, INVALID
};

struct Command {
    CommandType type = CommandType::INVALID;
    string key;
    string value;
};