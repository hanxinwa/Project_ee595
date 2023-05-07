`define tgate 14
`include "./SFQ_PKG.sv"

`timescale 1ps/1ps
module GateDelay (out, in);
   input in;
   output out;
   buf g0 (out, in);
   
   specify
      specparam delay = `tgate;
      (in => out) = delay;
   endspecify


endmodule

`timescale 1ps/1ps
module PropagationDelay;
   logic in, out, state;
   GateDelay g1 (out,in);
   initial begin
      state = 1'b0;
      in = 1'b0;
   end
   
   task tPD ();
      in = !state;
      while(out!=in) #1;
   endtask

endmodule
   