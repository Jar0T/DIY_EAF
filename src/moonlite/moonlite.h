#pragma once

#include <Arduino.h>
#include <string>
#include "command.h"

/**
 * @brief Non-blocking Moonlite protocol communication handler
 *
 * Handles serial communication for Moonlite focuser protocol.
 * Commands are framed with ':' (start) and '#' (end) characters.
 * This class only parses the protocol - business logic should be handled separately.
 */
class Moonlite
{
private:
    static const char START_CHARACTER = ':';
    static const char END_CHARACTER = '#';
    static const int MAX_MESSAGE_LENGTH = 16;

    // Current message buffer
    std::string command_;

    Command current_command_ = {CommandType::UNKNOWN, 0};
    bool command_available_ = false;

    /**
     * @brief Parse hex string to integer
     * @param str Pointer to hex string
     * @param length Number of hex digits to parse
     * @return Parsed integer value
     */
    int parseHex(const char *str, size_t length);

    /**
     * @brief Receive and buffer incoming serial data (non-blocking)
     */
    void receive();

    /**
     * @brief Parse Focuser commands (FG, FQ)
     */
    void parseFocuserCommand();

    /**
     * @brief Parse Get commands (GB, GC, GD, GH, GI, GN, GP, GT, GV)
     */
    void parseGetCommand();

    /**
     * @brief Parse Set commands (SC, SD, SF, SH, SN, SP)
     */
    void parseSetCommand();

    /**
     * @brief Parse buffered command string into Command struct
     */
    void parseCommand();

public:
    /**
     * @brief Initialize Moonlite communication
     * @param baudRate Serial baud rate (default: 9600, standard for Moonlite)
     */
    explicit Moonlite(unsigned long baudRate = 9600);

    /**
     * @brief Update communication state (call frequently in main loop)
     *
     * Non-blocking - processes available serial data without delay
     */
    void update();

    /**
     * @brief Check if a new command has been received
     * @return true if command is ready to be retrieved
     */
    bool commandAvailable() const;

    /**
     * @brief Get the received command (clears the available flag)
     * @return Parsed command with type and value
     */
    Command getCommand();

    /**
     * @brief Send 2-digit hex response (for GB, GC, GD, GV commands)
     * @param value 8-bit value to send (0x00-0xFF)
     */
    void sendHex2(uint8_t value);

    /**
     * @brief Send 4-digit hex response (for GH, GN, GP, GT commands)
     * @param value 16-bit value to send (0x0000-0xFFFF)
     */
    void sendHex4(uint16_t value);

    /**
     * @brief Send '#' terminated string
     * @param str Null-terminated string to send
     */
    void sendString(const char *str);

    /**
     * @brief Send simple acknowledgment (no data)
     */
    void sendAck();
};
