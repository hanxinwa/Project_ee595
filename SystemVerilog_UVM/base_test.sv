`include "package.sv"
`include "interface.sv"

class base_test extends uvm_test;
    `uvm_component_utils(base_test)

    function new(string name = "base_test", uvm_component parent = null);
        super.new(name, parent);
    endfunction

    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
    endfunction

    task run_phase(uvm_phase phase);
        phase.raise_objection(this);
        super.run_phase(phase);
    endtask;
endclass