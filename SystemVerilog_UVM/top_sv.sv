/* verilator lint_off COMBDLY */
/* verilator lint_off UNOPTFLAT */
// top module for systemverilog verify
//original top file
`define Ninputs 9
`define Noutputs 5
`define thold 0
`define tsetup 5
`define tsplit 7
`define tconf 7
`define tcoinc 7
`define tjtl 4
`define HLevels 0
`define seq_len 1000
`define tgate 14
`define LastCLK 18
`define pw 1
`define inp_delay `tgate

`include "./SFQ_PKG.sv"
`include "./and_modules.sv"
`include "./buf_modules.sv"

`timescale 1ps/1ps


module top(SFQ out, clk, in1, in2, in3);

   SFQ #(`pw) out1();
   SFQ #(`pw) out2();
   
   // Instance the interface with an input, using named connection

   and2 g0(clk, in1, in2, out1);
   buffer b0(clk, in3, out2);                             
   and2 g1(clk, out1, out2, out);


endmodule
   