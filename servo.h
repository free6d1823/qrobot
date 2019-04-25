#ifndef SERVO_H
#define SERVO_H

#include "common.h"
class Servo
{
public:
    Servo();

    void setPortName(char* name, int port);

    char* getPortName(int* pPortNumber){
        *pPortNumber = mPort;
        return mName;
    }
    float currentAngle(){ return mCurrentAngle;}
    void setCurrentAngle(float angle){ mCurrentAngle = angle;}
    ///
    /// \brief set/get pules width for minimum/maximum/center angle
    /// \param us pules width in micro-seconds
    ///
    void setMinPw(float us){mMinPw =  us;}
    void setMaxPw(float us){mMaxPw =   us;}
    void setCenterPw(float us){mCenterPw =   us;}
    float minPw(){ return   mMinPw;}
    float maxPw(){ return  mMaxPw;}
    float centerPw(){ return  mCenterPw;}

    ///
    /// \brief set/get minimum/maximum/center angle
    /// \param deg angle in degree
    ///
    void setMinAngle(float deg){mMinAngle = deg;}
    void setMaxAngle(float deg){mMaxAngle = deg;}
    void setCenterAngle(float deg){mCenterAngle = deg;}
    float minAngle(){ return mMinAngle;}
    float maxAngle(){ return mMaxAngle;}
    float centerAngle(){ return mCenterAngle;}

    ///
    /// \brief do the calibration. Called after all parameters are set.
    ///
    void calibration();

    ///
    /// \brief find the impulse width by the desired move angle
    /// \param deg
    /// \return the impulse width in micro-seconds
    ///
    int findPwByAngle(float deg);
protected:
    int mPort;
    char mName[64];
    float mMinAngle;
    float mMaxAngle;
    float mCenterAngle;
    float mCurrentAngle;
    float     mMinPw;
    float     mMaxPw;
    float     mCenterPw;
private:
    //y=ax+b
    float   ma1,ma2,mb1,mb2;
};

#endif // SERVO_H
