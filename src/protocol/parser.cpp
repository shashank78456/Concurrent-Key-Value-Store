#include "protocol/parser.hpp"

#include <sstream>

CommandType Parser::parseCommandType(const string& command) {
    if(command=="SET")
        return CommandType::SET;
    if(command=="GET")
        return CommandType::GET;
    if(command=="DELETE")
        return CommandType::DELETE;
    
    return CommandType::INVALID;
}

Command Parser::parse(const string& request) {
    istringstream stream(request);
    string command;
    stream >> command;

    CommandType type = parseCommandType(command);

    switch(type) {
        
        case CommandType::SET: {
            string key;
            string value;

            stream >> key;

            if(key.empty())
                return {};

            getline(stream >> ws, value);

            if(value.empty())
                return {};

            return {CommandType::SET, key, value};
        }
    
        case CommandType::GET: {
            string key;

            stream >> key;

            if(key.empty())
                return {};

            return {CommandType::GET, key, ""};
        }

        case CommandType::DELETE: {
            string key;

            stream >> key;

            if(key.empty())
                return {};

            return {CommandType::DELETE, key, ""};
        }

        case CommandType::INVALID: {
            return {};
        }
    }

    return {};
}