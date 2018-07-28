//
//  MainScene.cpp
//  MyCppGame-mobile
//
//  Created by 畑俊樹 on 2018/07/16.
//

#include "MainScene.hpp"
#include "TitleScene.hpp"
#include "SimpleAudioEngine.h"

USING_NS_CC;

// 描画位置に関する定数群
const float kFruitTopMargin = 80.0f;

// 制限時間
const float kTimeLimitSecond = 60.0f;

// スコアに関する定数群
const int kNormalFruitScore = 1;
const int kGoldenFruitScore = 5;
const int kBombPenaltyScore = -4;

// UserDefalultのキー値に関する定数群
const char* kHighScoreKey = "highScoreKey";

// 毎フレのフルーツ生成確率に関する定数群
const float kFruitSpawnIncreaseBase = 0.02f;
const float kFruitSpawnIncreateRate = 1.05f;
const float kMaximumSpawnProbability = 0.5f;

// どのフルーツを生成するかの確率に関する定数群
const int kFruitSpawnRate = 20;
const float kGoldenFruitProbabilityBase = 0.02f;
const float kGoldenFruitProbabilityRate = 0.001f;
const float kBombProbabilityBase = 0.05f;
const float kBombProbabilityRate = 0.003f;

// フルーツの個数
const int kNormalFruitCount = 5;

MainScene::MainScene()
: _fruits(Vector<cocos2d::Sprite*>())
, _score(0)
, _isCrash(false)
, _second(kTimeLimitSecond)
, _state(GameState::kReady)
, _engine()
, _player(NULL)
, _scoreLabel(NULL)
, _secondLabel(NULL)
, _fruitBatchNode(NULL)
{
    std::random_device rdev;
    _engine.seed(rdev());
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_secondLabel);
    CC_SAFE_RELEASE_NULL(_fruitBatchNode);
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
        if (this->getIsCrash()) return;
        
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
    
    // BatchNode(フルーツ)
    _fruitBatchNode = SpriteBatchNode::create("fruits.png");
    if (_fruitBatchNode == nullptr) {
        return false;
    }
    this->addChild(_fruitBatchNode);
    
    // 毎フレupdateを呼ぶ
    this->scheduleUpdate();
    
    return true;
}

void MainScene::update(float delta)
{
    if (_state == GameState::kPlaying) {
        // フルーツの生成
        float pastTime = kTimeLimitSecond - _second;
        float p = kFruitSpawnIncreaseBase * (1 + powf(kGoldenFruitProbabilityRate, pastTime));
        p = MIN(p, kMaximumSpawnProbability);
        float random = this->generateRandom(0.0f, 1.0f);
        if (random < p) {
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
            _state = GameState::kEnding;
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("finish.mp3");
            
            auto finish = Sprite::create("finish.png");
            auto winSize = Director::getInstance()->getWinSize();
            finish->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
            finish->setScale(0);
            
            auto appear = EaseExponentialIn::create(ScaleTo::create(0.25f, 1.0f));
            auto disappear = EaseExponentialIn::create(ScaleTo::create(0.25f, 0.0f));
            finish->runAction(Sequence::create(appear,
                                               DelayTime::create(2.0f),
                                               disappear,
                                               CallFunc::create([this]() -> void{
                                                   _state = GameState::kResult;
                                                   this->onResult();
                                               }),
                                               RemoveSelf::create(),
                                               NULL));
            this->addChild(finish);
        }
    }
}

void MainScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);
    this->addReadyLabel();
}

void MainScene::addReadyLabel()
{
    auto director = Director::getInstance();
    auto winSize = director->getWinSize();
    auto center = Vec2(winSize.width/2.0f, winSize.height/2.0f);
    
    // スタートボタン.
    auto start = Sprite::create("start.png");
    auto startAction1 = CCSpawn::create(EaseIn::create(ScaleTo::create(0.5f, 5.0f), 0.5f),
                                       FadeOut::create(0.5f),
                                       NULL
    );
    start->setPosition(center);
    start->retain();
    start->runAction(Sequence::create(startAction1, RemoveSelf::create(), NULL));
    
    
    // レディボタン.
    auto ready = Sprite::create("ready.png");
    ready->setScale(0.0f);
    ready->setPosition(center);
    ready->runAction(Sequence::create(ScaleTo::create(0.25f, 1.0f),
                                      DelayTime::create(1.0f),
                                      CallFunc::create([this, start]()->void{
                                          // addChildした時点でActionが実行される.
                                          this->addChild(start);
                                          _state = GameState::kPlaying;
                                          CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("play.mp3");
                                      }),
                                      RemoveSelf::create(),
                                      NULL));
    this->addChild(ready);
}

