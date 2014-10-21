#include "HelloWorldScene.h"


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
#include <mapnik/image_util.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/config_error.hpp>

#include <string>
#include <iostream>


USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
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
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);


	//添加一个按钮，【添加数据】
	 CCMenuItemImage *pAddItem = CCMenuItemImage::create("Add.gif","Add.gif",this,menu_selector(HelloWorld::menuAddCallback));
	 pAddItem->setPosition(ccp(pAddItem->getContentSize().width/2 , visibleSize.height-pAddItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pAddMenu = CCMenu::create(pAddItem, NULL);
    pAddMenu->setPosition(CCPointZero);
    this->addChild(pAddMenu, 1);

	int rtn = runMap();


    
    
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
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

void HelloWorld::menuAddCallback(CCObject* pSender)
{
	//加载shp数据

	//加载渲染配置
	renderMap("");
	//显示
}

int HelloWorld::renderMap(string path)
{
	using namespace mapnik;// 使用 mapnik 命名空间

	try {
		std::cout << " running demo ... /n";
		// 注册数据源插件
		datasource_cache::instance().register_datasources("mapnik/input");
        freetype_engine::register_font("mapnik/fonts/DejaVuSans.ttf");
		std::vector<std::string> names = freetype_engine::face_names();
		std::cout << "face names size:" << names.size() << "/n";
		for (unsigned i = 0; i < names.size(); ++i)
		{
			std::cout << names[i] << "/n";
		}
		// 声明 Map 对象，包括绘制使用的设备大小，及投影信息
		Map m(1024,1024,"+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +no_defs");
		// 加载 Map 配置文件
		load_map(m,"Map.xml");
		// 设置地图绘制地理范围
		m.zoom_to_box(box2d<double>(12934400,4862620,12944200,4872400));
		// 定义具体绘制的内存图片缓冲区
		image_32 buf(m.width(),m.height());
		// 使用 agg_renderer 渲染绘制
		agg_renderer<image_32> ren(m,buf);
		ren.apply();
		// 保存到文件
		save_to_file<image_data_32>(buf.data(),"demo256.png","png256");
		//save_map(m,"map.xml",false);
		std::cout << "One maps have been rendered using AGG in the current directory:/n"
            "Have a look!/n";
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

int HelloWorld::runMap()
{
	using namespace mapnik;
    const std::string srs_lcc="+proj=lcc +ellps=GRS80 +lat_0=49 +lon_0=-95 +lat+1=49 +lat_2=77 \
                           +datum=NAD83 +units=m +no_defs";
    const std::string srs_merc="+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 \
                           +y_0=0.0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs +over";

    try {
        datasource_cache::instance().register_datasources("mapnik/input");
        freetype_engine::register_font("mapnik/fonts/DejaVuSans.ttf");

        Map m(800,600);
        m.set_background(parse_color("white"));
        m.set_srs(srs_merc);
        // create styles

        // Provinces (polygon)
        feature_type_style provpoly_style;

        rule provpoly_rule_on;
        provpoly_rule_on.set_filter(parse_expression("[NAME_EN] = 'Ontario'"));
        provpoly_rule_on.append(polygon_symbolizer(color(250, 190, 183)));
        provpoly_style.add_rule(provpoly_rule_on);

        rule provpoly_rule_qc;
        provpoly_rule_qc.set_filter(parse_expression("[NOM_FR] = 'Québec'"));
        provpoly_rule_qc.append(polygon_symbolizer(color(217, 235, 203)));
        provpoly_style.add_rule(provpoly_rule_qc);

        m.insert_style("provinces",provpoly_style);

        // Provinces (polyline)
        feature_type_style provlines_style;

        stroke provlines_stk (color(0,0,0),1.0);
        provlines_stk.add_dash(8, 4);
        provlines_stk.add_dash(2, 2);
        provlines_stk.add_dash(2, 2);

        rule provlines_rule;
        provlines_rule.append(line_symbolizer(provlines_stk));
        provlines_style.add_rule(provlines_rule);

        m.insert_style("provlines",provlines_style);

        // Drainage
        feature_type_style qcdrain_style;

        rule qcdrain_rule;
        qcdrain_rule.set_filter(parse_expression("[HYC] = 8"));
        qcdrain_rule.append(polygon_symbolizer(color(153, 204, 255)));
        qcdrain_style.add_rule(qcdrain_rule);

        m.insert_style("drainage",qcdrain_style);

        // Roads 3 and 4 (The "grey" roads)
        feature_type_style roads34_style;
        rule roads34_rule;
        roads34_rule.set_filter(parse_expression("[CLASS] = 3 or [CLASS] = 4"));
        stroke roads34_rule_stk(color(171,158,137),2.0);
        roads34_rule_stk.set_line_cap(ROUND_CAP);
        roads34_rule_stk.set_line_join(ROUND_JOIN);
        roads34_rule.append(line_symbolizer(roads34_rule_stk));
        roads34_style.add_rule(roads34_rule);

        m.insert_style("smallroads",roads34_style);


        // Roads 2 (The thin yellow ones)
        feature_type_style roads2_style_1;
        rule roads2_rule_1;
        roads2_rule_1.set_filter(parse_expression("[CLASS] = 2"));
        stroke roads2_rule_stk_1(color(171,158,137),4.0);
        roads2_rule_stk_1.set_line_cap(ROUND_CAP);
        roads2_rule_stk_1.set_line_join(ROUND_JOIN);
        roads2_rule_1.append(line_symbolizer(roads2_rule_stk_1));
        roads2_style_1.add_rule(roads2_rule_1);

        m.insert_style("road-border", roads2_style_1);

        feature_type_style roads2_style_2;
        rule roads2_rule_2;
        roads2_rule_2.set_filter(parse_expression("[CLASS] = 2"));
        stroke roads2_rule_stk_2(color(255,250,115),2.0);
        roads2_rule_stk_2.set_line_cap(ROUND_CAP);
        roads2_rule_stk_2.set_line_join(ROUND_JOIN);
        roads2_rule_2.append(line_symbolizer(roads2_rule_stk_2));
        roads2_style_2.add_rule(roads2_rule_2);

        m.insert_style("road-fill", roads2_style_2);

        // Roads 1 (The big orange ones, the highways)
        feature_type_style roads1_style_1;
        rule roads1_rule_1;
        roads1_rule_1.set_filter(parse_expression("[CLASS] = 1"));
        stroke roads1_rule_stk_1(color(188,149,28),7.0);
        roads1_rule_stk_1.set_line_cap(ROUND_CAP);
        roads1_rule_stk_1.set_line_join(ROUND_JOIN);
        roads1_rule_1.append(line_symbolizer(roads1_rule_stk_1));
        roads1_style_1.add_rule(roads1_rule_1);
        m.insert_style("highway-border", roads1_style_1);

        feature_type_style roads1_style_2;
        rule roads1_rule_2;
        roads1_rule_2.set_filter(parse_expression("[CLASS] = 1"));
        stroke roads1_rule_stk_2(color(242,191,36),5.0);
        roads1_rule_stk_2.set_line_cap(ROUND_CAP);
        roads1_rule_stk_2.set_line_join(ROUND_JOIN);
        roads1_rule_2.append(line_symbolizer(roads1_rule_stk_2));
        roads1_style_2.add_rule(roads1_rule_2);
        m.insert_style("highway-fill", roads1_style_2);

        // Populated Places

        feature_type_style popplaces_style;
        rule popplaces_rule;
        text_symbolizer popplaces_text_symbolizer(parse_expression("[GEONAME]"),"DejaVu Sans Book",10,color(0,0,0));
        popplaces_text_symbolizer.set_halo_fill(color(255,255,200));
        popplaces_text_symbolizer.set_halo_radius(1);
        popplaces_rule.append(popplaces_text_symbolizer);
        popplaces_style.add_rule(popplaces_rule);

        m.insert_style("popplaces",popplaces_style );

        // layers
        // Provincial  polygons
        {
            parameters p;
            p["type"]="shape";
            p["file"]="data/boundaries";
            p["encoding"]="latin1";

            layer lyr("Provinces");
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.add_style("provinces");
            lyr.set_srs(srs_lcc);
            m.addLayer(lyr);
        }

        // Drainage
        {
            parameters p;
            p["type"]="shape";
            p["file"]="data/qcdrainage";
            layer lyr("Quebec Hydrography");
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.set_srs(srs_lcc);
            lyr.add_style("drainage");
            m.addLayer(lyr);
        }

        {
            parameters p;
            p["type"]="shape";
            p["file"]="data/ontdrainage";
            layer lyr("Ontario Hydrography");
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.set_srs(srs_lcc);
            lyr.add_style("drainage");
            m.addLayer(lyr);
        }

        // Provincial boundaries
        {
            parameters p;
            p["type"]="shape";
            p["file"]="data/boundaries_l";
            layer lyr("Provincial borders");
            lyr.set_srs(srs_lcc);
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.add_style("provlines");
            m.addLayer(lyr);
        }

        // Roads
        {
            parameters p;
            p["type"]="shape";
            p["file"]="data/roads";
            layer lyr("Roads");
            lyr.set_srs(srs_lcc);
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.add_style("smallroads");
            lyr.add_style("road-border");
            lyr.add_style("road-fill");
            lyr.add_style("highway-border");
            lyr.add_style("highway-fill");

            m.addLayer(lyr);
        }
        // popplaces
        {
            parameters p;
            p["type"]="shape";
            p["file"]="data/popplaces";
            p["encoding"] = "latin1";
            layer lyr("Populated Places");
            lyr.set_srs(srs_lcc);
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.add_style("popplaces");
            m.addLayer(lyr);
        }

        m.zoom_to_box(box2d<double>(-8024477.28459,5445190.38849,-7381388.20071,5662941.44855));

        image_32 buf(m.width(),m.height());
        agg_renderer<image_32> ren(m,buf);
        ren.apply();
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

		/*save_to_file(buf,"demo.png","png");
        save_to_file(buf,"demo.jpg","jpeg");
        save_to_file(buf,"demo.png","png");
        save_to_file(buf,"demo256.png","png8");
        save_to_file(buf,"demo.tif","tiff");*/
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

