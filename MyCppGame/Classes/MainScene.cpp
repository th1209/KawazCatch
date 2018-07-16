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
const float kTimeLimitSecond = 10.0f;

MainScene::MainScene()
: _fruits(Vector<cocos2d::Sprite*>())
, _score(0)
, _second(kTimeLimitSecond)
, _state(GameState::kPlaying)
, _player(NULL)
, _scoreLabel(NULL)
, _secondLabel(NULL)
{
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_secondLabel);
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
    
    // スコアラベル
    float fontSize = 32.0f;
    auto scoreLabel = Label::createWithSystemFont(StringUtils::toString(_score), "Marker Felt", fontSize);
    this->setScoreLabel(scoreLabel);
    scoreLabel->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 6);
    scoreLabel->enableOutline(Color4B::BLACK, 3);
    scoreLabel->setPosition(Vec2(winSize.width / 2.0f * 1.5f, winSize.height - 150));
    this->addChild(_scoreLabel);
    
    auto scoreLabelHeader = Label::createWithSystemFont("SCORE", "Marker Felt", fontSize);
    scoreLabelHeader->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 6);
    scoreLabelHeader->enableOutline(Color4B::BLACK, 3);
    scoreLabelHeader->setPosition(Vec2(winSize.width / 2.0f * 1.5f, winSize.height - 120));
    this->addChild(scoreLabelHeader);
    
    // タイマーラベル
    int second = static_cast<int>(_second);
    auto secondLabel = Label::createWithSystemFont(StringUtils::toString(second), "Marker Felt", fontSize);
    this->setSecondLabel(secondLabel);
    secondLabel->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 6);
    secondLabel->enableOutline(Color4B::BLACK, 3);
    secondLabel->setPosition(Vec2(winSize.width / 2.0f, winSize.height - 150));
    this->addChild(secondLabel);
    
    auto secondLabelHeader = Label::createWithSystemFont("TIME", "Marker Felt", fontSize);
    secondLabelHeader->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 6);
    secondLabelHeader->enableOutline(Color4B::BLACK, 3);
    secondLabelHeader->setPosition(Vec2(winSize.width / 2.0f, winSize.height - 120));
    this->addChild(secondLabelHeader);
    
    
    // 毎フレupdateを呼ぶ
    this->scheduleUpdate();
    
    return true;
}

void MainScene::update(float delta)
{
    if (_state == GameState::kPlaying) {
        // フルーツの生成
        int random = rand() % kFruitSpawnRate;
        if (random == 0) {
            this->addFruit();
        }
        
        // フルーツのキャッチ判定
        for (auto& fruit : _fruits) {
            Vec2 basketPositon = _player->getPosition() - Vec2(0, -10);
            Rect boundingBox = fruit->getBoundingBox();
            bool isHit = boundingBox.containsPoint(basketPositon);
            if (isHit) {
                this->catchFruit(fruit);
            }
        }
        
        // 残り時間更新
        _second -= delta;
        int second = static_cast<int>(_second);
        if (_secondLabel != NULL) {
            _secondLabel->setString(StringUtils::toString(second));
        }
        
        // 残り時間判定
        if (_second < 0) {
            _state = GameState::kResult;
            this->onResult();
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
    this->removeFruit(fruit);
    
    _score += 1;
    _scoreLabel->setString(StringUtils::toString(_score));
}

void MainScene::onResult()
{
    // 状態の変更
    _state = GameState::kResult;
    
    // フルーツの全削除
    for (auto fruit : _fruits) {
        removeFruit(fruit);
    }
    
    // 各種リセットボタンの表示
    auto winSize = Director::getInstance()->getWinSize();
    auto replayButton = MenuItemImage::create("replay_button.png",
                                              "replay_button_pressed.png",
                                              [](Ref* ref) {
                                                  auto scene = MainScene::createScene();
                                                  Director::getInstance()->replaceScene(scene);
                                              });
    auto titleButton = MenuItemImage::create("title_button.png",
                                             "title_button_pressed.png",
                                             [](Ref* ref) {
                                                 // 後で実装する.
                                             });
    auto menu = Menu::create(replayButton, titleButton, NULL);
    menu->alignItemsVerticallyWithPadding(15);
    menu->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
    this->addChild(menu);
}
