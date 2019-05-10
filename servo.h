#ifndef SERVO_H
#define SERVO_H

#include "common.h"

class Servo
{
public:
    Servo();

    void setPort(int port){mPort = port;}

    int getPort(){
        return mPort;
    }
    void setName(char* name);

    char* getName(){
        return mName;
    }
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
    /// \brief currentAngle current angle in each servo, in single action
    ///
    float currentAngle(){ return mCurrentAngle;}
    void setCurrentAngle(int angle){ mCurrentAngle = angle;}
    float currentSpeed(){ return mCurrentSpeed;}
    void setCurrentSpeed(int speed){ mCurrentSpeed = speed;}

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
    float mCurrentSpeed;
    float     mMinPw;
    float     mMaxPw;
    float     mCenterPw;
private:
    //y=ax+b
    float   ma1,ma2,mb1,mb2;
};

///
/// \brief The Sequence class store Sequence data
///
class Sequence
{
public:
    Sequence();
};

///
/// \brief The SystemData class stores all system datas
///
class SystemData
{
public:
    SystemData();
    bool loadIni(const char* szIniName);
    bool saveIni(const char* szIniName);
    Servo* getServo(int i){
        return &mServo[i%MAX_SERVOS_NUMBER];
    }
    int singleTime(){ return mSingleTime;}
    void setSingleTime(int time){mSingleTime = time;}
protected:
    int mSingleTime; /*<! total command time for single action used in page 1,2 >*/

    Servo   mServo[MAX_SERVOS_NUMBER];
    Sequence* mpSeqList;

};

#endif // SERVO_H
