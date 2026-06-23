#include "multiplexer.h"
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iostream>

Multiplexer::Multiplexer(int n_inputs) : num_inputs(n_inputs) {
    // Validate power of 2
    if (n_inputs < 2 || (n_inputs & (n_inputs - 1)) != 0) {
        throw std::invalid_argument("Number of inputs must be a power of 2");
    }
    num_select_lines = static_cast<int>(log2(n_inputs));
    
    // Name the inputs
    for (int i = 0; i < num_inputs; i++) {
        input_names.push_back("I" + std::to_string(i));
    }
    for (int i = 0; i < num_select_lines; i++) {
        select_names.push_back("S" + std::to_string(i));
    }
    output_name = "Y";
}

void Multiplexer::build(MuxStructure structure) {
    gates.clear();
    structure_type = structure;
    
    switch (structure) {
        case MuxStructure::SUM_OF_PRODUCTS:
            buildSumOfProducts();
            break;
        case MuxStructure::TREE_OF_2TO1:
            buildTreeOf2to1();
            break;
        case MuxStructure::NAND_BASED:
            buildNANDBased();
            break;
    }
}

void Multiplexer::buildSumOfProducts() {
    /*
     * Sum-of-Products Implementation
     * For N:1 MUX with M select lines:
     * - N AND gates (one per minterm) with M+1 inputs each (decomposed)
     * - 1 OR gate with N inputs (decomposed into tree)
     * - M NOT gates for select line complements
     * 
     * This is the canonical "textbook" implementation.
     */
    int gate_counter = 0;
    
    // NOT gates for select lines
    std::vector<int> sel_true_ids, sel_comp_ids;
    for (int i = 0; i < num_select_lines; i++) {
        Gate not_gate(GateType::NOT, "NOT_S" + std::to_string(i), 
                      getTypicalDelay(GateType::NOT));
        not_gate.input1_id = i;  // Select line i
        not_gate.output_id = gate_counter;
        gates.push_back(not_gate);
        sel_true_ids.push_back(i);
        sel_comp_ids.push_back(gate_counter);
        gate_counter++;
    }
    
    // AND gates for each minterm (decomposed into 2-input ANDs)
    std::vector<int> minterm_outputs;
    for (int i = 0; i < num_inputs; i++) {
        // Build the minterm using a tree of 2-input AND gates
        std::vector<int> term_inputs;
        term_inputs.push_back(num_select_lines + i);  // Data input Ii
        
        // Select lines based on binary representation of i
        for (int j = 0; j < num_select_lines; j++) {
            if (i & (1 << (num_select_lines - 1 - j))) {
                term_inputs.push_back(sel_true_ids[j]);
            } else {
                term_inputs.push_back(sel_comp_ids[j]);
            }
        }
        
        // Reduce using 2-input AND gates
        int current_output = term_inputs[0];
        for (size_t k = 1; k < term_inputs.size(); k++) {
            Gate and_gate(GateType::AND2, 
                         "AND_min" + std::to_string(i) + "_" + std::to_string(k),
                         getTypicalDelay(GateType::AND2));
            and_gate.input1_id = current_output;
            and_gate.input2_id = term_inputs[k];
            and_gate.output_id = gate_counter;
            gates.push_back(and_gate);
            current_output = gate_counter;
            gate_counter++;
        }
        minterm_outputs.push_back(current_output);
    }
    
    // OR reduction (tree of 2-input OR gates)
    std::vector<int> or_inputs = minterm_outputs;
    while (or_inputs.size() > 1) {
        std::vector<int> next_level;
        for (size_t i = 0; i < or_inputs.size(); i += 2) {
            if (i + 1 < or_inputs.size()) {
                Gate or_gate(GateType::OR2, 
                           "OR_level" + std::to_string(next_level.size()),
                           getTypicalDelay(GateType::OR2));
                or_gate.input1_id = or_inputs[i];
                or_gate.input2_id = or_inputs[i + 1];
                or_gate.output_id = gate_counter;
                gates.push_back(or_gate);
                next_level.push_back(gate_counter);
                gate_counter++;
            } else {
                next_level.push_back(or_inputs[i]);
            }
        }
        or_inputs = next_level;
    }
}

