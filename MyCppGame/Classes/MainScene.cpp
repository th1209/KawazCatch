//
//  MainScene.cpp
//  MyCppGame-mobile
//
//  Created by 畑俊樹 on 2018/07/16.
//

#include "MainScene.hpp"

USING_NS_CC;

MainScene::MainScene()
: _player(NULL)
{
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_player);
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
    auto winSize = director->getWinSize();
    
    // 背景
    auto background = Sprite::create("background.png");
    background->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
    this->addChild(background);
    
    // プレイヤー
    this->setPlayer(Sprite::create("player.png"));
    _player->setPosition(winSize.width / 2.0f, winSize.height / 2.0f - 425.0f);
    this->addChild(_player);
    
    // タッチを登録
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) {
        return true;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        
        if (_player == NULL) return;
        
        // フリックした分だけ移動
        Vec2 position = _player->getPosition();
        Vec2 delta = touch->getDelta();
        Vec2 newPosition = position + delta;
        
        // 移動範囲を画面領域内に切り詰める
        auto winSize = Director::getInstance()->getWinSize();
        auto halfWidth = _player->getContentSize().width * _player->getScaleX() / 2;
        auto mixX = 0.0f + halfWidth;
        auto maxX = winSize.width - halfWidth;
        newPosition = newPosition.getClampPoint(Vec2(mixX, position.y), Vec2(maxX, position.y));
        
        _player->setPosition(newPosition);
    };
    director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}
