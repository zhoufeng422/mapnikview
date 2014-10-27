#include "mapnix.h"

#include <stdio.h>
#include <iostream>
#include <string>

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

#include <agg_pixfmt_rgb.h>

using namespace std;
using namespace mapnik;// 使用 mapnik 命名空间

int register_info(const char *path){

	cout << " register map ... /n";

	string pluginPath(path);
	pluginPath += "mapnik\\input";

	string fontsPath(path);
	fontsPath += "mapnik\\fonts";

	try {
		// 注册数据源插件
		datasource_cache::instance().register_datasources(pluginPath);
		// 注册字体库
        freetype_engine::register_fonts(fontsPath);

		vector<string> names = freetype_engine::face_names();
		cout << "face names size:" << names.size() << "/n";
		for (unsigned i = 0; i < names.size(); ++i)
		{
			cout << names[i] << "/n";
		}
     }
     catch ( const config_error & ex )
     {
         cerr << "### Configuration error: " << ex.what() << endl;
         return EXIT_FAILURE;
     }
     catch ( const exception & ex )
     {
         cerr << "### exception: " << ex.what() << endl;
         return EXIT_FAILURE;
     }
     catch ( ... )
     {
         cerr << "### Unknown exception." << endl;
         return EXIT_FAILURE;
     }

     return EXIT_SUCCESS;
}

void *load_map_file(const char *filename,int width, int height)
{
    cout<<"loading "<< filename << endl;

	// 声明 Map 对象，包括绘制使用的设备大小，及投影信息
    /*unsigned width = 800;
    unsigned height = 600;*/
	Map *map = new Map(width,height);//(width,height);

	try
	{
		// 加载地图配置信息
		load_map(*map,filename);

		// 设置地图绘制地理范围
		if(&map)
		{
			map->zoom_all();		// 全部区域
			box2d<double> const& ext = map->get_current_extent();
			map->zoom_to_box(ext);
		}
	}
	catch (config_error & ex)
	{
		cout << ex.what() << "\n";
		return NULL;
	}
	catch (...)
	{
		cerr << "Exception caught in load_map\n";
		return NULL;
	}

	return map;
}

void render_agg(void *map,unsigned char &buffer){
	if(!map){
		cerr << "Map is MULL!\n";
	}

	Map *map_ptr = (Map*) map;
	unsigned width=map_ptr->width();
	unsigned height=map_ptr->height();

	// 定义具体绘制的内存图片缓冲区
	image_32 buf(width,height);
	agg_renderer<image_32> ren(*map_ptr,buf);

	try
	{
		// 使用agg_renderer生成地图缓存
		ren.apply();

		// 保存到文件或缓冲区
		//save_to_file<image_data_32>(buf.data(),"demo256.png","png256");
		int imgl = sizeof(unsigned);
		int len = width*height*imgl;

		//memset(pdata,0,len);
		memcpy(&buffer,buf.raw_data(),len);

		cout << "One maps have been rendered using AGG!/n";
	}
	catch (config_error & ex)
	{
		cerr << ex.what() << endl;
	}
	catch (const exception & ex)
	{
		cerr << "exception: " << ex.what() << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception caught!\n";
	}
}

int map_destroy(void *map){
	if(map){
		try{
			// 释放地图对象
			delete map;
		}
		catch (const exception & ex)
		{
			cerr << "exception: " << ex.what() << endl;
			return EXIT_FAILURE;
		}
		catch (...)
		{
			cerr << "Unknown exception caught!\n";
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}