
#include "rpIRComm.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/time.h"
#include "pico/stdlib.h"




void rpIRComm::init(int iInputPin, int iOutputPin)
{
    m_iInputPin = iInputPin;
    m_iOutputPin = iOutputPin;

    gpio_init(m_iInputPin);
    gpio_set_dir(m_iInputPin,GPIO_IN);
    gpio_pull_up(m_iInputPin);

    gpio_init(m_iOutputPin);
    gpio_set_dir(m_iOutputPin,GPIO_OUT);
    gpio_set_drive_strength(m_iOutputPin, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_function(m_iOutputPin, GPIO_FUNC_PWM);

    // PWM setup
    slice_num = pwm_gpio_to_slice_num(m_iOutputPin);
    pwm_set_wrap(slice_num, 3289);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1644); // 44 Khz
    pwm_set_enabled(slice_num, false);

    m_iIRCommTxState = 0;
    
}

bool rpIRComm::isRxActive()
{
    return !((bool)gpio_get(m_iInputPin)); // IR is active low
}

void rpIRComm::ReadIRData()
{
    //int iCurrentBit = 0;
    int iCurrentBitState = 0;
    int iCount = 0;
    absolute_time_t stStartTime;
    absolute_time_t stEndTime;
    int64_t iTime;

    iIRBitTimeCount = 0;

    // for each bit
    for (iCount=0;iCount<IR_MEASUREMENTMENT_MAX;iCount++)
    {
        stStartTime = get_absolute_time();

        while (gpio_get(m_iInputPin) == iCurrentBitState)
        {
            stEndTime = get_absolute_time(); 
            iTime = absolute_time_diff_us(stStartTime,stEndTime);
            if (iTime  > 10000)
            {
                iIRBitTimes[iCount] = 0;
                iIRBitTimeCount = iCount;
                return;
            }
        };

        // save the time of this bit
        iIRBitTimes[iCount] = iTime;
        // toggle the bit state
        if (iCurrentBitState == 0)
            iCurrentBitState = 1;
        else
            iCurrentBitState = 0;

    }

}

void rpIRComm::ProcessIRData()
{
    int iCount = 0;
    int64_t iCurrentBit=0;
    iIRCode = 0;
    iIRCodeTotalBits = 0;

    if (iIRBitTimeCount < 16)
        return;

    // validate the start bits
    if (!(iIRBitTimes[0] > 7000 && iIRBitTimes[0] < 9500))
        return;
    if (!(iIRBitTimes[1] > 3500 && iIRBitTimes[1] < 5000))
        return;

    // now process the times
    for (iCount = 2;iCount<iIRBitTimeCount;iCount+=2)
    {
        if (!(iIRBitTimes[iCount] > 400 && iIRBitTimes[iCount] < 650)) // first pulse 562.5 us
            return;

        if (iIRBitTimes[iCount+1] > 1000 && iIRBitTimes[iCount+1] < 2000) // Logic one
            iIRCode += 1LL << iCurrentBit;
        
        iCurrentBit++;
        iIRCodeTotalBits++;
        if (iCurrentBit == 32)
            return;
    }

}

enum IRCOM_TXSTATE
{
    IRCOM_TXSTATE_IDLE,
    IRCOM_TXSTATE_START,
    IRCOM_TXSTATE_TXBITS,
    IRCOM_TXSTATE_DONE,
};

void rpIRComm::BuildIRDataFromCode(unsigned int iNewCode)
{
    int iCount;
    int iBitTimeCount=2;
    iIRBitTimes[0] = 9000;
    iIRBitTimes[1] = 4500;

    for (iCount=0;iCount<32;iCount++)
    {
        if (iNewCode & (1 << (iCount)))
        {
            iIRBitTimes[iBitTimeCount++] = 563;
            iIRBitTimes[iBitTimeCount++] = 1688;
        }
        else
        {
            iIRBitTimes[iBitTimeCount++] = 563;
            iIRBitTimes[iBitTimeCount++] = 563;
        }
    }

    iIRBitTimes[iBitTimeCount++] = 563;
    iIRBitTimeCount = 67;

}

void rpIRComm::SendIRDataStateMachine(bool bStart, bool & bFinished)
{
    absolute_time_t stLastTime;
    absolute_time_t stCurrentTime;
    bFinished = false;
    int  iTime;

    stLastTime._private_us_since_boot = m_stIRCommTxStateStartTime;
    
    if (bStart)
    {
        m_iIRCommTxState = IRCOM_TXSTATE_START;
    }

IRTxStateChange:

    switch (m_iIRCommTxState)
    {
        case IRCOM_TXSTATE_IDLE:
            break;
        case IRCOM_TXSTATE_START:
            m_iIRTxStateBitTimeCount = 0;
            m_bTxStateEnable = true;
            PWMEnable();
            stLastTime = get_absolute_time();
            m_stIRCommTxStateStartTime = stLastTime._private_us_since_boot;
            m_iIRCommTxState =  IRCOM_TXSTATE_TXBITS;
            break;
        case  IRCOM_TXSTATE_TXBITS:

            stCurrentTime = get_absolute_time(); 
            iTime = absolute_time_diff_us(stLastTime,stCurrentTime);

            // Did we reach the time?
            if (iTime >= iIRBitTimes[m_iIRTxStateBitTimeCount])
            {
                m_iIRTxStateBitTimeCount++;
                m_stIRCommTxStateStartTime = stCurrentTime._private_us_since_boot;

                if (iIRBitTimeCount == m_iIRTxStateBitTimeCount)
                {
                    m_iIRCommTxState = IRCOM_TXSTATE_DONE;
                    goto IRTxStateChange;
                }
                
                m_bTxStateEnable = !m_bTxStateEnable;

                if (m_bTxStateEnable)
                    PWMEnable();
                else 
                    PWMDisable();
                
               
            }
            break;
        case IRCOM_TXSTATE_DONE:
            PWMDisable();
            m_iIRCommTxState = IRCOM_TXSTATE_IDLE;
            bFinished = true;
            break;
    }

}

void rpIRComm::PWMEnable()
{
    gpio_set_function(m_iOutputPin, GPIO_FUNC_PWM);
    pwm_set_enabled(slice_num, true);
}

void rpIRComm::PWMDisable()
{
    pwm_set_enabled(slice_num, false);
    gpio_set_function(m_iOutputPin, GPIO_FUNC_SIO);
    gpio_put(m_iOutputPin, 0);
}

void rpIRComm::SendIRDataBlocking()
{
    int iCount = 0;
    absolute_time_t stStartTime;
    absolute_time_t stEndTime;
    bool bPWMEnable = true;
    int  iTime;

    // start with the first pus
    for (iCount = 0;iCount<iIRBitTimeCount;iCount++)
    {
        if (bPWMEnable)
            PWMEnable();
        else 
            PWMDisable();

        stStartTime = get_absolute_time();
        do 
        {
            stEndTime = get_absolute_time(); 
            iTime = absolute_time_diff_us(stStartTime,stEndTime);
        } while (iTime < iIRBitTimes[iCount]);

        bPWMEnable = !bPWMEnable;

    }
    
    PWMDisable();

}


