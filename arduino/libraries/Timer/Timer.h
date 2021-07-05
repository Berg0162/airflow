#ifndef _TIMER_H_
#define _TIMER_H_

typedef void (*funcPtr_t)();

class TimerClass {
    public:
        TimerClass(NRF_TIMER_Type* timer, int channel);
        void attachInterrupt(funcPtr_t callback, int microsec);
        inline void detachInterrupt();
    private:
        NRF_TIMER_Type*        nrf_timer;
        nrf_timer_cc_channel_t cc_channel;
};

//extern TimerClass NrfTimer0;
extern TimerClass NrfTimer1;
extern TimerClass NrfTimer2;
extern TimerClass NrfTimer3;
extern TimerClass NrfTimer4;

extern TimerClass* NrfTimerx[];

#endif // _TIMER_H_
