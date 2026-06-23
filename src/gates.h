#ifndef GATES_H
#define GATES_H

#include <string>
#include <vector>
#include <cstdint>

/*
 * Gate-Level Digital Logic Simulator
 * 
 * Models fundamental logic gates with realistic propagation delays.
 * Delays are in picoseconds, based on typical 5V CMOS behavior
 * approximated for educational modeling.
 */

enum class GateType {
    AND2,
    OR2,
    NOT,
    NAND2,
    NOR2,
    XOR2
};

struct Gate {
    GateType type;
    std::string name;
    double propagation_delay_ps;   // in picoseconds
    
    // Connection tracking for delay calculation
    int input1_id;
    int input2_id;                 // -1 for NOT gate
    int output_id;
    
    Gate() : type(GateType::AND2), name(""), propagation_delay_ps(50.0),
             input1_id(-1), input2_id(-1), output_id(-1) {}
    
    Gate(GateType t, const std::string& n, double delay)
        : type(t), name(n), propagation_delay_ps(delay),
          input1_id(-1), input2_id(-1), output_id(-1) {}
};

// Evaluate a single gate given its inputs
int evaluateGate(GateType type, int in1, int in2 = 0);

// Get typical propagation delay for a gate type
double getTypicalDelay(GateType type);

// Get gate type as string
std::string gateTypeToString(GateType type);

#endif // GATES_H