void Multiplexer::buildTreeOf2to1() {
    /*
     * Tree of 2:1 Multiplexers
     * Recursively builds larger muxes from 2:1 building blocks.
     * A 2:1 mux is implemented as: Y = (I0 & ~S) | (I1 & S)
     * Larger muxes cascade these hierarchically.
     * 
     * This structure has logarithmic depth but more total gates.
     */
    std::vector<int> input_ids;
    for (int i = 0; i < num_inputs; i++) {
        input_ids.push_back(num_select_lines + i);
    }
    
    // Build tree bottom-up
    std::vector<int> current_level = input_ids;
    int sel_level = 0;
    int gate_counter = num_select_lines + num_inputs;
    
    while (current_level.size() > 1) {
        std::vector<int> next_level;
        for (size_t i = 0; i < current_level.size(); i += 2) {
            if (i + 1 < current_level.size()) {
                int mux_out = build2to1Mux(current_level[i], current_level[i+1],
                                          sel_level, gate_counter);
                next_level.push_back(mux_out);
                gate_counter = gates.size() + num_select_lines + num_inputs;
            } else {
                next_level.push_back(current_level[i]);
            }
        }
        current_level = next_level;
        sel_level++;
    }
}

int Multiplexer::build2to1Mux(int in1_id, int in2_id, int sel_id, 
                               int start_gate_idx) {
    int gate_idx = start_gate_idx;
    
    // NOT gate for select
    Gate not_gate(GateType::NOT, "NOT_tree_sel", getTypicalDelay(GateType::NOT));
    not_gate.input1_id = sel_id;
    not_gate.output_id = gate_idx;
    gates.push_back(not_gate);
    int sel_comp = gate_idx;
    gate_idx++;
    
    // AND gate for I0 & ~S
    Gate and1(GateType::AND2, "AND_tree0", getTypicalDelay(GateType::AND2));
    and1.input1_id = in1_id;
    and1.input2_id = sel_comp;
    and1.output_id = gate_idx;
    gates.push_back(and1);
    int and1_out = gate_idx;
    gate_idx++;
    
    // AND gate for I1 & S
    Gate and2(GateType::AND2, "AND_tree1", getTypicalDelay(GateType::AND2));
    and2.input1_id = in2_id;
    and2.input2_id = sel_id;
    and2.output_id = gate_idx;
    gates.push_back(and2);
    int and2_out = gate_idx;
    gate_idx++;
    
    // OR gate to combine
    Gate or_gate(GateType::OR2, "OR_tree", getTypicalDelay(GateType::OR2));
    or_gate.input1_id = and1_out;
    or_gate.input2_id = and2_out;
    or_gate.output_id = gate_idx;
    gates.push_back(or_gate);
    
    return gate_idx;
}

void Multiplexer::buildNANDBased() {
    /*
     * NAND-NAND Implementation
     * Uses DeMorgan's theorem to convert AND-OR to NAND-NAND.
     * F = (AB + CD) = ((AB)'(CD)')'
     * 
     * NAND gates are typically faster in CMOS.
     */
    // For simplicity, implement as tree but with NAND gates
    std::vector<int> input_ids;
    for (int i = 0; i < num_inputs; i++) {
        input_ids.push_back(num_select_lines + i);
    }
    
    // Similar recursive structure using NAND gates
    buildTreeOf2to1();  // Placeholder — full NAND optimization would go here
}

