#pragma once

class rpIRComm 
{
    private:
        int m_iInputPin;
        int m_iOutputPin;

        int m_iIRCommTxState;

        void PWMEnable();
        void PWMDisable();

        unsigned long long m_stIRCommTxStateStartTime;
        int m_iIRTxStateBitTimeCount;
        bool m_bTxStateEnable;

        #define IR_MEASUREMENTMENT_MAX  120
        int iIRBitTimes[IR_MEASUREMENTMENT_MAX];
        int iIRBitTimeCount = 0;
        int iIRCodeTotalBits = 0;

unsigned int slice_num;

    public:
        
        long long iIRCode = 0;
        
        
        void init(int iInputPin, int iOutputPin);
        void SendIRDataBlocking();
        void ProcessIRData();
        void ReadIRData();

        bool isRxActive();

        void SendIRDataStateMachine(bool bStart, bool & bFinished);

        void BuildIRDataFromCode(unsigned int iNewCode);
};




