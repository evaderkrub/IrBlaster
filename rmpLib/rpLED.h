#pragma once

class rpLED {
    private:
        int iIOPin ;
        bool bHasBeenInit=false;
    public:

        rpLED();
        rpLED(int iPin);

        void on();
        void off();

        void init();
        
        void toggle();
};

