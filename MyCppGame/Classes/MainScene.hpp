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
        kPlaying,
        kResult
    };

public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(MainScene);

    CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite*>, _fruits, Fruits);
    CC_SYNTHESIZE(int, _score, Score);
    CC_SYNTHESIZE(float, _second, Second);
    CC_SYNTHESIZE(GameState, _state, State);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite*, _player, Player);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _scoreLabel, ScoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _secondLabel, SecondLabel);
    
    virtual void update(float delta);

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
        kCount
    };
    
    cocos2d::Sprite* addFruit();
    bool removeFruit(cocos2d::Sprite* fruit);
    void catchFruit(cocos2d::Sprite* fruit);
    void onResult();
};

#endif /* MainScene_hpp */
