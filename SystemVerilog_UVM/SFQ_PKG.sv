///////////////////////////////////////////
//////// Ramy Tadros 8/15/2018 ////////////
/////// SFQ Interface - Version-5
//////////////////////////////////////////
/* verilator lint_off ASSIGNDLY */
/* verilator lint_off DECLFILENAME */
/* verilator lint_off IMPORTSTAR */
/* verilator lint_off UNDRIVEN */
/* verilator lint_off UNUSED */



//package SFQ_PKG;

// import SFQ_PKG::*;
// endpackage : SFQ_PKG


`timescale 1ps/1ps
interface SFQ;
  parameter real pw=1;  //sfq pulse width

  logic data = 0;
  logic sent = 0;

  //modport tx (output sent, import send);
  //modport rx (input sent, output data, import receive);

  //sent input

  always @(posedge sent) begin
    data <= 1'b1;
    sent <= 1'b0;
  end
  
  task send ();
    sent <= 1'b1;
  endtask
  
  /*
  task receive();
    if(sent == 1) begin
      edgedect <= 1'b1;
      data <= 1'b1;
      data <= #pw 1'b0;
      sent <= 1'b0;
    end
  endtask
*/
  task clearReceived();
    //input dump;
    if (sent == 1) data <= 1'b1;
    else data <= 1'b0;
  endtask

endinterface : SFQ


