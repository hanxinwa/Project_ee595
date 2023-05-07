`include "package.sv"
`include "interface.sv"
`include "sequence_item.sv"

class base_seq extends uvm_sequence#(seq_item);
  seq_item req;
  `uvm_object_utils(base_seq)
  
  function new (string name = "base_seq");
    super.new(name);
  endfunction

  task body();
    `uvm_info(get_type_name(), "Base seq: Inside Body", UVM_LOW);
    #3;
    `uvm_do_with(req, {req.ip == 3'b011;});
    #3;
    `uvm_do_with(req, {req.ip == 3'b111;});
    #3;
    `uvm_do_with(req, {req.ip == 3'b100;});
    #3;
    `uvm_do_with(req, {req.ip == 3'b100;});
    #2;
    `uvm_do_with(req, {req.ip == 3'b011;});
    #1;
    `uvm_do_with(req, {req.ip == 3'b100;});
    #3;
    `uvm_do_with(req, {req.ip == 3'b100;});
    #3;
    `uvm_do_with(req, {req.ip == 3'b100;});
  endtask
endclass