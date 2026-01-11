#include "moonlite.h"

Moonlite::Moonlite(unsigned long baudRate)
{
    Serial.begin(baudRate);
}

void Moonlite::update()
{
    receive();
}

bool Moonlite::commandAvailable() const
{
    return command_available_;
}

Command Moonlite::getCommand()
{
    command_available_ = false;
    return current_command_;
}

void Moonlite::sendHex2(uint8_t value)
{
    char buffer[3];
    sprintf(buffer, "%02X", value);
    Serial.print(buffer);
    Serial.print('#');
}

void Moonlite::sendHex4(uint16_t value)
{
    char buffer[5];
    sprintf(buffer, "%04X", value);
    Serial.print(buffer);
    Serial.print('#');
}

void Moonlite::sendString(const char *str)
{
    Serial.print(str);
    Serial.print('#');
}

void Moonlite::sendAck()
{
    Serial.print('#');
}

int Moonlite::parseHex(const char *str, size_t length)
{
    int result = 0;
    for (size_t i = 0; i < length && str[i] != '\0'; i++)
    {
        result *= 16;
        char c = str[i];
        if (c >= '0' && c <= '9')
            result += c - '0';
        else if (c >= 'A' && c <= 'F')
            result += c - 'A' + 10;
        else if (c >= 'a' && c <= 'f')
            result += c - 'a' + 10;
    }
    return result;
}

void Moonlite::receive()
{
    while (Serial.available())
    {
        char ch = Serial.read();

        switch (ch)
        {
        case START_CHARACTER:
            command_.clear();
            command_available_ = false;
            break;

        case END_CHARACTER:
            parseCommand();
            command_available_ = true;
            break;

        default:
            command_ += ch;
            command_available_ = false;
            break;
        }
    }
}

void Moonlite::parseFocuserCommand()
{
    if (command_.length() < 2)
    {
        current_command_ = Command{CommandType::UNKNOWN, 0};
        return;
    }

    switch (command_[1])
    {
    case 'G':
        current_command_ = Command{CommandType::CMD_FG, 0};
        break;
    case 'Q':
        current_command_ = Command{CommandType::CMD_FQ, 0};
        break;
    default:
        current_command_ = Command{CommandType::UNKNOWN, 0};
        break;
    }
}

void Moonlite::parseGetCommand()
{
    if (command_.length() < 2)
    {
        current_command_ = Command{CommandType::UNKNOWN, 0};
        return;
    }

    switch (command_[1])
    {
    case 'B':
        current_command_ = Command{CommandType::CMD_GB, 0};
        break;
    case 'C':
        current_command_ = Command{CommandType::CMD_GC, 0};
        break;
    case 'D':
        current_command_ = Command{CommandType::CMD_GD, 0};
        break;
    case 'H':
        current_command_ = Command{CommandType::CMD_GH, 0};
        break;
    case 'I':
        current_command_ = Command{CommandType::CMD_GI, 0};
        break;
    case 'N':
        current_command_ = Command{CommandType::CMD_GN, 0};
        break;
    case 'P':
        current_command_ = Command{CommandType::CMD_GP, 0};
        break;
    case 'T':
        current_command_ = Command{CommandType::CMD_GT, 0};
        break;
    case 'V':
        current_command_ = Command{CommandType::CMD_GV, 0};
        break;
    default:
        current_command_ = Command{CommandType::UNKNOWN, 0};
        break;
    }
}

void Moonlite::parseSetCommand()
{
    if (command_.length() < 2)
    {
        current_command_ = Command{CommandType::UNKNOWN, 0};
        return;
    }

    switch (command_[1])
    {
    case 'C':
        if (command_.length() >= 4)
        {
            int value = parseHex(&command_[2], 2);
            current_command_ = Command{CommandType::CMD_SC, value};
        }
        else
        {
            current_command_ = Command{CommandType::UNKNOWN, 0};
        }
        break;

    case 'D':
        if (command_.length() >= 4)
        {
            int value = parseHex(&command_[2], 2);
            current_command_ = Command{CommandType::CMD_SD, value};
        }
        else
        {
            current_command_ = Command{CommandType::UNKNOWN, 0};
        }
        break;

    case 'F':
        current_command_ = Command{CommandType::CMD_SF, 0};
        break;

    case 'H':
        current_command_ = Command{CommandType::CMD_SH, 0};
        break;

    case 'N':
        if (command_.length() >= 6)
        {
            int value = parseHex(&command_[2], 4);
            current_command_ = Command{CommandType::CMD_SN, value};
        }
        else
        {
            current_command_ = Command{CommandType::UNKNOWN, 0};
        }
        break;

    case 'P':
        if (command_.length() >= 6)
        {
            int value = parseHex(&command_[2], 4);
            current_command_ = Command{CommandType::CMD_SP, value};
        }
        else
        {
            current_command_ = Command{CommandType::UNKNOWN, 0};
        }
        break;

    default:
        current_command_ = Command{CommandType::UNKNOWN, 0};
        break;
    }
}

void Moonlite::parseCommand()
{
    if (command_[0] == '2')
        command_ = command_.substr(1);

    if (command_.length() < 1)
    {
        current_command_ = Command{CommandType::UNKNOWN, 0};
        return;
    }

    switch (command_[0])
    {
    case 'C':
        current_command_ = Command{CommandType::CMD_C, 0};
        break;

    case 'F':
        parseFocuserCommand();
        break;

    case 'G':
        parseGetCommand();
        break;

    case 'S':
        parseSetCommand();
        break;

    default:
        current_command_ = Command{CommandType::UNKNOWN, 0};
        break;
    }
}
