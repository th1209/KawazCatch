//
//  TitleScene.hpp
//  MyCppGame-mobile
//
//  Created by 畑俊樹 on 2018/07/28.
//

#ifndef TitleScene_hpp
#define TitleScene_hpp

#include "cocos2d.h"

class TitleScene : public cocos2d::Layer
{
private:
    TitleScene();
    virtual ~TitleScene();
    bool init() override;
    
    cocos2d::Sprite* _touchToStart;

public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(TitleScene);
    void onEnterTransitionDidFinish() override;
};

#endif /* TitleScene_hpp */
