/* verilator lint_off COMBDLY */
/* verilator lint_off UNOPTFLAT */
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

`include "./SFQ_PKG.sv"
`include "./and_modules.sv"
`include "./buf_modules.sv"
`include "./top_sv.sv"
//`include "./split.sv"
//`include "./hclc_modules.sv"
`define pw 1
`timescale 1ps/1ps
`define inp_delay `tgate

module tb();

    SFQ #(`pw) clk();
    SFQ #(`pw) in1();
    SFQ #(`pw) in2();
    SFQ #(`pw) in3();
    
    SFQ #(`pw) out();

    top test_and3(out, clk, in1, in2, in3);
    data_bucket db(clk, out);

    always
    begin
        #2 clk.send();
    end

    initial begin

        #3;
        fork
            in1.send();
            in2.send();
        join

        #3;
        fork
            in1.send();
            in2.send();
            in3.send();
        join

        #3;
        fork
            in3.send();
        join

        #3;
        fork
            in3.send();
        join
        
        #2;
        fork
            in1.send();
            in2.send();
        join

        #1;
        fork
            in3.send();
        join

        #3;
        fork
            in3.send();
        join

        #3;
        fork
            in3.send();
        join
                                                                                                                                                                              
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