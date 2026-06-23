#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "multiplexer.h"
#include "optimizer.h"

/*
 * Digital Multiplexer Optimization Tool
 * 
 * A gate-level simulator and structural optimizer for digital multiplexers.
 * Explores different logic structures (Sum-of-Products, Tree of 2:1 MUXes,
 * NAND-based) and finds optimal implementations using algorithmic search.
 * 
 * Author: [Your Name]
 * Originally developed as BTech mini-project exploring digital logic 
 * optimization with algorithmic approaches.
 */

void printHeader() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   DIGITAL MULTIPLEXER STRUCTURAL OPTIMIZER                  ║\n";
    std::cout << "║   Gate-Level Simulation & Algorithmic Optimization          ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
}

void runOptimization(int mux_size, OptimizationGoal goal) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Optimizing " << mux_size << ":1 Multiplexer\n";
    std::cout << "Goal: " << MultiplexerOptimizer::goalToString(goal) << "\n";
    std::cout << std::string(60, '=') << "\n";
    
    MultiplexerOptimizer optimizer(mux_size, goal);
    
    // Run exhaustive search
    OptimizationResult result = optimizer.optimizeExhaustive();
    
    // Print comparison report
    std::cout << optimizer.generateComparisonReport();
    
    // Print optimization result
    std::cout << "★ OPTIMAL STRUCTURE: " 
              << Multiplexer::structureToString(result.best_structure) << "\n";
    std::cout << "  Algorithm: " << result.algorithm_used << "\n";
    std::cout << "  Time taken: " << std::fixed << std::setprecision(3) 
              << result.time_taken_ms << " ms\n";
    std::cout << "  Iterations: " << result.iterations_performed << "\n\n";
    
    // Show gate-level schematic of optimal design
    Multiplexer optimal_mux(mux_size);
    optimal_mux.build(result.best_structure);
    std::cout << "Gate-Level Structure of Optimal Design:\n";
    std::cout << optimal_mux.getSchematic() << "\n";
    
    // Verify functionality
    std::cout << "Functional Verification:\n";
    bool all_passed = true;
    
    int num_sel = static_cast<int>(log2(mux_size));
    
    for (int sel = 0; sel < mux_size; sel++) {
        std::vector<int> inputs(mux_size, 0);
        inputs[sel] = 1;  // Only selected input is HIGH
        
        std::vector<int> select_lines;
        for (int i = num_sel - 1; i >= 0; i--) {
            select_lines.push_back((sel >> i) & 1);
        }
        
        int output = optimal_mux.simulate(inputs, select_lines);
        
        if (output != 1) {
            std::cout << "  ✗ FAIL: Select=" << sel << ", Output=" << output 
                     << " (expected 1)\n";
            all_passed = false;
        }
    }
    
    if (all_passed) {
        std::cout << "  ✓ All " << mux_size << " test cases passed\n";
    }
}

int main() {
    printHeader();
    
    // Define optimization goals as an array (C++ compatible iteration)
    OptimizationGoal goals[] = {
        OptimizationGoal::MINIMIZE_DELAY,
        OptimizationGoal::MINIMIZE_AREA,
        OptimizationGoal::MINIMIZE_DELAY_AREA_PRODUCT
    };
    int num_goals = 3;
    
    // Define mux sizes to test
    int sizes[] = {4, 8, 16};
    int num_sizes = 3;
    
    // Run optimization for all combinations
    for (int s = 0; s < num_sizes; s++) {
        for (int g = 0; g < num_goals; g++) {
            runOptimization(sizes[s], goals[g]);
        }
    }
    
    // Save CSV data for visualization
    std::ofstream csv_file("optimization_results.csv");
    csv_file << "Size,Goal,Structure,Gates,Depth,Delay_ps,Area\n";
    
    for (int s = 0; s < num_sizes; s++) {
        for (int g = 0; g < num_goals; g++) {
            MultiplexerOptimizer opt(sizes[s], goals[g]);
            auto result = opt.optimizeExhaustive();
            for (const auto& [struct_type, metrics] : result.all_candidates) {
                csv_file << sizes[s] << ","
                        << MultiplexerOptimizer::goalToString(goals[g]) << ","
                        << Multiplexer::structureToString(struct_type) << ","
                        << metrics.num_gates << ","
                        << metrics.critical_path_depth << ","
                        << metrics.critical_path_delay_ps << ","
                        << metrics.area_estimate << "\n";
            }
        }
    }
    csv_file.close();
    std::cout << "\nResults saved to optimization_results.csv\n";
    
    // Print final summary table
    std::cout << "\n" << std::string(60, '═') << "\n";
    std::cout << "FINAL SUMMARY: Optimal Structures\n";
    std::cout << std::string(60, '═') << "\n\n";
    std::cout << std::left << std::setw(10) << "Mux Size"
              << std::setw(30) << "Goal"
              << std::setw(25) << "Best Structure"
              << std::right << std::setw(10) << "Gates"
              << std::setw(15) << "Delay (ps)" << "\n";
    std::cout << std::string(90, '-') << "\n";
    
    for (int s = 0; s < num_sizes; s++) {
        for (int g = 0; g < num_goals; g++) {
            MultiplexerOptimizer opt(sizes[s], goals[g]);
            auto result = opt.optimizeExhaustive();
            std::cout << std::left << std::setw(10) 
                      << (std::to_string(sizes[s]) + ":1")
                      << std::setw(30) << MultiplexerOptimizer::goalToString(goals[g])
                      << std::setw(25) << Multiplexer::structureToString(result.best_structure)
                      << std::right << std::setw(10) << result.best_metrics.num_gates
                      << std::setw(15) << std::fixed << std::setprecision(1) 
                      << result.best_metrics.critical_path_delay_ps << "\n";
        }
        std::cout << "\n";
    }
    
    std::cout << "✓ Optimization complete.\n\n";
    
    return 0;
}