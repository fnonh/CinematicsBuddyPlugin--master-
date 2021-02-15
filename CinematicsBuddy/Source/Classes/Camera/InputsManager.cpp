#include "InputsManager.h"
#include "SupportFiles/MacrosStructsEnums.h"

/*

    Instead of averaging forward/back inputs, use those inputs to generate an acceleration/brake along forward vector up to a certain velocity?
        - Take delta into account. Add/subtract velocity based on input strength multiplied by delta
        - Velocity would be entirely local. If player is looking up and down while decelerating with no inputs, they'll move in a rollercoaster
            - Alternatively have a checkbox to enable preservation of momentum in world space
                - i.e. User could build up speed moving forward, then look up. They would build up speed going upward while they continue moving forward. That would create a nice curve
        - Reduce the strength of the input (or lack of input) the closer the velocity is to max to ease into final speed?
        - Acceleration should be a matter of duration (i.e. 1 second to max speed), so force applied should be tied to max speed
        - Take FOV into account for pitch and yaw rotation speed. Maybe even roll as well, but that's less likely to be an issue
        - Make roll button more responsive? It seems to take about a second before it toggles bRoll
            - Maybe just make that a binding in a dropdown in the settings file and check each tick if the chosen (cached) FNameByString button is pressed
                - Only necessary for controllers because keyboard users get instant input. Could nicely trim down the size of the list
                    - Name the buttons independently of XBox. i.e. "Xbox A - PS4 X@XboxTypeS_A"
                    - Generate a static vector<pair<DisplayName, RealName>> to be filled by settings file generator once, but used multiple times
                        - static bool bHasBeenGenerated = false; // Do thing // bHasBeenGenerated = true;

*/

InputsManager::InputsManager()
    :
Forward(0.f),
Right(0.f),
Up(0.f),
Pitch(0.f),
Yaw(0.f),
Roll(0.f),
bUsingGamepad(false) {}

void InputsManager::PlayerInputTick(float Delta, bool bRoll)
{
    PlayerControllerWrapper Controller = GlobalGameWrapper->GetPlayerController();
	if(Controller.IsNull()) return;

    GetInputs(Controller, bRoll);
    NullifyInputs(Controller);
}

void InputsManager::GetInputs(PlayerControllerWrapper Controller, bool bRoll)
{
    //Retrieve all the state values
    bUsingGamepad = Controller.GetbUsingGamepad();
    bRoll = Controller.GetbRoll(); // #TODO: Replace this with cvar/IsKeyPressed method to avoid delay. Since that isn't set up yet, just pull from game

    //Retrieve the inputs
    //#TODO: Use bUsingGamepad here to multiply inputs by GamepadLookScale? Give the final inputs to CameraManager instead of premultiplied inputs?
    //In that same vein, should final inputs be multiplied by Delta as well?
    Forward = Controller.GetAForward();
    Right = Controller.GetAStrafe();
    Up = Controller.GetAUp();
    Pitch = Controller.GetALookUp();
    Yaw = Controller.GetATurn();
    Roll = 0.f;
    if(bUsingGamepad)
    {
        if(bRoll)
        {
            Roll = Yaw;
            Yaw = 0.f;
        }
    }
    else
    {
        Roll = Controller.GetALookRoll();
    }
}

void InputsManager::NullifyInputs(PlayerControllerWrapper Controller)
{
    //Reset the inputs that cause movement or rotation
    Controller.SetAForward(0.f);
    Controller.SetAStrafe(0.f);
    Controller.SetAUp(0.f);
    Controller.SetALookUp(0.f);
    Controller.SetATurn(0.f);
    Controller.SetALookRoll(0.f);
}

// TESTS - REMOVE WHEN DONE //
void InputsManager::DebugRender(CanvasWrapper Canvas, std::vector<std::string>& RenderStrings)
{
    RenderStrings.push_back("bUsingGamepad: " + std::to_string(bUsingGamepad));
    RenderStrings.push_back("Forward: " + std::to_string(Forward));
    RenderStrings.push_back("Strafe: " + std::to_string(Right));
    RenderStrings.push_back("Up: " + std::to_string(Up));
    RenderStrings.push_back("Pitch: " + std::to_string(Pitch));
    RenderStrings.push_back("Yaw: " + std::to_string(Yaw));
    RenderStrings.push_back("Roll: " + std::to_string(Roll));
}