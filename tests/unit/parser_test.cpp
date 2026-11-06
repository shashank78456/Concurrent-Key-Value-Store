#include "protocol/parser.hpp"
#include "utils.hpp"

using namespace std;

int main() {
    int failures = 0;

    Command command;

    command = Parser::parse("SET name Test");
    check(command.type==CommandType::SET && command.key=="name" && command.value=="Test", "Parse SET", failures);

    command = Parser::parse("SET message Hello World");
    check(command.type==CommandType::SET && command.value=="Hello World", "Parse SET with spaces", failures);

    command = Parser::parse("SET name");
    check(command.type==CommandType::INVALID, "Reject SET without value", failures);

    command = Parser::parse("SET name    ");
    check(command.type==CommandType::INVALID, "Reject SET with empty value", failures);


    command = Parser::parse("GET name");
    check(command.type==CommandType::GET && command.key=="name", "Parse GET", failures);

    command = Parser::parse("GET");
    check(command.type==CommandType::INVALID, "Reject GET without key", failures);

    command = Parser::parse("GET name extra");
    check(command.type==CommandType::INVALID, "Reject GET with extra arguments", failures);

    
    command = Parser::parse("DELETE name");
    check(command.type==CommandType::DELETE && command.key=="name", "Parse DELETE", failures);

    command = Parser::parse("DELETE");
    check(command.type==CommandType::INVALID, "Reject DELETE without key", failures);

    command = Parser::parse("DELETE name extra");
    check(command.type==CommandType::INVALID, "Reject DELETE with extra arguments", failures);


    command = Parser::parse("UNKNOWN name");
    check(command.type==CommandType::INVALID, "Reject unknown command", failures);

    command = Parser::parse("");
    check(command.type==CommandType::INVALID, "Reject empty request", failures);

    return ((failures==0) ? 0 : 1);
}