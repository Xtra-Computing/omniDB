// THIS LOCK IS NOT-REENTRANT !!
class CPP_SpinLock
{
public:    // inlined constructor
    // inlined NON-virtual destructor
    inline CPP_SpinLock() : m_s(1) {}
    inline ~CPP_SpinLock() {}    
    
    // enter the lock, spinlocks (with/without Sleep)
    // when mutex is already locked
    inline void Enter(void)
    {
        int prev_s;
        do
        {
            prev_s = TestAndSet(&m_s, 0);
            if (m_s == 0 && prev_s == 1)
            {
                break;
            }
            // reluinquish current timeslice (can only
            // be used when OS available and
            // we do NOT want to 'spin')
            // HWSleep(0);
        }
        while (true);
    }
    // Tries to enter the lock, returns 0
    // when mutex is already locked, 
    // returns != 0 when success
    inline int TryEnter(void)
    {
        int prev_s = TestAndSet(&m_s, 0);
        if (m_s == 0 && prev_s == 1)
        {
            return 1;
        }
        return 0;
    }
    // Leaves or unlocks the mutex
    // (should only be called by lock owner)
    inline void Leave(void)
    {
        TestAndSet(&m_s, 1);
    }
    
    protected:
    // sets BIT value and returns previous
    // value.in 1 atomic un-interruptable operation
    int TestAndSet(int* pTargetAddress, int nValue);
    
    private:
    int m_s;
};

// This part is Platform dependent!
#ifdef WIN32
inline int CPP_SpinLock::TestAndSet(int* pTargetAddress, 
                                              int nValue)
{
    __asm
    {
        mov edx, dword ptr [pTargetAddress]
        mov eax, nValue
        lock xchg eax, dword ptr [edx]
    }
    // mov = 1 CPU cycle
    // lock = 1 CPU cycle
    // xchg = 3 CPU cycles
}
#endif // WIN32