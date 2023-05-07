// mostly self explanatory
#include <stdlib.h>
#include <iostream>
#include <cstdlib>

// routines and write waveforms to a VCD
#include <verilated.h>
#include <verilated_vcd_c.h>
// contains the top class of our Verilated ALU module
#include "Vtop.h"

#define MAX_SIM_TIME 600
#define VERIF_START_TIME 15
vluint64_t sim_time = 0;
vluint64_t clk_cnt = 0;

void set_rand_input(Vtop *dut, vluint64_t &sim_time)
{
  if (sim_time > VERIF_START_TIME)
  {
    dut->in1_sent = rand() % 2;
    dut->in2_sent = rand() % 2;
    dut->in3_sent = rand() % 2;
  }
}

// void clk_gen(Vtop *dut, vluint64_t &sim_time){
//     dut->clk_sent ^= 1;       // Invert clock
//     dut->eval();              // Evaluate dut on the current edge [clk edge]
// }

void clk_tik(Vtop *dut, vluint64_t &sim_time)
{
  clk_cnt++;
  if (clk_cnt == 10)
  {
    clk_cnt = 0;
  }
  dut->clk_sent = (clk_cnt == 0);
}

void clk_tok(Vtop *dut, vluint64_t &sim_time)
{
  if (clk_cnt == 11)
  {
    dut->clk_sent = 1;
  }
}

int main(int argc, char **argv, char **env)
{
  // rand seed
  srand(time(NULL));
  // instantiate the module
  Verilated::commandArgs(argc, argv);
  Vtop *dut = new Vtop;

  // The following lines set up the waveform dumping
  Verilated::traceEverOn(true);
  VerilatedVcdC *m_trace = new VerilatedVcdC;
  // we create an m_trace obj and pass
  // it on to our dut, The parameter 5 simply limits the depth
  // of the trace to 5 levels down the device under test.
  dut->trace(m_trace, 5);
  m_trace->open("waveform.vcd");

  // makes the simulation happen
  while (sim_time < MAX_SIM_TIME)
  {
    // dut_reset(dut, sim_time);

    // Invert clock
    clk_tik(dut, sim_time);
    clk_tok(dut, sim_time);

    set_rand_input(dut, sim_time);

    // if(sim_time == 13){

    //     dut->in1_sent = 1;
    //     dut->in2_sent = 0;
    //     dut->in3_sent = 1;
    // }

    // if(sim_time == 17){

    //     dut->in1_sent = 0;
    //     dut->in2_sent = 1;
    //     dut->in3_sent = 0;
    // }

    // if(sim_time == 19){

    //     dut->in1_sent = 0;
    //     dut->in2_sent = 0;
    //     dut->in3_sent = 0;
    // }

    dut->eval();
    // if(dut->clk_sent == 1){
    //     posedge_cnt++;
    // }

    // posedge_cnt++;        // Increment posedge counter if clk is 1
    m_trace->dump(sim_time); // Dump to waveform.vcd
    sim_time++;              // Advance simulation time

    // detect the $finish()
    if (Verilated::gotFinish())
      exit(0);
  }

  m_trace->close();
  delete dut;
  exit(EXIT_SUCCESS);
}
