/* verilator lint_off COMBDLY */
/* verilator lint_off UNOPTFLAT */
/* verilator lint_off MULTIDRIVEN */
/* verilator lint_off STMTDLY */
/* verilator lint_off MODDUP */

// top module for verilator test

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


module top(output reg out_data, out_sent, input clk_sent, in1_sent, in2_sent, in3_sent);

   SFQ #(`pw) out1();
   SFQ #(`pw) out2();
   SFQ #(`pw) out();
   SFQ #(`pw) clk();
   SFQ #(`pw) in1();
   SFQ #(`pw) in2();
   SFQ #(`pw) in3();

   //assign data & sent

   always @(posedge clk_sent) begin
      if(clk_sent) clk.send();
   end

   always @(posedge in1_sent) begin
      if(in1_sent) in1.send();
   end

   always @(posedge in2_sent) begin
      if(in2_sent) in2.send();
   end

   always @(posedge in3_sent) begin
      if(in3_sent) in3.send();
   end

   always @(out.data) begin
      if(out.data == 1'b1) out_data <= 1;
      else out_data <= 0;
   end

   always @(out.sent) begin
      if(out.sent == 1'b1) out_sent <= 1;
      else out_sent <= 0;
   end
   
   // Instance the interface with an input, using named connection

   and2 g0(clk, in1, in2, out1);
   buffer b0(clk, in3, out2);                             
   and2 g1(clk, out1, out2, out);


endmodule
   