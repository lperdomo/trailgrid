#include "bot.h"
#include "keyboard.h"
#include "pid.h"

#include <iostream>

Bot::Bot(int *argc, char **argv) :
    parser(argc, argv),
    botConn(&parser, this),
    laserConn(&parser, this, &botConn),
    ArRobot(),
    QObject()
{
    velForward = 500;
}

Bot::~Bot()
{
    this->shutdown();
}

bool Bot::start()
{
    Aria::init();
    //parser.addDefaultArgument("-rh 192.168.1.11 -remoteLaserTcpPort 10002");
    this->addRangeDevice(&sonar);
    this->addRangeDevice(&sick);
    botConn.parseArgs();
    if (!botConn.connectRobot()) {
        ArLog::log(ArLog::Terse, "bot: failed to connect with");
        return false;
    }

    laserConn.setupLaser(&sick);
    if (!laserConn.connectLaser(&sick)) {
        ArLog::log(ArLog::Terse, "sick: failed to connect with");
        return false;
    }

    ArLog::log(ArLog::Normal,"bot: sucessfully connected");
    sick.runAsync();
    this->runAsync(true);

    this->lock();
    this->enableMotors();
    this->unlock();


    return true;
}

void Bot::stop()
{
    this->stop();
    this->disableMotors();
}

void Bot::shutdown()
{
    this->stop();
    this->stopRunning();
    this->waitForRunExit();
    Aria::shutdown();
}

void Bot::readingSensors()
{
    if (this->isConnected() && sick.isConnected()) {
        sick.lockDevice();
        lasers = sick.getRawReadingsAsVector();
        sick.unlockDevice();
    }
}

int Bot::getLaserRange(int angle)
{
    if (!this->lasers || (this->lasers->size() <= angle)) {
        return 0;
    }
    return this->lasers->at(angle).getRange();
}

void Bot::doTeleOp()
{
    if (!this->start()) {
        return;
    }
    double rightW, leftW;
    while (Aria::getRunning()) {
        leftW = 0;
        rightW = 0;
        if (Keyboard::getInstance()->isArrowUp()) {
            leftW = velForward;
            rightW = velForward;
        }
        if (Keyboard::getInstance()->isArrowDown()) {
            leftW = 0;
            rightW = 0;
        }
        if (Keyboard::getInstance()->isArrowLeft()) {
            leftW = velForward*0.1;
        }
        if (Keyboard::getInstance()->isArrowRight()) {
            rightW = velForward*0.1;
        }
        this->move(leftW, rightW);
        ArUtil::sleep(500);
    }
}

void Bot::doWallFollowing()
{
    if (!this->start()) {
        return;
    }

    bool following, leftFollowing = false, rightFollowing = false
    , frontWall = false, leftWall = false, rightWall = false;
    double leftReading, rightReading, frontReading, range = 1500, timeInterval = 0.5;

    PID *leftPID = new PID(1, 0.8, 0.002, 1000);//2, 1.8, 0.5
    PID *rightPID = new PID(1, 0.8, 0.002, 1000);

    while (Aria::getRunning()) {
        leftReading = sick.currentReadingPolar(30, 60);
        rightReading = sick.currentReadingPolar(-30, -60);
        frontReading = sick.currentReadingPolar(-30, 30);
        frontWall = (frontReading < range);
        leftWall = (leftReading < range);
        rightWall = (rightReading < range);
        following = false;
        if (frontWall) {
            this->move(0, 0);
            if (rightWall) {//turning left
                this->move(velForward*0.1, velForward);
            } else {//default or left wall, turn right
                this->move(velForward, velForward*0.1);
            }
        }

        if (leftWall) {//sticking to the wall
            following = true;
            leftFollowing = true;
            rightFollowing = false;
            this->correctWithPID(leftPID->calculate(leftReading, timeInterval));
        }
        if (rightWall) {
            following = true;
            rightFollowing = true;
            leftFollowing = false;
            this->correctWithPID(rightPID->calculate(rightReading, timeInterval));
        }

        if (following == false) {
            if (leftFollowing) {//small turn, just checking if it was a sharp wall edge
                this->move(velForward*0.1, velForward);
                leftFollowing = false;
            } else if (rightFollowing) {
                this->move(velForward, velForward*0.1);
                rightFollowing = false;
            } else {//wander
                this->move(velForward, velForward);
            }
        }
        ArUtil::sleep(1000*timeInterval);
    }
}

void Bot::correctWithPID(double pid)
{
    double e1 = 0, e2 = 0, v1 = velForward, v2 = velForward;

    if (pid > 0) {
        e1 = pid;
    } else {
        e2 = -pid;
    }
    if (pid > v1) {
        e1 = v1;
    } else if (pid < -v1) {
        e2 = v2;
    }
    this->move(v1-e1, v2-e2);
}

void Bot::move(double leftW, double rightW)
{
    this->lock();
    this->setVel2(leftW, rightW);
    this->moving();
    this->unlock();
}

double Bot::getVelForward()
{
    return velForward;
}

/*double x = this->getX()/5 + 500/2, y = 500/2 - this->getY()/5;
double th = atan2(x, y)*180/M_PI;
double rth = this->getTh();
double rad = (th-rth)*M_PI/180;
if (fabs(rad) > M_PI/6) rad = M_PI/2*rad/rad;
this->move(50+50*sin(rad), 50-50*sin(rad));*/
