#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "servo.h"
#include "inifile/inifile.h"

Servo::Servo():
    mMinAngle(MIN_ANGLE),mMaxAngle(MAX_ANGLE), mCenterAngle(CENTER_ANGLE),
    mMinPw(DEFAULT_MIN_PW), mMaxPw(DEFAULT_MAX_PW), mCenterPw(DEFAULT_CENTER_PW)
{    
    calibration();
    mCurrentAngle = mCenterAngle;
}

void Servo:: setName(char* name)
{
    if(name)
        strncpy(mName, name, sizeof(mName));
}

///
/// \brief do the calibration. Called after all parameters are set.
///
void Servo::calibration()
{
    //ma = (pw0 - pw1)/(angle0 - angle1)
    //mb = pw0 - ma * angle0;
    ma2 = (mMaxPw - mCenterPw)/(mMaxAngle - mCenterAngle);
    mb2 = mMaxPw - ma2*mMaxAngle;

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
////
/// \brief SystemData::SystemData
///
SystemData::SystemData()
{

}

bool SystemData::loadIni(const char* szIniName)
{
    void* handle  = openIniFile(szIniName, true);
    if(!handle)
        return false;
    int nMAXServo =
        GetProfileInt("system", "servoCounts", MAX_SERVOS_NUMBER, handle);
    if (nMAXServo > MAX_SERVOS_NUMBER)
        nMAXServo = MAX_SERVOS_NUMBER;

    mSingleTime =
            GetProfileInt("single", "actionTime", 1000, handle);
    char session[64];
    char szValue[256];
    int nValue;
    float fValue;
    for (int i=0; i< nMAXServo; i++) {
        sprintf(session, "servo_%d", i);
        nValue = GetProfileInt(session, "port", i, handle);
        GetProfileString(session, "name", szValue, sizeof(szValue), "", handle );
        mServo[i].setName(szValue);
        mServo[i].setPort(nValue);
        fValue = GetProfileFloat(session, "minAngle", MIN_ANGLE, handle);
        mServo[i].setMinAngle(fValue);
        fValue = GetProfileFloat(session, "maxAngle", MAX_ANGLE, handle);
        mServo[i].setMaxAngle(fValue);
        fValue = GetProfileFloat(session, "centerAngle", CENTER_ANGLE, handle);
        mServo[i].setCenterAngle(fValue);
        fValue = GetProfileFloat(session, "centerPw", DEFAULT_CENTER_PW, handle);
        mServo[i].setCenterPw(fValue);
        fValue = GetProfileFloat(session, "minPw", DEFAULT_MIN_PW, handle);
        mServo[i].setMinPw(fValue);
        fValue = GetProfileFloat(session, "maxPw", DEFAULT_MAX_PW, handle);
        mServo[i].setMaxPw(fValue);
        fValue = GetProfileFloat(session, "currentAngle", CENTER_ANGLE, handle);
        mServo[i].setCurrentAngle(fValue);
        fValue = GetProfileFloat(session, "currentSpeed", CENTER_ANGLE, handle);
        mServo[i].setCurrentSpeed(fValue);


        mServo[i].calibration();
    }

    closeIniFile(handle);
    return true;
}

bool SystemData::saveIni(const char* szIniName)
{
    void* handle  = openIniFile(szIniName, false);
    if(!handle)
        return false;
    WriteProfileInt("system", "servoCounts", MAX_SERVOS_NUMBER, handle);
    WriteProfileInt("single", "actionTime", mSingleTime, handle);
    char session[64];
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        sprintf(session, "servo_%d", i);
        WriteProfileInt(session, "port", mServo[i].getPort(), handle);
        WriteProfileString(session, "name", mServo[i].getName(), handle );

        WriteProfileFloat(session, "minAngle",  mServo[i].minAngle(), handle);
        WriteProfileFloat(session, "maxAngle",  mServo[i].maxAngle(), handle);
        WriteProfileFloat(session, "centerAngle",  mServo[i].centerAngle(), handle);
        WriteProfileFloat(session, "minPw",  mServo[i].minPw(), handle);
        WriteProfileFloat(session, "maxPw",  mServo[i].maxPw(), handle);
        WriteProfileFloat(session, "centerPw",  mServo[i].centerPw(), handle);
        WriteProfileFloat(session, "currentAngle",  mServo[i].currentAngle(), handle);
        WriteProfileFloat(session, "currentSpeed",  mServo[i].currentSpeed(), handle);

    }
    closeIniFile(handle);

    return true;
}
