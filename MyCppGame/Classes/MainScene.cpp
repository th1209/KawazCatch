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
    
    return true;
}
