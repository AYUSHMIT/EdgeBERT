 /*
  */

#include <systemc.h>
#include <mc_scverify.h>
#include <nvhls_int.h>

#include <vector>
#include "SMax.h"
#include "../include/Spec.h"
#include "../include/utils.h"
#include "../include/helper.h"

#define NVHLS_VERIFY_BLOCKS (SMax)
#include <nvhls_verify.h>

using namespace::std;

SC_MODULE(Source) {
  sc_in<bool> clk;
  sc_in<bool> rst; 

  typedef DecodeTop MaskMemType;
  typedef AuxMem AuxMemType;

  Connections::Out<bool> start;
  Connections::Out<spec::InputBufferConfig> input_buffer_config;  // input buffer configurations
  Connections::Out<spec::PeriphConfig> softmax_config;
  Connections::Out<spec::GBControlConfig> gbcontrol_config;
  Connections::Out<spec::VectorType>  act_rsp; //Activation response from DecodeTop
  Connections::Out<spec::VectorType>   attn_span_rsp;  // To AuxMem for read response

  Connections::In<AuxMemType::aux_req_t>  attn_span_req;  // To AuxMem for read request
  Connections::In<spec::VectorType>  act_out_vec; // To be stored in DecodeTop
  Connections::In<MaskMemType::mask_req_t>  mask_rd_req; // To DecodeTop for mask read requests
  Connections::In<bool> done;

  Connections::Combinational<AuxMemType::aux_req_t>  softmax_param_wr;  // To AuxMem for read request
  Connections::Combinational<spec::VectorType>  act_out_wr; // To be stored in DecodeTop
  Connections::Combinational<MaskMemType::mask_req_t>  mask_rd_wr; // To DecodeTop for mask read requests

  bool start_src; 
  
  spec::VectorType  act_rsp_reg;
  spec::VectorType  attn_span_rsp_reg;
  spec::PeriphConfig softmax_config_reg;
  spec::GBControlConfig gbcontrol_config_reg;
    
  SC_CTOR(Source) {
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);

    SC_THREAD(runOut);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);
  }


  void runOut(){
    attn_span_req.Reset();
    act_out_vec.Reset();
    mask_rd_req.Reset();
    done.Reset();
    softmax_param_wr.ResetWrite();
    act_out_wr.ResetWrite();
    mask_rd_wr.ResetWrite();

    wait();
 
    while (1) {
      bool done_dest;
      AuxMemType::aux_req_t softmax_param_reg;
      spec::VectorType act_reg;
      MaskMemType::mask_req_t  mask_reg;


      if (act_out_vec.PopNB(act_reg)) {
        cout << hex << sc_time_stamp() << " TB - Popped act_out_vec_reg data"  << endl;
        act_out_wr.Push(act_reg);
      }

      if (attn_span_req.PopNB(softmax_param_reg)) {
        cout << hex << sc_time_stamp() << " TB - Popped softmax_param_reg data" << endl;
        softmax_param_wr.Push(softmax_param_reg);
      }

      if (mask_rd_req.PopNB(mask_reg)) {
        cout << hex << sc_time_stamp() << " TB - Popped mask_reg data" << endl;
        mask_rd_wr.Push(mask_reg);
      }

      if (done.PopNB(done_dest)) {
        cout << hex << sc_time_stamp() << " Done signal issued !!!!" << endl;
        sc_stop();
      }
      
      wait();    
    }
  }

  void run(){
    input_buffer_config.Reset();
    softmax_config.Reset();
    gbcontrol_config.Reset();
    act_rsp.Reset();
    attn_span_rsp.Reset();
    start.Reset();

    softmax_param_wr.ResetRead();
    act_out_wr.ResetRead();
    mask_rd_wr.ResetRead();

    AuxMemType::aux_req_t  softmax_param_reg;  // To AuxMem for read request
    spec::VectorType  act_reg; // To be stored in DecodeTop
    MaskMemType::mask_req_t  mask_reg; // To DecodeTop for mask read requests

    wait();
    spec::InputBufferConfig input_buffer_config_reg;
    input_buffer_config_reg.base_input[0] = 0;
    input_buffer_config_reg.base_input[1] = 0;
    input_buffer_config.Push(input_buffer_config_reg);

    wait(20);
    softmax_config_reg.base_attn_span = 0;
    softmax_config_reg.base_gamma = 16;
    softmax_config_reg.base_beta = 32;
    softmax_config_reg.adpbias_attn_span = 0;
    softmax_config_reg.adpbias_beta = 2;
    softmax_config_reg.adpbias_gamma = 2;
    softmax_config.Push(softmax_config_reg);
    wait(20);

    gbcontrol_config_reg.num_vector = 2;
    gbcontrol_config_reg.num_timestep = 1;
    gbcontrol_config_reg.adpbias_act1 = 2;
    gbcontrol_config_reg.adpbias_act2 = 2;
    gbcontrol_config_reg.adpbias_act3 = 2;
    gbcontrol_config.Push(gbcontrol_config_reg); 
    wait(20);

    wait();

    start_src = 1;
    start.Push(start_src);
    wait(4);

    //Timestep 1
    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);


    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    softmax_param_reg = softmax_param_wr.Pop(); 
    attn_span_rsp_reg = set_bytes<16>("00_00_00_02_FF_00_00_A0_00_10_00_01_CC_00_00_01");
    attn_span_rsp.Push(attn_span_rsp_reg); 

    act_reg = act_out_wr.Pop();

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    softmax_param_reg = softmax_param_wr.Pop(); 
    attn_span_rsp_reg = set_bytes<16>("00_00_00_02_FF_00_00_A0_00_10_00_01_CC_00_00_01");
    attn_span_rsp.Push(attn_span_rsp_reg); 

    act_reg = act_out_wr.Pop();

    //Timestep 2
    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);


    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    softmax_param_reg = softmax_param_wr.Pop(); 
    attn_span_rsp_reg = set_bytes<16>("00_00_00_02_FF_00_00_A0_00_10_00_01_CC_00_00_01");
    attn_span_rsp.Push(attn_span_rsp_reg); 

    act_reg = act_out_wr.Pop();

    mask_reg = mask_rd_wr.Pop();
    act_rsp_reg = set_bytes<16>("00_00_00_02_00_00_00_B0_00_10_00_01_00_00_00_01");
    act_rsp.Push(act_rsp_reg);

    softmax_param_reg = softmax_param_wr.Pop(); 
    attn_span_rsp_reg = set_bytes<16>("00_00_00_02_FF_00_00_A0_00_10_00_01_CC_00_00_01");
    attn_span_rsp.Push(attn_span_rsp_reg); 

    act_reg = act_out_wr.Pop(); 
  }
};

