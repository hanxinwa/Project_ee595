`include "package.sv"
`include "./base_test.sv"
`include "interface.sv"
`define pw 1
`timescale 1ps/1ps

module tb;
    and_if vif();
    top test_and3(vif.out, vif.clk, vif.in1, vif.in2, vif.in3);
    data_bucket db(vif.clk, vif.out);
    always
    begin
        #2 vif.clk.send();
    end

    initial begin
        uvm_config_db#(virtual and_if)::set(uvm_root::get(), "", "vif", vif);
    end

    initial begin
        run_test("base_test");
    end
endmodule

module data_bucket(interface clkin, interface L);
    always @(posedge clkin.sent) begin
    	if(L.data == 1) begin
       	$display("Output Receive a signal!");
       	L.clearReceived();
       	end
    end
endmodule