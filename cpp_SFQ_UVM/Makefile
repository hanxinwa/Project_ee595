# ====================INFORMATION===========================
# Makefile for Verilator-C++
# Maolin Xiong 6727554115
# May/2023
# ====================INSTRUCTIONS==========================
# make verilate  : verilating the design using verilator
# make build : build the verilated source using build
# make waves: use gtkwave to check output waveform
# make clean: clean previous compiled files 
# 
# =================ORIGINAL COMMANDS========================
# verilator -Wall --trace --x-assign unique --x-initial unique -cc --timing top.sv --exe tb_top_uvm.cpp
# make -j -C obj_dir -f Vtop.mk Vtop
# obj_dir/Vtop


MODULE=top

#rebuild the entire simualtion
.PHONY:sim
sim: waveform.vcd

#verilating the design using verilator
.PHONY:verilate
verilate: .stamp.verilate

#build the verilated source using build
.PHONY:build
build: obj_dir/Vtop

#open GTKWave to check the waveform
.PHONY:waves
waves: waveform.vcd
	@echo
	@echo "### WAVES ###"
	gtkwave waveform.vcd


#Lastly, we need to pass +verilator+rand+reset+2 to our simulation executable
waveform.vcd: ./obj_dir/V$(MODULE)
	@echo
	@echo "### SIMULATING ###"
	@./obj_dir/V$(MODULE) +verilator+rand+reset+2

./obj_dir/V$(MODULE): .stamp.verilate
	@echo
	@echo "### BUILDING SIM ###"
	make -C obj_dir -f V$(MODULE).mk V$(MODULE)

.stamp.verilate: $(MODULE).sv tb_$(MODULE).cpp
	@echo
	@echo "### VERILATING ###"
	verilator -Wall --trace --x-assign unique --x-initial unique -cc --timing $(MODULE).sv --exe tb_$(MODULE)_uvm.cpp
	@touch .stamp.verilate

#linting check
.PHONY:lint
lint: $(MODULE).sv
	verilator --lint-only $(MODULE).sv


#clean the verilated files
.PHONY: clean
clean:
	rm -rf .stamp.*;
	rm -rf ./obj_dir
	rm -rf waveform.vcd
