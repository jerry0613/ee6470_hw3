#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "GBFilter.h"

GBFilter::GBFilter( sc_module_name n ): sc_module( n )
{
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
	i_x.clk_rst(i_clk, i_rst);
	i_y.clk_rst(i_clk, i_rst);
  	o_result.clk_rst(i_clk, i_rst);
#endif
#if defined(REG)
    HLS_MAP_TO_REG_BANK( buffer_r, "mem_r" );
	HLS_MAP_TO_REG_BANK( buffer_g, "mem_g" );
	HLS_MAP_TO_REG_BANK( buffer_b, "mem_b" );
	HLS_FLATTEN_ARRAY( mask );
#endif
}

GBFilter::~GBFilter() {}

// GB mask
const sc_dt::sc_uint<3> mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

void GBFilter::do_filter() {
	{
	#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		i_x.reset();
		i_y.reset();
		o_result.reset();
	#endif
		wait();
	}
	for (int i = 0; i < 256; ++i) {
		buffer_r[0][i] = 0;
		buffer_r[1][i] = 0;
		buffer_r[2][i] = 0;
		buffer_g[0][i] = 0;
		buffer_g[1][i] = 0;
		buffer_g[2][i] = 0;
		buffer_b[0][i] = 0;
		buffer_b[1][i] = 0;
		buffer_b[2][i] = 0;
  	}
	while (true) {
		val_r = 0;
      	val_g = 0;
      	val_b = 0;
		#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("input");
			x = i_x.get();
			y = i_y.get();
			rgb = i_rgb.get();
			wait();
		}
		#else
			x = i_x.read();
			y = i_y.read();
			rgb = i_rgb.read();
		#endif
		// row buffer
			buffer_r[0][x] = buffer_r[1][x];
			buffer_g[0][x] = buffer_g[1][x];
			buffer_b[0][x] = buffer_b[1][x];
			buffer_r[1][x] = buffer_r[2][x];
			buffer_g[1][x] = buffer_g[2][x];
			buffer_b[1][x] = buffer_b[2][x];
			buffer_r[2][x] = rgb.range(7, 0);
			buffer_g[2][x] = rgb.range(15, 8);
			buffer_b[2][x] = rgb.range(23, 16);
		// do GB filter
		if (y >= 1 && x >= 1) {
			for (unsigned int v = 0; v < MASK_Y; ++v) {
				for (unsigned int u = 0; u < MASK_X; ++u) {
					if ((u + x - 1) >= 0 && (u + x - 1) < 256) {
						val_r += buffer_r[v][u + x - 1] * mask[v][u];
						val_g += buffer_g[v][u + x - 1] * mask[v][u];
						val_b += buffer_b[v][u + x - 1] * mask[v][u];
					}
				}
			}
				rgb.range(7, 0) = val_r >> 4;
				rgb.range(15, 8) = val_g >> 4;
				rgb.range(23, 16) = val_b >> 4;
			#ifndef NATIVE_SYSTEMC
				HLS_DEFINE_PROTOCOL("output");
				o_result.put(rgb);
				wait();
			#else
				o_result.write(rgb);
			#endif
		}
	}
}
