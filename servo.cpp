#include <stdlib.h>
#include <string.h>
#include "servo.h"

Servo::Servo():
    mMinAngle(MIN_ANGLE),mMaxAngle(MAX_ANGLE), mCenterAngle(CENTER_ANGLE),
    mMinPw(DEFAULT_MIN_PW), mMaxPw(DEFAULT_MAX_PW), mCenterPw(DEFAULT_CENTER_PW)
{    
    calibration();
    mCurrentAngle = mCenterAngle;
}

void Servo:: setPortName(char* name, int port)
{
    strncpy(mName, name, sizeof(mName));
    if(port >= MAX_CONTROL_PORTS) //invalide port number
        port = 0;
    mPort = port;
}

///
/// \brief do the calibration. Called after all parameters are set.
///
void Servo::calibration()
{
    //ma = (pw0 - pw1)/(angle0 - angle1)
    //mb = pw0 - ma * angle0;
    ma2 = (mMaxPw - mCenterPw)/(mMaxAngle - mCenterAngle);
    mb2 = mMaxPw - ma2*mMinAngle;

    ma1 = (mMinPw - mCenterPw)/(mMinAngle - mCenterAngle);
    mb1 = mMinPw - ma1*mMinAngle;

}
///
/// \brief find the impulse width by the desired move angle
/// \param deg
/// \return the impulse width in micro-seconds
///
int Servo::findPwByAngle(float deg)
{
    // pw = ma* deg + mb
    if (deg < mCenterAngle) {
       return (int) (ma1*deg + mb1);
    }
    return (int) (ma2*deg + mb2);
}
