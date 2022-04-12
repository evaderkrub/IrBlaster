#pragma once

#include "rpGPIO.h"

enum class rpButtonActiveStates 
{
    low=false,
    high=true,
};

class rpButton
{
    private:

        rpGPIO obGPIO;
        bool m_bPressedState = true;

    public:
        rpButton(int iPin, rpButtonActiveStates bActiveState);
        
        bool pressed();

        void init(rpGPIOPullResistor rpPull);

        void EnableEvents();

};
