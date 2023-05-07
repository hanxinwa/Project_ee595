// mostly self explanatory
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <math.h>

//routines and write waveforms to a VCD
#include <verilated.h>
#include <verilated_vcd_c.h>
//contains the top class of our Verilated and3 module
#include "Vtop.h"

#define MAX_SIM_TIME 600
#define VERIF_START_TIME 15
vluint64_t sim_time = 0;
vluint64_t clk_cnt = 0;
vluint64_t posedge_cnt = 0;
vluint64_t clk_cnt2 =0;


// transaction, we encapsulate the item into the tx
class and3Tx
{
public:
    bool input1;
    bool input2;
    bool input3;
    bool output;
};

// Transaction item generator
and3Tx *rndand3Tx()
{
    // 20% chance of generating a transaction
    if (rand() % 5 == 0)
    {
        and3Tx *tx = new and3Tx();
        tx->input1 = rand() % 2; // generate a in range 0 or 1
        tx->input2 = rand() % 2; // generate b in range 0 or 1
        tx->input3 = rand() % 2; // generate c in range 0 or 1
        return tx;
    }
    else
    {
        return NULL;
    }
}



class and3InDrv
{
private:
    Vtop *dut;

public:
    and3InDrv(Vtop *dut)
    {
        this->dut = dut;
    }

    void drive(and3Tx *tx)
    {
        // Don't drive anything if a transaction item doesn't exist
        if (tx != NULL)
        {
            dut->in1_sent = tx->input1;
            dut->in2_sent = tx->input2;
            dut->in3_sent = tx->input3;
            // Release the memory by deleting the tx item
            // after it has been consumed
            delete tx;
        }
        else{
            dut->in1_sent = 0;
            dut->in2_sent = 0;
            dut->in3_sent = 0;
        }
    }
};

class and3OutDrv
{
private:
    Vtop *dut;

public:
    and3OutDrv(Vtop *dut)
    {
        this->dut = dut;
    }

    void drive()
    {

        // read the output once it available
        if (dut->out_data)
        {
            dut->out_read = 1;
        }
        else{
            dut->out_read = 0; 
        }
    }
};


// and3 scoreboard
class and3Scb
{
private:
    std::deque<and3Tx *> in_q;
    // std::deque<and3Tx *> out_q;

public:
    // Input interface monitor port
    void writeIn(and3Tx *tx)
    {
        // Push the received transaction item into a queue for later
        in_q.push_back(tx);
    }


    // Output interface monitor port
    void writeOut(and3Tx *tx)
    {
        // We should never get any data from the output interface
        // before an input gets driven to the input interface
        if (in_q.empty())
        {
            std::cout << "Fatal Error in and3Scb: empty and3Tx queue" << std::endl;
            exit(1);
        }
        // else if(){

        // }

        // Grab the transaction item from the front of the input item queue
        and3Tx *in;
        in = in_q.front();
        in_q.pop_front();
        if ((in->input1 & in->input2 & in->input3) != tx->output)
        {
            std::cout << "--------------------------"<<std::endl;
            std::cout << "-[UVM_SCB]: and3 mismatch-" << std::endl;
            std::cout << "- Expected output:"<< (in->input1 & in->input2 & in->input3) <<"       -" << std::endl;
            std::cout << "- Actual output:"<< tx->output<<"        -"<< std::endl;
            std::cout << "- Simtime:"<< sim_time << "            -"  << std::endl;
            std::cout << "--------------------------"<<std::endl;
        }
        else{
            std::cout << std::endl;
            std::cout << "[UVM_SCB]: and3 match" << std::endl;
            std::cout << "  Expected output: " << (in->input1 & in->input2 & in->input3)
                      << "  Actual output: " << tx->output << std::endl;
            std::cout << "  Simtime: " << sim_time << std::endl;
        }

        // As the transaction items were allocated on the heap, it's important
        // to free the memory after they have been used
        delete in;
        delete tx;
    }
};

class and3InMon
{
private:
    Vtop *dut;
    and3Scb *scb;
    bool _in1;
    bool _in2;
    bool _in3;

public:
    and3InMon(Vtop *dut, and3Scb *scb)
    {
        this->dut = dut;
        this->scb = scb;
    }

