#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "multiplexer.h"
#include <vector>
#include <string>
#include <functional>

/*
 * Structural Optimization Engine
 * 
 * Implements algorithms to find optimal multiplexer structure
 * given optimization criteria (delay, area, or product).
 * 
 * Algorithms implemented:
 * 1. Exhaustive search (for small muxes)
 * 2. Heuristic-guided selection
 * 3. Greedy local optimization
 */

struct OptimizationResult {
    MuxStructure best_structure;
    MuxMetrics best_metrics;
    double optimization_score;
    std::string algorithm_used;
    int iterations_performed;
    double time_taken_ms;
    
    // Comparison data
    std::vector<std::pair<MuxStructure, MuxMetrics>> all_candidates;
};

enum class OptimizationGoal {
    MINIMIZE_DELAY,
    MINIMIZE_AREA,
    MINIMIZE_DELAY_AREA_PRODUCT
};

class MultiplexerOptimizer {
private:
    int num_inputs;
    OptimizationGoal goal;
    std::vector<MuxStructure> available_structures;
    
    double scoreMetrics(const MuxMetrics& metrics) const;
    
public:
    MultiplexerOptimizer(int n_inputs, OptimizationGoal g);
    
    // Algorithm 1: Exhaustive evaluation of all structures
    OptimizationResult optimizeExhaustive();
    
    // Algorithm 2: Heuristic based on input size
    OptimizationResult optimizeHeuristic();
    
    // Algorithm 3: Iterative refinement (simulated annealing inspired)
    OptimizationResult optimizeIterative(int max_iterations = 50);
    
    // Run all algorithms and return best
    OptimizationResult optimizeAll();
    
    // Pretty-print comparison
    std::string generateComparisonReport() const;
    
    // Generate optimization data for graphing
    std::string generateCSVData() const;
    
    static std::string goalToString(OptimizationGoal g);
};

#endif // OPTIMIZER_H