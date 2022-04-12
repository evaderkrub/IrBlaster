
#include "rpButton.h"




rpButton::rpButton(int iPin, rpButtonActiveStates bActiveState) 
{
    m_bPressedState = bActiveState == rpButtonActiveStates::high;
    obGPIO.setPin(iPin);
    
}

bool rpButton::pressed()
{
    if (m_bPressedState)
        return obGPIO.get() ;
    else 
        return !((bool)obGPIO.get());
}

void rpButton::init(rpGPIOPullResistor rpPull)
{
    obGPIO.init(false, rpPull);
}


