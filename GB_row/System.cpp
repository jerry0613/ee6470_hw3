#include "System.h"
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), GB_filter("GB_filter"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	GB_filter.i_clk(clk);
	GB_filter.i_rst(rst);
	tb.o_rgb(rgb);
	tb.i_result(result);
	GB_filter.i_rgb(rgb);
	GB_filter.o_result(result);
	tb.o_x(x);
	tb.o_y(y);
	GB_filter.i_x(x);
	GB_filter.i_y(y);

  tb.read_bmp(input_bmp);
}

System::~System() {
  tb.write_bmp(_output_bmp);
}
