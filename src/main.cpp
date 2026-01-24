#include <Arduino.h>
#include "moonlite/moonlite.h"
#include "moonlite/command.h"
#include "stepper/motion_controller.h"

Moonlite moonlite(9600);
MotionController motionController;

void setup()
{
    // put your setup code here, to run once:
}

void loop()
{
    moonlite.update();
    if (moonlite.commandAvailable())
    {
        Command cmd = moonlite.getCommand();

        switch (cmd.type)
        {
        case CommandType::CMD_C:
            // Initiate temperature conversion
            break;

        case CommandType::CMD_FG:
            // Go to target position
            motionController.startMovement();
            break;

        case CommandType::CMD_FQ:
            // Halt motor movement immediately
            motionController.stopMovement();
            break;

        case CommandType::CMD_GB:
            // Get red LED backlight brightness value
            moonlite.sendHex2(0x00); // TODO: Implement backlight control
            break;

        case CommandType::CMD_GC:
            // Get current temperature coefficient
            moonlite.sendHex2(0x02); // TODO: Implement temperature compensation
            break;

        case CommandType::CMD_GD:
            // Get current motor speed
            moonlite.sendHex2(motionController.getSpeed()); // TODO: Implement speed control
            break;

        case CommandType::CMD_GH:
            // Get half-step mode status
            if (motionController.getStepMode() == StepMode::HALF_STEP)
                moonlite.sendHex2(0xFF);
            else
                moonlite.sendHex2(0x00);
            break;

        case CommandType::CMD_GI:
            // Get motor is moving status (00=stopped, 01=moving)
            if (motionController.getIsMoving())
                moonlite.sendHex2(0x01);
            else
                moonlite.sendHex2(0x00);
            break;

        case CommandType::CMD_GN:
            // Get target position
            moonlite.sendHex4(motionController.getTargetPosition());
            break;

        case CommandType::CMD_GP:
            // Get current position
            moonlite.sendHex4(motionController.getCurrentPosition());
            break;

        case CommandType::CMD_GT:
            // Get current temperature
            // Return placeholder 20 degrees Celsius
            moonlite.sendHex4(40); // 20 C = 40 in half degrees
            break;

        case CommandType::CMD_GV:
            // Get firmware version
            moonlite.sendString("V1.0"); // Version 1
            break;

        case CommandType::CMD_SC:
            // Set temperature coefficient
            // Value in cmd.value
            break;

        case CommandType::CMD_SD:
            // Set motor speed
            motionController.setSpeed(cmd.value);
            break;

        case CommandType::CMD_SF:
            // Set full-step mode
            motionController.setStepMode(StepMode::FULL_STEP);
            break;

        case CommandType::CMD_SH:
            // Set half-step mode
            motionController.setStepMode(StepMode::HALF_STEP);
            break;

        case CommandType::CMD_SN:
            // Set target position
            motionController.setTargetPosition(cmd.value);
            break;

        case CommandType::CMD_SP:
            // Set current position
            motionController.setCurrentPosition(cmd.value);
            break;

        case CommandType::UNKNOWN:
            // Unknown command, ignore
            break;
        }
    }
    motionController.update();
}
