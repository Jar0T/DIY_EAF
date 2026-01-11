#pragma once

enum class CommandType
{
    CMD_C,  // Initiate temperature conversion
    CMD_FG, // Go to target position (with XXXX parameter)
    CMD_FQ, // Halt motor movement immediately
    CMD_GB, // Get red LED backlight brightness value (00-FF)
    CMD_GC, // Get current temperature coefficient (XX format, signed 2's complement)
    CMD_GD, // Get current motor speed (FF=slow, 00=fast)
    CMD_GH, // Get half-step mode status
    CMD_GI, // Get motor is moving status (00=stopped, 01=moving)
    CMD_GN, // Get target position (XXXX format)
    CMD_GP, // Get current position (XXXX format)
    CMD_GT, // Get current temperature (XXXX format in half degrees Celsius)
    CMD_GV, // Get firmware version (XX format)
    CMD_SC, // Set temperature coefficient (SCXX format, signed 2's complement)
    CMD_SD, // Set motor speed (SDXX format, FF=slow, 00=fast)
    CMD_SF, // Set full-step mode
    CMD_SH, // Set half-step mode
    CMD_SN, // Set target position (SNXXXX format)
    CMD_SP, // Set current position (SPXXXX format)
    UNKNOWN,
};

struct Command
{
    CommandType type;
    int value;
    int motor;
};
