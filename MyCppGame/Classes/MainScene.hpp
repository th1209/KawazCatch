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
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(MainScene);

protected:
    MainScene();
    virtual ~MainScene();
    bool init() override;
};

#endif /* MainScene_hpp */
