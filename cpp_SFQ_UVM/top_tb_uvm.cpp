// mostly self explanatory
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <deque>

// routines and write waveforms to a VCD
#include <verilated.h>
#include <verilated_vcd_c.h>
// contains the top class of our Verilated and3 module
#include "Vtop.h"

#define MAX_SIM_TIME 600
#define VERIF_START_TIME 15
vluint64_t sim_time = 0;
vluint64_t clk_cnt = 0;

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
            dut->in2_sent = tx->input1;
            dut->in2_sent = tx->input2;
            dut->in3_sent = tx->input3;
            // Release the memory by deleting the tx item
            // after it has been consumed
            delete tx;
        }
    }
};

class and3InMon
{
private:
    Vtop *dut;
    and3Scb *scb;

public:
    and3InMon(Vtop *dut, and3Scb *scb)
    {
        this->dut = dut;
        this->scb = scb;
    }

    void monitor()
    {
        if (dut->in_valid == 1)
        {
            // If there is valid data at the input interface,
            // create a new and3Tx transaction item and populate
            // it with data observed at the interface pins
            and3Tx *tx = new and3Tx();
            dut->in1_sent = tx->input1;
            dut->in2_sent = tx->input2;
            dut->in3_sent = tx->input3;
            // then pass the transaction item to the scoreboard
            scb->writeIn(tx);
        }
    }
};

class and3OutMon
{
private:
    Vtop *dut;
    and3Scb *scb;

public:
    and3OutMon(Vtop *dut, and3Scb *scb)
    {
        this->dut = dut;
        this->scb = scb;
    }

    void monitor()
    {
        if (dut->clk_sent == 1)
        {
            // If there is valid data at the output interface,
            // create a new and3OutTx transaction item and populate
            // it with result observed at the interface pins
            and3Tx *tx = new and3Tx();
            tx->out = dut->out_data;

            // then pass the transaction item to the scoreboard
            scb->writeOut(tx);
        }
    }
};

// and3 scoreboard
class and3Scb
{
private:
    std::deque<and3Tx *> in_q;

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

        // Grab the transaction item from the front of the input item queue
        and3Tx *in;
        in = in_q.front();
        in_q.pop_front();
        if ((in->input1 & in->input2 & in->input3) != tx->output)
        {
            std::cout << std::endl;
            std::cout << "and3Scb: and3 mismatch" << std::endl;
            std::cout << "  Expected: " << (in->input1 & in->input2 & in->input3)
                      << "  Actual: " << tx->output << std::endl;
            std::cout << "  Simtime: " << sim_time << std::endl;
        }

        // As the transaction items were allocated on the heap, it's important
        // to free the memory after they have been used
        delete in;
        delete tx;
    }
};

// void clk_gen(Vtop *dut, vluint64_t &sim_time){
//     dut->clk_sent ^= 1;       // Invert clock
//     dut->eval();              // Evaluate dut on the current edge [clk edge]
// }

void clk_tik(Vtop *dut, vluint64_t &sim_time)
{
    clk_cnt++;
    if (clk_cnt == 10)
    {
        clk_cnt = 0;
    }
    dut->clk_sent = (clk_cnt == 0);
}

void clk_tok(Vtop *dut, vluint64_t &sim_time)
{
    if (clk_cnt == 11)
    {
        dut->clk_sent = 1;
    }
}

int main(int argc, char **argv, char **env)
{
    // rand seed
    srand(time(NULL));
    // instantiate the module
    Verilated::commandArgs(argc, argv);
    Vtop *dut = new Vtop;
    // The following lines set up the waveform dumping
    Verilated::traceEverOn(true);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    // we create an m_trace obj and pass
    // it on to our dut, The parameter 5 simply limits the depth
    // of the trace to 5 levels down the device under test.
    dut->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

    and3Tx *tx;
    and3InDrv *drv = new and3InDrv(dut);
    and3Scb *scb = new and3Scb();
    and3InMon *inMon = new and3InMon(dut, scb);
    and3OutMon *outMon = new and3OutMon(dut, scb);

    // makes the simulation happen
    while (sim_time < MAX_SIM_TIME)
    {
        // dut_reset(dut, sim_time);

        // Invert clock for simulating the SFQ circuit
        clk_tik(dut, sim_time);
        clk_tok(dut, sim_time);

        if (sim_time >= VERIF_START_TIME)
        {
            // Generate a randomized transaction item of type
            tx = rndand3Tx();

            // Pass the transaction item to the And3 input interface driver,
            // which drives the input interface based on the info in the
            // transaction item
            drv->drive(tx);

            // Monitor the input interface
            inMon->monitor();

            // Monitor the output interface
            outMon->monitor();
        }

        dut->eval();

        m_trace->dump(sim_time); // Dump to waveform.vcd
        sim_time++;              // Advance simulation time

        // detect the $finish()
        if (Verilated::gotFinish())
            exit(0);
    }

    m_trace->close();
    delete dut;
    delete outMon;
    delete inMon;
    delete scb;
    delete drv;
    exit(EXIT_SUCCESS);
}
