
// 
// Raspberry Meet-up Pi 4-11-2022
// https://www.meetup.com/raspberry-meetup-pi/
// Copyright 2022 Dave Robins
// MIT license
//
// IR control meetup : https://www.meetup.com/raspberry-meetup-pi/events/284622521/
//

#include "pico/stdlib.h"

// Raspberry Meetup Pi Class Library
#include "rmpPicoLib.h"

// This project's pin definitions
#include "IRBlaster_pin_definitions.h"

#define IR_ROKU_OK      0xD52AC7EA
#define IR_ROKU_PLAY    0xb34cc7ea

// objects used for this project
rpLED obLED(LED_PIN);
rpButton obButton(BUTTON_BOARD, rpButtonActiveStates::low);
rpIRComm obIR;

int main()
 {

    // initilization of objects
    stdio_init_all();
    obLED.init();
    obButton.init(rpGPIOPullResistor::up);
    obIR.init(IRLED_INPUT, IRLED_OUTPUT);
    
    obIR.iIRCode = IR_ROKU_PLAY;

    // main loop
    while (true)
    {
        // Send the IR waveform when the button is pressed
        if (obButton.pressed())
        {
            //obIR.SendIRData();

            bool bFinished = false;
            
            obIR.BuildIRDataFromCode(obIR.iIRCode);
            obIR.SendIRDataStateMachine(true,bFinished);
            do
            {
                obIR.SendIRDataStateMachine(false,bFinished);
                /* code */
            } while (!bFinished);
            
            obLED.off();
            sleep_ms(125);
            obLED.on();
            sleep_ms(125);
            //while (obButton.pressed()) {};
        }

        // do nothing until we receive IR data
        if (!obIR.isRxActive())
            continue;

        // Tight loop on the IR reception
        if (obIR.isRxActive())
        {
            obIR.ReadIRData();
            obIR.ProcessIRData();
            while (obIR.isRxActive()) {};
            // Toggle LED when a IR Read is done
            for (int iCount=0;iCount<4;iCount++)
            {
                obLED.off();
                sleep_ms(50);
                obLED.on();
                sleep_ms(50);
            }
        }

        
        
    }

}