int Multiplexer::simulate(const std::vector<int>& inputs,
                          const std::vector<int>& select_lines) const {
    if (inputs.size() != static_cast<size_t>(num_inputs)) {
        throw std::invalid_argument("Wrong number of inputs");
    }
    if (select_lines.size() != static_cast<size_t>(num_select_lines)) {
        throw std::invalid_argument("Wrong number of select lines");
    }
    
    // Combine all primary inputs
    std::vector<int> node_values(num_select_lines + num_inputs + gates.size(), 0);
    
    // Set select lines
    for (int i = 0; i < num_select_lines; i++) {
        node_values[i] = select_lines[i];
    }
    // Set data inputs
    for (int i = 0; i < num_inputs; i++) {
        node_values[num_select_lines + i] = inputs[i];
    }
    
    // Evaluate gates in order
    for (size_t i = 0; i < gates.size(); i++) {
        int in1 = node_values[gates[i].input1_id];
        int in2 = (gates[i].input2_id >= 0) ? node_values[gates[i].input2_id] : 0;
        node_values[gates[i].output_id] = evaluateGate(gates[i].type, in1, in2);
    }
    
    // Output is the last gate's output
    return node_values.back();
}

MuxMetrics Multiplexer::calculateMetrics() const {
    MuxMetrics metrics;
    metrics.num_gates = gates.size();
    
    // Calculate critical path using topological ordering
    std::vector<int> node_depth(num_select_lines + num_inputs + gates.size(), 0);
    std::vector<double> node_delay(num_select_lines + num_inputs + gates.size(), 0.0);
    
    for (size_t i = 0; i < gates.size(); i++) {
        int in1_depth = node_depth[gates[i].input1_id];
        int in2_depth = (gates[i].input2_id >= 0) ? 
                        node_depth[gates[i].input2_id] : 0;
        int max_input_depth = std::max(in1_depth, in2_depth);
        
        node_depth[gates[i].output_id] = max_input_depth + 1;
        
        double in1_delay = node_delay[gates[i].input1_id];
        double in2_delay = (gates[i].input2_id >= 0) ? 
                           node_delay[gates[i].input2_id] : 0.0;
        node_delay[gates[i].output_id] = 
            std::max(in1_delay, in2_delay) + gates[i].propagation_delay_ps;
    }
    
    metrics.critical_path_depth = node_depth.back();
    metrics.critical_path_delay_ps = node_delay.back();
    
    // Area estimate (normalized)
    double area = 0;
    for (const auto& gate : gates) {
        switch (gate.type) {
            case GateType::NOT:   area += 2;  break;  // 2 transistors
            case GateType::NAND2:
            case GateType::NOR2:  area += 4;  break;  // 4 transistors
            case GateType::AND2:
            case GateType::OR2:   area += 6;  break;  // 6 transistors
            case GateType::XOR2:  area += 12; break;  // 12 transistors
        }
    }
    metrics.area_estimate = area;
    
    // Estimate switching events for power
    metrics.total_switching_events = gates.size();  // Simplified
    
    return metrics;
}

std::string Multiplexer::getSchematic() const {
    std::stringstream ss;
    ss << "Multiplexer " << num_inputs << ":1 (" << structureToString(structure_type) 
       << ")\n";
    ss << "Inputs: " << num_inputs << ", Select lines: " << num_select_lines << "\n";
    ss << "Gate count: " << gates.size() << "\n\n";
    
    for (size_t i = 0; i < gates.size(); i++) {
        ss << "Gate[" << i << "]: " << gateTypeToString(gates[i].type) 
           << " \"" << gates[i].name << "\""
           << "  delay=" << gates[i].propagation_delay_ps << "ps\n";
    }
    
    return ss.str();
}

std::string Multiplexer::structureToString(MuxStructure s) {
    switch (s) {
        case MuxStructure::SUM_OF_PRODUCTS: return "Sum-of-Products";
        case MuxStructure::TREE_OF_2TO1:   return "Tree of 2:1 MUXes";
        case MuxStructure::NAND_BASED:     return "NAND-Based";
        default: return "Unknown";
    }
}