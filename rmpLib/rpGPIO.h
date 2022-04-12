#pragma once

enum class rpGPIOPullResistor
{
    none,
    up,
    down,
};

class rpGPIO
{
    private:

        int iPinNumber;

    public:

        rpGPIO();
        rpGPIO(int iPin);

        void init(bool bMakeOutput, rpGPIOPullResistor rpPull);

        bool get();
        void set(bool bValue);

        void EnableEvents();
       
        void setPin(int iPin);

};

