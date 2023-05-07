///////////////////////////////////////////
//////// Zhiyuan Ning 06/26/2019  ////////////
///////  add modules in SV 
/////////////////////////////////////////////////
`define tgate 14

//import SFQ_PKG::*;
`include "./SFQ_PKG.sv"
`include "./gate_delay.sv"


`timescale 1ps/1ps

// adding SFQ here ***
module and2 (SFQ clkin, in1, in2, out);
	
	PropagationDelay gPD (); 
//	timingcheck2 TC (clkin.data, in1.data, in2.data);
	
	always @(posedge clk.sent) begin
		if((in1.data == 1) & (in2.data == 1))begin
			//$display("start");
			//gPD.tPD();
			//$display("end");
			out.send();
		end
		in1.clearReceived();
		in2.clearReceived();
	end

/*
	always begin
		fork
			in1.receive();		// wait for high in1
		join_none
		@(posedge clkin.sent);	
		in1.clearReceived();	// reset edge detection at the very begining of the clock
	end

	always begin
		fork
			in2.receive();		// wait for high in2
		join_none
		@(posedge clkin.sent);
		in2.clearReceived();	// reset edge detection at the very begining of the clock
	end

*/
endmodule //and2