Sprite* MainScene::addFruit()
{
    if (_fruitBatchNode == nullptr) {
        return nullptr;
    }
    
    // 経過秒数に応じて、出現するフルーツを変動させる.
    //int fruitType = rand() % static_cast<int>(FruitType::kCount);
    int fruitType = 0;
    float r = this->generateRandom(0.0f, 1.0f);
    int pastSecond = kTimeLimitSecond - _second;
    float goldenFruitProbability = kGoldenFruitProbabilityBase + kGoldenFruitProbabilityRate * pastSecond;
    float bombProbability = kBombProbabilityBase + kBombProbabilityRate * pastSecond;
    if (r <= goldenFruitProbability) {
        fruitType = static_cast<int>(FruitType::kGolden);
    }else if (r <= bombProbability) {
        fruitType = static_cast<int>(FruitType::kBomb);
    }else {
        fruitType = static_cast<int>(floor(this->generateRandom(0, kNormalFruitCount)));
    }
    
    // フルーツを生成
    auto textureSize = _fruitBatchNode->getTextureAtlas()->getTexture()->getContentSize();
    auto fruitWidth = textureSize.width / static_cast<int>(FruitType::kCount);
    auto fruit = Sprite::create("fruits.png", Rect(fruitWidth * fruitType, 0, fruitWidth, textureSize.height));
    if (fruit == NULL) {
        return NULL;
    }
    fruit->setTag(fruitType);
    
    // フルーツの初期位置
    auto winSize = Director::getInstance()->getWinSize();
    auto fruitSize = fruit->getContentSize();
    float fruitXPos = rand() % static_cast<int>(winSize.width);
    fruit->setPosition(Vec2(fruitXPos, winSize.height - kFruitTopMargin));
    
    _fruitBatchNode->addChild(fruit);
    _fruits.pushBack(fruit);
    
    // アニメーションを付けながら、落下開始.
    fruit->setScale(0.0f);
    auto ground = Vec2(fruitXPos, 0);
    auto fall = MoveTo::create(3, ground);
    auto remove = CallFuncN::create([this](Node* node) {
        auto sprite = dynamic_cast<Sprite*>(node);
        if (sprite == NULL) return;
        this->removeFruit(sprite);
    });
    auto swing = Repeat::create(Sequence::create(RotateTo::create(0.25f, -30),
                                                 RotateTo::create(0.25f, 30),
                                                 NULL), 2);
    auto sequence = Sequence::create(ScaleTo::create(0.25f, 1.0f),
                                     swing,
                                     RotateTo::create(0.125f, 0),
                                     fall,
                                     remove,
                                     NULL);
    fruit->runAction(sequence);
    
    return fruit;
}

bool MainScene::removeFruit(Sprite* fruit)
{
    if (fruit == NULL) return false;
    if (! _fruits.contains(fruit)) return false;
    
    _fruits.eraseObject(fruit);
    fruit->removeFromParent();
    return true;
}

void MainScene::catchFruit(Sprite* fruit)
{
    if (this->getIsCrash()) return;
    
    FruitType fruitType = static_cast<FruitType>(fruit->getTag());
    switch (fruitType) {
        case MainScene::FruitType::kGolden:
            _score += kGoldenFruitScore;
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("catch_golden.mp3");
            break;
        case MainScene::FruitType::kBomb:
            this->onCatchBomb();
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("catch_bomb.mp3");
            break;
        default:
            _score += kNormalFruitScore;
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("catch_fruit.mp3");
            break;
    }
    
    this->removeFruit(fruit);
    
    _scoreLabel->setString(StringUtils::toString(_score));
}

void MainScene::onResult()
{
    // 状態の変更
    _state = GameState::kResult;
    
    // 各種リセットボタンの表示
    auto winSize = Director::getInstance()->getWinSize();
    auto replayButton = MenuItemImage::create("replay_button.png",
                                              "replay_button_pressed.png",
                                              [](Ref* ref) {
                                                  CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
                                                  
                                                  auto scene = MainScene::createScene();
                                                  auto transition = TransitionFade::create(0.5, scene);
                                                  Director::getInstance()->replaceScene(transition);
                                              });
    auto titleButton = MenuItemImage::create("title_button.png",
                                             "title_button_pressed.png",
                                             [](Ref* ref) {
                                                 CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
                                                 
                                                 auto scene = TitleScene::createScene();
                                                 auto transition = TransitionCrossFade::create(1.0, scene);
                                                 Director::getInstance()->replaceScene(transition);
                                             });
    auto menu = Menu::create(replayButton, titleButton, NULL);
    menu->alignItemsVerticallyWithPadding(15);
    menu->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
    this->addChild(menu);
    
    // ハイスコアの更新
    auto userDefault = UserDefault::getInstance();
    int highScore = userDefault->getIntegerForKey(kHighScoreKey);
    if (_score > highScore) {
        userDefault->setIntegerForKey(kHighScoreKey, _score);
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("finish.mp3");
}

void MainScene::onCatchBomb()
{
    // クラッシュ状態に.
    _isCrash = true;
    
    // 3枚ワンセットの画像を元にアニメーションする.
    const int animationFrameCount = 3;
    Vector<SpriteFrame*> frames;
    auto playerSize = _player->getContentSize();
    for (int i = 0; i < animationFrameCount; ++i) {
        auto rect = Rect(playerSize.width * i, 0, playerSize.width, playerSize.height);
        auto frame = SpriteFrame::create("player_crash.png", rect);
        frames.pushBack(frame);
    }
    
    // 10フレの * 3枚 * 3回 = 90フレのアニメーション.
    auto animation = Animation::createWithSpriteFrames(frames, 10.0 / 60.0);
    animation->setLoops(3);
    
    // 終わったら元のスプライトに戻す.
    animation->setRestoreOriginalFrame(true);
    
    // アニメーション再生.
    auto sequence = Sequence::create(Animate::create(animation),
                                     CallFunc::create([this]{
                                         // クラッシュ状態から復帰.
                                         _isCrash = false;
                                     }),
                                     NULL);
    _player->runAction(sequence);
    
    // スコア更新.
    _score = MAX(0, _score + kBombPenaltyScore);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("crash.mp3");
}

float MainScene::generateRandom(float min, float max)
{
    // 連続一様分布で乱数を生成(min以上、max未満).
    std::uniform_real_distribution<float> dist(min, max);
    return dist(_engine);
}