    void monitor()
    {
        //only when there is a expected 1 we write it into scb dequeue
        if(dut->clk_sent == 1){
            and3Tx *tx = new and3Tx();           
            tx->input1 = _in1;
            tx->input2 = _in2;
            tx->input3 = _in3;
            // then pass the transaction item to the scoreboard
            scb->writeIn(tx);
            _in1 = 0;
            _in2 = 0;
            _in3 = 0;
           // delete tx;
        }
        // If there is valid data at the input interface,
        // create a new and3Tx transaction item and populate
        // it with data observed at the interface pins
        else{
            if(dut->in1_sent){
                _in1 = 1;
            }
            if(dut->in2_sent){
                _in2 = 1;
            }
            if(dut->in3_sent){
                _in3 = 1;
            }
        }
        // delete tx;

    }
};

class and3OutMon {
    private:
        Vtop *dut;
        and3Scb *scb;
    public:
        and3OutMon(Vtop *dut, and3Scb *scb){
            this->dut = dut;
            this->scb = scb;
        }

        void monitor(){
            //dut->clk_sent == 1 &&
            //You may delay the compare as there is a buffer within the and3
            if ((dut->clk_sent == 1) &&(sim_time > VERIF_START_TIME+10)) {
                // If there is valid data at the output interface,
                // create a new and3OutTx transaction item and populate
                // it with result observed at the interface pins
                and3Tx *tx = new and3Tx();
                tx->output = dut->out_data;

                // then pass the transaction item to the scoreboard to compare
                scb->writeOut(tx);
            }
        }
};




// void clk_gen(Vtop *dut, vluint64_t &sim_time){
//     dut->clk_sent ^= 1;       // Invert clock
//     dut->eval();              // Evaluate dut on the current edge [clk edge]
// }

void clk_tik(Vtop *dut, vluint64_t &sim_time){
    clk_cnt++;
    if(clk_cnt == 10){
         clk_cnt= 0;
    }
    dut->clk_sent = (clk_cnt == 0);

}


void clk_tok(Vtop *dut, vluint64_t &sim_time){
    clk_cnt2++;
    if(clk_cnt2 == 11){
        clk_cnt2 =0;
    }
}




int main(int argc, char** argv, char** env) {
    //rand seed
    srand(time(NULL)); 

    //instantiate the module
    Verilated::commandArgs(argc,argv);
    Vtop *dut = new Vtop;
    //The following lines set up the waveform dumping
    Verilated::traceEverOn(true);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    //we create an m_trace obj and pass 
    //it on to our dut, The parameter 5 simply limits the depth 
    //of the trace to 5 levels down the device under test.
    dut->trace(m_trace, 5); 
    m_trace->open("waveform.vcd");


    and3Tx *tx;
    and3InDrv  *Indrv  = new and3InDrv(dut);
    and3OutDrv *Outdrv = new and3OutDrv(dut);
    and3Scb    *scb    = new and3Scb();
    and3InMon  *inMon  = new and3InMon(dut,scb);
    and3OutMon *outMon = new and3OutMon(dut,scb);

    //makes the simulation happen
    while (sim_time < MAX_SIM_TIME) {

        // Invert clock for simulating the SFQ circuit clk
        clk_tik(dut, sim_time);
        clk_tok(dut, sim_time);
        
        if (sim_time >= VERIF_START_TIME) {
            // Generate a randomized transaction item of type 
            tx = rndand3Tx();

            // Pass the transaction item to the And3 input interface driver,
            // which drives the input interface based on the info in the
            // transaction item
            Indrv->drive(tx);

            Outdrv->drive();
            
            // Monitor the input interface
            inMon->monitor();

            // Monitor the output interface
            outMon->monitor();
        }
        
        
        

        dut->eval(); 
        m_trace->dump(sim_time);  // Dump to waveform.vcd
        sim_time++;               // Advance simulation time


        //detect the $finish()
        if (Verilated::gotFinish()) exit(0);
    }




    m_trace->close();
    delete dut;
    delete outMon;
    delete inMon;
    delete scb;
    delete Indrv;
    delete Outdrv;
    exit(EXIT_SUCCESS);
}
