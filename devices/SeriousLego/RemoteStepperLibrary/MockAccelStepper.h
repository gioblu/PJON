#ifndef MockAccelStepper_h
#define MockAccelStepper_h
#endif

class MockAccelStepper {
    private:
    public:
        MockAccelStepper();
        void run();
        void move(long param);
        void moveTo(long param);
        void setAcceleration(float param);
        void setMaxSpeed(float param);
        void setCurrentPosition(int param);
        long distanceToGo();
        long currentPosition();
};
