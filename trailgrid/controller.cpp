#include "controller.h"
#include "bot.h"

#include <iostream>

Controller::Controller(double width, double height, double cellSize, double cellScale) :
    grid(width, height),
    scene((width/2)*-1, (height/2)*-1, width, height, cellSize, cellScale),
    view(&scene),
    QObject()
{
    thread = new QThread();
}

Controller::~Controller()
{
    delete bot;
}

void Controller::setBot(Bot *bot)
{
    this->bot = bot;
    this->connect(this->bot, SIGNAL(moving()), this, SLOT(updateGrid()));
    this->bot->connect(thread, SIGNAL(started()), this->bot, SLOT(doTeleOp()));
    //this->bot->connect(thread, SIGNAL(started()), this->bot, SLOT(doWallFollowing()));
    this->bot->moveToThread(thread);
}

void Controller::runBot()
{
    thread->start();
}

void Controller::updateGrid()
{
    double size = scene.getCellSize(), scale = scene.getCellScale();
    int x = round(bot->getX()/scale), y = round(bot->getY()/scale);
    std::cout << "x" << x << " = " << size*(round(x/size))
              << " y" << y << " = " << size*(round(y/size)) << std::endl;
    x = round(x/size);
    y = round(y/size);
    this->updateBotCell(x, y);
    this->updateTrailCell(x-1, y);
    this->updateTrailCell(x-1, y-1);
    this->updateTrailCell(x-1, y+1);
    this->updateTrailCell(x+1, y);
    this->updateTrailCell(x+1, y-1);
    this->updateTrailCell(x+1, y+1);
    this->updateTrailCell(x, y-1);
    this->updateTrailCell(x, y+1);
    this->drawBotView();
}

void Controller::updateBotCell(double x, double y)
{
    double size = scene.getCellSize();
    grid.assign(x, y, 1);
    scene.drawBotRect(size*x, size*y*-1);
}

void Controller::updateTrailCell(double x, double y)
{
    if (grid.at(x, y) != NULL) {
        double size = scene.getCellSize();
        grid.assign(x, y, 2);
        scene.drawTrailRect(size*x, size*y*-1);
    }
}

void Controller::drawBotView()
{
    if (!view.isVisible()) {
        view.show();
    }
    view.update();
}
