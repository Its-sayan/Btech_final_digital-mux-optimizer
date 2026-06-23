#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include "gates.h"
#include <vector>
#include <string>
#include <map>

/*
 * Gate-Level Multiplexer Model
 * 
 * Represents a multiplexer as a network of logic gates with
 * measurable properties: gate count, critical path depth,
 * and estimated dynamic power (via switching activity).
 */

enum class MuxStructure {
    SUM_OF_PRODUCTS,      // Flat AND-OR structure
    TREE_OF_2TO1,         // Hierarchical tree of 2:1 muxes
    NAND_BASED            // Optimized NAND-NAND structure
};

struct MuxMetrics {
    int num_gates;
    int critical_path_depth;      // Longest gate chain from input to output
    double critical_path_delay_ps; // Total propagation delay
    int total_switching_events;    // For dynamic power estimation
    double area_estimate;          // Normalized area (gate count weighted by complexity)
    
    // Quality metrics
    double delay_product() const { 
        return critical_path_delay_ps * area_estimate; 
    }
};

class Multiplexer {
private:
    int num_inputs;           // Number of data inputs (power of 2)
    int num_select_lines;     // log2(num_inputs)
    std::vector<Gate> gates;  // The gate netlist
    std::vector<std::string> input_names;
    std::vector<std::string> select_names;
    std::string output_name;
    MuxStructure structure_type;
    
    // Internal construction methods
    void buildSumOfProducts();
    void buildTreeOf2to1();
    void buildNANDBased();
    int build2to1Mux(int in1_id, int in2_id, int sel_id, int start_gate_idx);
    
public:
    Multiplexer(int n_inputs);
    
    // Build the multiplexer with specified structure
    void build(MuxStructure structure);
    
    // Simulate the multiplexer for given inputs
    int simulate(const std::vector<int>& inputs, 
                 const std::vector<int>& select_lines) const;
    
    // Calculate metrics for the current structure
    MuxMetrics calculateMetrics() const;
    
    // Get gate-level representation as string
    std::string getSchematic() const;
    
    // Accessors
    int getNumInputs() const { return num_inputs; }
    int getNumSelectLines() const { return num_select_lines; }
    MuxStructure getStructureType() const { return structure_type; }
    int getGateCount() const { return gates.size(); }
    
    // Static: get name of structure type
    static std::string structureToString(MuxStructure s);
};

#endif // MULTIPLEXER_H