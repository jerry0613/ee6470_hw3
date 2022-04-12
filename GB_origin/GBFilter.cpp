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
  	o_result.clk_rst(i_clk, i_rst);
#endif
}

GBFilter::~GBFilter() {}

// GB mask
const int mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

void GBFilter::do_filter() {
	{
	#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_result.reset();
	#endif
		wait();
	}
	while (true) {
		{
			val_r = 0;
      		val_g = 0;
      		val_b = 0;
		}
		for (unsigned int v = 0; v < MASK_Y; ++v) {
			for (unsigned int u = 0; u < MASK_X; ++u) {
				sc_dt::sc_uint<24> rgb;
				#ifndef NATIVE_SYSTEMC
					HLS_DEFINE_PROTOCOL("input");
					rgb = i_rgb.get();
					wait();
				#else
					rgb = i_rgb.read();
				#endif
        		unsigned char r = rgb.range(7, 0);
        		unsigned char g = rgb.range(15, 8);
        		unsigned char b = rgb.range(23, 16);
        		{
          			val_r += r * mask[u][v];
          			val_g += g * mask[u][v];
          			val_b += b * mask[u][v];
				}
			}
		}
		
		val_r = val_r >> 4;
    	val_g = val_g >> 4;
    	val_b = val_b >> 4;
    	sc_dt::sc_uint<24> tmp;
		tmp.range(7, 0) = val_r;
		tmp.range(15, 8) = val_g;
		tmp.range(23, 16) = val_b;
		#ifndef NATIVE_SYSTEMC
			HLS_DEFINE_PROTOCOL("output");
			o_result.put(tmp);
			wait();
		#else
			o_result.write(tmp);
		#endif
	}
}
