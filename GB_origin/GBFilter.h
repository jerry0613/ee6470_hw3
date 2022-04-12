#ifndef GB_FILTER_H_
#define GB_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class GBFilter: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool > i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<24> >::in i_rgb;
	cynw_p2p< sc_dt::sc_uint<24> >::out o_result;
#else
	sc_fifo_in< sc_dt::sc_uint<24> > i_rgb;
	sc_fifo_out< sc_dt::sc_uint<24> > o_result;
#endif

	SC_HAS_PROCESS( GBFilter );
	GBFilter( sc_module_name n );
	~GBFilter();
private:
	void do_filter();
  	int val_r, val_g, val_b;
};
#endif
