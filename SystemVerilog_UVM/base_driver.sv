`include "package.sv"
`include "interface.sv"
`include "sequence_item.sv"

class driver extends uvm_driver#(seq_item);
  virtual add_if vif;
  `uvm_component_utils(driver)
  
  function new(string name = "driver", uvm_component parent = null);
    super.new(name, parent);
  endfunction
  
  function void build_phase(uvm_phase phase);
    super.build_phase(phase);
    if(!uvm_config_db#(virtual add_if) :: get(this, "", "vif", vif))
      `uvm_fatal(get_type_name(), "Not set at top level");
  endfunction
  
  task run_phase (uvm_phase phase);
    forever begin
      // Driver to the DUT
      //@(posedge vif.clk);
      seq_item_port.get_next_item(req);
      `uvm_info(get_type_name, $sformatf("ip1 = %0d, ip2 = %0d", req.ip1, req.ip2), UVM_LOW);
      if(req.ip[0])
        in1_send();
      if(req.ip[1])
        in2_send();
      if(req.ip[2])
        in3_send();
      //vif.ip <= req.ip;
      
      //@(posedge vif.clk);
      //req.out <= vif.out;
      seq_item_port.item_done();
    end
  endtask

  task in1_send();
    vif.in1.send();
  endtask

  task in2_send();
    vif.in2.send();
  endtask

  task in3_send();
    vif.in3.send();
  endtask;
endclass