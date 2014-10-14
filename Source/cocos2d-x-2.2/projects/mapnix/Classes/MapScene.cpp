#include "MapScene.h"

#include <mapnik/map.hpp>
#include <mapnik/layer.hpp>
#include <mapnik/rule.hpp>
#include <mapnik/line_symbolizer.hpp>
#include <mapnik/polygon_symbolizer.hpp>
#include <mapnik/text_symbolizer.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/graphics.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/expression.hpp>
#include <mapnik/color_factory.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/config_error.hpp>

#include <string>
#include <iostream>


USING_NS_CC;
using namespace mapnik;// 使用 mapnik 命名空间

Map m;

CCScene* MapScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    MapScene *layer = MapScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


// on "init" you need to initialize your instance
bool MapScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(MapScene::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);


	//添加一个按钮，【添加数据】
	 CCMenuItemImage *pAddItem = CCMenuItemImage::create("Add.png","Add.png",this,menu_selector(MapScene::menuAddCallback));
	 pAddItem->setPosition(ccp(pAddItem->getContentSize().width/2 , visibleSize.height-pAddItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pAddMenu = CCMenu::create(pAddItem, NULL);
    pAddMenu->setPosition(CCPointZero);
    this->addChild(pAddMenu, 1);

	//添加一个按钮，【放大】
	 CCMenuItemImage *pZoomInItem = CCMenuItemImage::create("ZoomIn.png","ZoomIn.png",this,menu_selector(MapScene::menuZoomInCallback));
	 pZoomInItem->setPosition(ccp(pZoomInItem->getContentSize().width*3/2 , visibleSize.height-pAddItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pZoomInMenu = CCMenu::create(pZoomInItem, NULL);
    pZoomInMenu->setPosition(CCPointZero);
    this->addChild(pZoomInMenu, 1);
    
    
    return true;
}


void MapScene::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}

void MapScene::menuAddCallback(CCObject* pSender)
{
	int rtn = renderMap("");

}

void MapScene::menuZoomInCallback(CCObject* pSender)
{
	m.zoom_to_box(mapnik::box2d<double>(-2881430,-1927977,2378049,2319622));
	image_32 buf(m.width(),m.height());
    // 使用 agg_renderer 渲染绘制
    agg_renderer<image_32> ren(m,buf);
	ren.apply();
	AddMap(buf);
}

/* 渲染步骤
	1、注册数据源插件
	2、注册字体
	3、声明 Map对象，包括绘制使用的设备大小，及投影信息
	4、设置或加载Map配置信息
	5、设置地图绘制地理范围
	6、定义具体绘制的内存图片缓冲区
	7、使用 agg_renderer或其它方式 渲染绘制
	8、显示渲染地图
*/
int MapScene::renderMap(string path)
{
	try {
		std::cout << " running demo ... /n";
		datasource_cache::instance().register_datasources("mapnik/input");// 注册数据源插件
        freetype_engine::register_font("fonts/MicrosoftYaHei.ttf");// 注册字体
		
		//std::vector<std::string> names = freetype_engine::face_names();
		//std::cout << "face names size:" << names.size() << "/n";
  //       for (unsigned i = 0; i < names.size(); ++i){
  //           std::cout << names[i] << "/n";
  //       }
		 // 声明 Map 对象，包括绘制使用的设备大小，及投影信息
		
         m = Map(800,600,"+proj=aea +ellps=WGS84 +lat_1=27n +lat_2=45n +lat_0=35n +lon_0=105e +x_0=0 +y_0=0");
		 m.set_background(parse_color("white"));
		 // 加载 Map 配置文件
		 load_map(m,"mapconfig.xml");
		 // 设置地图绘制地理范围
		 m.zoom_all();
		 /*mapnik::box2d<double> ext = m.get_current_extent();
		 double x0 = ext.minx();
		 double y0 = ext.miny();
		 double x1 = ext.maxx();
		 double y1 = ext.maxy();
		 m.zoom_to_box(box2d<double>(x0,y0,x1,y1));*/
		 //m.zoom_to_box(box2d<double>(-2881430,-1927977,2378049,2319622));
		 // 定义具体绘制的内存图片缓冲区
		 image_32 buf(m.width(),m.height());
         // 使用 agg_renderer 渲染绘制
         agg_renderer<image_32> ren(m,buf);
		 ren.apply();
		 AddMap(buf);
		 
     }
     catch ( const mapnik::config_error & ex )
     {
         std::cerr << "### Configuration error: " << ex.what() << std::endl;
         return EXIT_FAILURE;
     }
     catch ( const std::exception & ex )
     {
         std::cerr << "### std::exception: " << ex.what() << std::endl;
         return EXIT_FAILURE;
     }
     catch ( ... )
     {
         std::cerr << "### Unknown exception." << std::endl;
         return EXIT_FAILURE;
     }
	 return EXIT_SUCCESS;
}

void MapScene::AddMap(image_32 buf)
{
	CCImage * img = new CCImage();
	int nSize = sizeof(buf);
	bool succ = img->initWithImageData(buf.raw_data(),nSize, CCImage::kFmtRawData,m.width(),m.height(), 32);
		
	// add "HelloWorld" splash screen"
	CCTexture2D *texture = new CCTexture2D;  
	texture->initWithImage(img);  
	CCSprite* pSprite = CCSprite::createWithTexture(texture);  
	delete img;  
	texture->release();  
	// position the sprite on the center of the screen
	pSprite->setPosition(ccp(m.width()/2, m.height()/2));

	// add the sprite as a child to this layer
	this->addChild(pSprite, 0);
}