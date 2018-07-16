//
//  MainScene.cpp
//  MyCppGame-mobile
//
//  Created by 畑俊樹 on 2018/07/16.
//

#include "MainScene.hpp"

USING_NS_CC;

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
    
}

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    return scene;
}

bool MainScene::init()
{
    if (! Layer::init()) {
        return false;
    }
    
    auto director = Director::getInstance();
    auto size = director->getWinSize();
    auto background = Sprite::create("background.png");
    background->setPosition(Vec2(size.width / 2.0f, size.height / 2.0f));
    this->addChild(background);
    
    return true;
}