SC_MODULE(testbench) {
  SC_HAS_PROCESS(testbench);
	sc_clock clk;
  sc_signal<bool> rst;

  typedef DecodeTop MaskMemType;
  typedef AuxMem AuxMemType;
 
  Connections::Combinational<bool> start;
  Connections::Combinational<spec::InputBufferConfig> input_buffer_config;  // input buffer configurations
  Connections::Combinational<spec::PeriphConfig> softmax_config;
  Connections::Combinational<spec::GBControlConfig> gbcontrol_config;
  Connections::Combinational<spec::VectorType>  act_rsp; //Activation response from DecodeTop
  Connections::Combinational<spec::VectorType>   attn_span_rsp;  // To AuxMem for read response

  Connections::Combinational<AuxMemType::aux_req_t>  attn_span_req;  // To AuxMem for read request
  Connections::Combinational<spec::VectorType>  act_out_vec; // To be stored in DecodeTop
  Connections::Combinational<MaskMemType::mask_req_t>  mask_rd_req; // To DecodeTop for mask read requests
  Connections::Combinational<bool> done;
 
  NVHLS_DESIGN(SMax) dut;
  Source  source;
  
  testbench(sc_module_name name)
  : sc_module(name),
    clk("clk", 1.0, SC_NS, 0.5, 0, SC_NS, true),
    rst("rst"),
    dut("dut"),
    source("source")
  {
    dut.clk(clk);
    dut.rst(rst);
    dut.start(start);
    dut.input_buffer_config(input_buffer_config);
    dut.softmax_config(softmax_config);
    dut.gbcontrol_config(gbcontrol_config);
    dut.act_rsp(act_rsp);
    dut.attn_span_rsp(attn_span_rsp);
    dut.attn_span_req(attn_span_req);
    dut.act_out_vec(act_out_vec);
    dut.mask_rd_req(mask_rd_req);
    dut.done(done);
    
    source.clk(clk);
    source.rst(rst);
    source.start(start);
    source.input_buffer_config(input_buffer_config);
    source.softmax_config(softmax_config);
    source.gbcontrol_config(gbcontrol_config);
    source.act_rsp(act_rsp);
    source.attn_span_rsp(attn_span_rsp);
    source.attn_span_req(attn_span_req);
    source.act_out_vec(act_out_vec);
    source.mask_rd_req(mask_rd_req);
    source.done(done);
	  
    SC_THREAD(run);
  }
  

  void run(){
	  wait(2, SC_NS );
    std::cout << "@" << sc_time_stamp() <<" Asserting reset" << std::endl;
    rst.write(false);
    wait(2, SC_NS );
    rst.write(true);
    std::cout << "@" << sc_time_stamp() <<" De-Asserting reset" << std::endl;
    wait(10000, SC_NS );
    std::cout << "@" << sc_time_stamp() <<" sc_stop" << std::endl;
    sc_stop();
  }
};


int sc_main(int argc, char *argv[]) {
  cout << "Vector Size = " << spec::kVectorSize << endl;
  
  testbench tb("tb");
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_start();

  bool rc = (sc_report_handler::get_count(SC_ERROR) > 0);
  if (rc)
    DCOUT("TESTBENCH FAIL" << endl);
  else
    DCOUT("TESTBENCH PASS" << endl);
  return rc;
}
