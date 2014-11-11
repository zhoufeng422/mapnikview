#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_renderer_base.h>

#include "agg_platform_support.h"
#include "agg_win32_bmp.h"

#include "mapnix.h"

class the_application : public agg::platform_support
{
public:
    the_application(agg::pix_format_e format, bool flip_y) : 
        agg::platform_support(format, flip_y),
        pix_fmt(rbuf_window()),
        ren_bas(pix_fmt) //初始化渲染器
    {
		img_buf = NULL;
	}

    virtual void on_draw()
    {
        ren_bas.reset_clipping(true);
		ren_bas.clear(agg::rgba8(204, 204, 204));

		int width = ren_bas.width();
		int height = ren_bas.height();

		//============================================================ 
		// 以下是mapnix代码
		// 构造地图对象
		void *map_ptr = load_map_file("D:\\GitHub\\mapnikview\\Source\\bin\\mapconfig.xml",width,height);
		
		// 构建绘图缓冲区
		if(img_buf){
			delete img_buf;
		}
		int imgl = sizeof(unsigned);
		int len = width*height*imgl; 
		img_buf = new unsigned char[len];
	

		if(map_ptr){
			render_agg(map_ptr,*img_buf);
		}

		// 释放地图对象
		map_destroy(map_ptr);

		//agg::pixel_map pm;
		//pm.create(width,height,agg::org_color32);

		//============================================================ 
		// 以下是AGG代码
		agg::rendering_buffer rbufdev = rbuf_window();
		agg::rendering_buffer rbufsrc;

		//rbufdev.attach(pm.buf(), pm.width(), pm.height(), pm.stride());
		rbufsrc.attach(img_buf, width, height, -width*4); 
		rbufdev.copy_from(rbufsrc);
		//HDC dc = (HDC) this->raw_display_handler();
		//pm.draw(dc);
    }

private:
    agg::pixfmt_rgba32 pix_fmt;
    agg::renderer_base<agg::pixfmt_rgba32> ren_bas;
	unsigned char *img_buf;

};

int agg_main(int argc, char* argv[])
{
    the_application app(agg::pix_format_rgba32, true);
    app.caption("AGG MX");
    
    if(app.init(500, 500, agg::window_resize)) {
		int rc = register_info("D:\\GitHub\\mapnikview\\Source\\lib\\");
		if(rc != 0){
			return -1;
		}

        return app.run();
    }
    return -1;
}