//
//  MainScene.hpp
//  MyCppGame-mobile
//
//  Created by 畑俊樹 on 2018/07/16.
//

#ifndef MainScene_hpp
#define MainScene_hpp

#include "cocos2d.h"

class MainScene : public cocos2d::Layer {
private:
    enum class GameState
    {
        kReady,
        kPlaying,
        kEnding,
        kResult
    };

public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(MainScene);

    CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite*>, _fruits, Fruits);
    CC_SYNTHESIZE(int, _score, Score);
    CC_SYNTHESIZE(float, _second, Second);
    CC_SYNTHESIZE(bool, _isCrash, IsCrash);
    CC_SYNTHESIZE(GameState, _state, State);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite*, _player, Player);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _scoreLabel, ScoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _secondLabel, SecondLabel);
    
    void update(float delta);
    void onEnterTransitionDidFinish() override;

protected:
    MainScene();
    virtual ~MainScene();
    bool init() override;
    
private:
    enum class FruitType
    {
        kApple,
        kGrape,
        kOrange,
        kBanana,
        kCherry,
        kGolden,
        kBomb,
        kCount
    };
    
    void addReadyLabel();
    cocos2d::Sprite* addFruit();
    bool removeFruit(cocos2d::Sprite* fruit);
    void catchFruit(cocos2d::Sprite* fruit);
    void onCatchBomb();
    void onResult();
};

#endif /* MainScene_hpp */
