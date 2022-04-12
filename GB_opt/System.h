#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "GBFilter_wrap.h"
#else
#include "GBFilter.h"
#endif

class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name n, std::string input_bmp, std::string output_bmp );
	~System();
private:
  Testbench tb;
#ifndef NATIVE_SYSTEMC
	GBFilter_wrapper GB_filter;
#else
	GBFilter GB_filter;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<24> > rgb;
	cynw_p2p< sc_dt::sc_uint<9> > x;
	cynw_p2p< sc_dt::sc_uint<9> > y;
	cynw_p2p< sc_dt::sc_uint<24> > result;
#else
	sc_fifo< sc_dt::sc_uint<24> > rgb;
	sc_fifo< sc_dt::sc_uint<9> > x;
	sc_fifo< sc_dt::sc_uint<9> > y;
	sc_fifo< sc_dt::sc_uint<24> > result;
#endif

	std::string _output_bmp;
};
#endif
