///////////////////////////////////////////
//////// Zhiyuan Ning 06/26/2019  ////////////
///////  add modules in SV 
/////////////////////////////////////////////////
`define tgate 14

//import SFQ_PKG::*;
`include "./SFQ_PKG.sv"
`include "./gate_delay.sv"

`timescale 1ps/1ps

// adding interface here ***
`timescale 1ps/1ps
// adding SFQ here ***
module buffer(SFQ clkin, in, out);
	
	PropagationDelay gPD (); 
//	timingcheck2 TC (clkin.data, in1.data, in2.data);
	
	always @(posedge clkin.sent) begin
		if(in.data == 1)begin
			out.send();
		end
		in.clearReceived();
	end

/*
	always begin	
		fork
			in.receive();
		join_none
		@(posedge clkin.sent);
		in.clearReceived();
	end
*/

endmodule //buffer

