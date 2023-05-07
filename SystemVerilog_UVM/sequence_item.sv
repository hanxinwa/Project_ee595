
`include "package.sv"
`include "interface.sv"
class seq_item extends uvm_sequence_item;
  rand bit[2:0] ip;
  bit [8:0] out;
  
  function new(string name = "seq_item");
    super.new(name);
  endfunction
  
  `uvm_object_utils_begin(seq_item)
    `uvm_field_int(ip,UVM_ALL_ON)
  `uvm_object_utils_end
  
//  constraint ip_c {ip1 < 100; ip2 < 100;}
endclass