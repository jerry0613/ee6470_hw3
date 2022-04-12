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
#if defined(MEM)
    HLS_MAP_TO_MEMORY( buffer, "mem" );
	HLS_FLATTEN_ARRAY( mask );
#endif
}

GBFilter::~GBFilter() {}

// GB mask
const sc_dt::sc_uint<2> mask[MASK_Y][MASK_X] = {{0, 1, 0}, {1, 2, 1}, {0, 1, 0}};

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
		buffer[0][i] = 0;
		buffer[1][i] = 0;
		buffer[2][i] = 0;
  	}
	while (true) {
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
		{
			HLS_CONSTRAIN_LATENCY(0, 1, "lat00");
			if (y % 3 == 0) {
				offset = 2;
				buffer[1][x] = rgb;
			} else if (y % 3 == 1) {
				offset = 0;
				buffer[2][x] = rgb;
			} else {
				offset = 1;
				buffer[0][x] = rgb;
			}
			val_r = 0;
			val_g = 0;
			val_b = 0;
		}
		// do GB filter
		if (y >= 1 && x >= 1) {
			for (sc_dt::sc_uint<2> v = 0; v < MASK_Y; ++v) {
				HLS_UNROLL_LOOP(ALL, "conv");
				index = (offset + v) % 3;
				for (sc_dt::sc_uint<2> u = 0; u < MASK_X; ++u) {
					if ((u + x - 1) >= 0 && (u + x - 1) < 256) { 
						val_r += buffer[index][u + x - 1].range(7, 0) << mask[v][u];
						val_g += buffer[index][u + x - 1].range(15, 8) << mask[v][u];
						val_b += buffer[index][u + x - 1].range(23, 16) << mask[v][u];
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
