#pragma once

#include "cocos2d.h"
#include <string>
#include <mapnik/image_util.hpp>

using namespace std;
using namespace mapnik;// 使用 mapnik 命名空间

class MapScene : public cocos2d::CCLayer
{
	public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    void menuAddCallback(CCObject* pSender);
	void menuZoomInCallback(CCObject* pSender);
    // implement the "static node()" method manually
    CREATE_FUNC(MapScene);

    int renderMap(string path);
	void AddMap(image_32 buf);

//private :
//	Map m;
};

