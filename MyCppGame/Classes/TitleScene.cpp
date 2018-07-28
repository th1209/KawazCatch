//
//  TitleScene.cpp
//  MyCppGame-mobile
//
//  Created by 畑俊樹 on 2018/07/28.
//

#include "TitleScene.hpp"
#include "MainScene.hpp"
#include "SimpleAudioEngine.h"

USING_NS_CC;

TitleScene::TitleScene()
: _touchToStart(nullptr)
{
    
}

TitleScene::~TitleScene()
{
    
}

Scene* TitleScene::createScene()
{
    auto scene = cocos2d::Scene::create();
    if (scene == nullptr) {
        return nullptr;
    }
    auto layer = TitleScene::create();
    scene->addChild(layer);
    return scene;
}

bool TitleScene::init ()
{
    if (! cocos2d::Layer::init()) {
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();
    auto winSize = director->getWinSize();
    
    // 背景.
    auto background = Sprite::create("title_background.png");
    if (background == nullptr) {
        return false;
    }
    background->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
    this->addChild(background);
    
    // ロゴ.
    auto logo = Sprite::create("title_logo.png");
    if (logo == nullptr) {
        return false;
    }
    logo->setPosition(Vec2(winSize.width / 2.0f, winSize.height - 370.0f));
    this->addChild(logo);
    
    // スタートボタン.
    _touchToStart = Sprite::create("title_start.png");
    if (_touchToStart == nullptr) {
        return false;
    }
    _touchToStart->setPosition(Vec2(winSize.width / 2.0f, 180.0f));
    auto blink = Sequence::create(FadeTo::create(0.5f, 127),
                                  FadeTo::create(0.5f, 255),
                                  NULL);
    _touchToStart->runAction(RepeatForever::create(blink));
    this->addChild(_touchToStart, 1);
    
    auto listener = EventListenerTouchOneByOne::create();
    if (listener == nullptr) {
        return false;
    }
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        if (target == nullptr) {
            return false;
        }
        
        // ↓ リスナを使ったタッチ判定はこんな感じで実装する. ちょっとめんどくさい.
        Point locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size size = target->getContentSize();
        Rect rect = Rect(0, 0, size.width, size.height);
        if (! rect.containsPoint(locationInNode)) {
            return false;
        }
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
        
        // ↓ 万一2回押された時のため、タッチイベントを除外する.
        this->getEventDispatcher()->removeEventListenersForTarget(target);
        
        auto delay = DelayTime::create(0.5f);
        auto startGame = CallFunc::create([]() -> void {
            auto scene = MainScene::createScene();
            auto transition = TransitionPageTurn::create(0.5f, scene, true);
            Director::getInstance()->replaceScene(transition);
        });
        this->runAction(Sequence::create(delay, startGame, NULL));
        
        return true;
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this->_touchToStart);
    
    return true;
}

void TitleScene::onEnterTransitionDidFinish()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("title.mp3");
}
