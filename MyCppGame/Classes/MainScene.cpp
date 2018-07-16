//
//  MainScene.cpp
//  MyCppGame-mobile
//
//  Created by 畑俊樹 on 2018/07/16.
//

#include "MainScene.hpp"

USING_NS_CC;

const float kFruitTopMargin = 80.0f;
const int kFruitSpawnRate = 20;

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
        newPosition = newPosition.getClampPoint(Vec2(0.0, position.y), Vec2(winSize.width, position.y));
        // ↓ フルーツの生成範囲とかも考慮する必要があるので、一旦コメントイン
//        auto winSize = Director::getInstance()->getWinSize();
//        auto halfWidth = _player->getContentSize().width * _player->getScaleX() / 2;
//        auto mixX = 0.0f + halfWidth;
//        auto maxX = winSize.width - halfWidth;
//        newPosition = newPosition.getClampPoint(Vec2(mixX, position.y), Vec2(maxX, position.y));
        
        _player->setPosition(newPosition);
    };
    director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // 毎フレupdateを呼ぶ
    this->scheduleUpdate();
    
    return true;
}

void MainScene::update(float delta)
{
    int random = rand() % kFruitSpawnRate;
    if (random == 0) {
        this->addFruit();
    }
    
    for (auto& fruit : _fruits) {
        Vec2 basketPositon = _player->getPosition() - Vec2(0, -10);
        Rect boundingBox = fruit->getBoundingBox();
        bool isHit = boundingBox.containsPoint(basketPositon);
        if (isHit) {
            this->catchFruit(fruit);
        }
    }
}

Sprite* MainScene::addFruit()
{
    // フルーツをランダム生成
    int fruitType = rand() % static_cast<int>(FruitType::kCount);
    std::string fileName = StringUtils::format("fruit%d.png", fruitType);
    auto fruit = Sprite::create(fileName);
    if (fruit == NULL) {
        return NULL;
    }
    fruit->setTag(fruitType);
    
    // フルーツの初期位置
    auto winSize = Director::getInstance()->getWinSize();
    auto fruitSize = fruit->getContentSize();
    float fruitXPos = rand() % static_cast<int>(winSize.width);
    fruit->setPosition(Vec2(fruitXPos, winSize.height - kFruitTopMargin));
    
    this->addChild(fruit);
    _fruits.pushBack(fruit);
    
    // 落下 & 削除の動きを付ける
    auto ground = Vec2(fruitXPos, 0);
    auto fall = MoveTo::create(3, ground);
    auto remove = CallFuncN::create([this](Node* node) {
        auto sprite = dynamic_cast<Sprite*>(node);
        if (sprite == NULL) return;
        this->removeFruit(sprite);
    });
    auto sequence = Sequence::create(fall, remove, NULL);
    fruit->runAction(sequence);
    
    return fruit;
}

bool MainScene::removeFruit(Sprite* fruit)
{
    if (fruit == NULL) return false;
    if (! _fruits.contains(fruit)) return false;
    
    fruit->removeFromParent();
    _fruits.eraseObject(fruit);
    return true;
}

void MainScene::catchFruit(Sprite* fruit)
{
    // 一旦フルーツを削除するだけ
    this->removeFruit(fruit);
}
