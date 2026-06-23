# Theory: Multiplexer Design & Optimization

## What is a Multiplexer?

A multiplexer (MUX) is a combinational digital circuit that selects
one of N input signals and forwards it to a single output. Selection
is controlled by log₂(N) select lines.

## Structural Implementations

### 1. Sum-of-Products (SOP)
- **Description**: Each minterm is generated using AND gates, then
  OR-ed together
- **Gates**: N × (M+1)-input AND gates + 1 N-input OR gate + M NOT gates
- **Depth**: O(M + log₂(N)) after decomposition
- **Best for**: Clarity and direct mapping from truth table

### 2. Tree of 2:1 MUXes
- **Description**: Hierarchical cascade of 2:1 multiplexers
- **Gates**: (N-1) × 3 gates per 2:1 MUX + inverter
- **Depth**: O(log₂(N))
- **Best for**: Large multiplexers, VLSI layout regularity

### 3. NAND-Based Implementation
- **Description**: Uses DeMorgan's theorem for NAND-NAND structure
- **Gates**: Similar to SOP but uses NAND gates (faster in CMOS)
- **Depth**: O(M + log₂(N))
- **Best for**: CMOS implementation where NAND gates are optimal

## Optimization Metrics

### Propagation Delay
Time for a signal change at input to propagate to output. Determined
by the longest path (critical path) through the gate network.

### Area
Silicon area required, estimated by transistor count. Each gate type
requires different transistor counts:
- NOT: 2 transistors
- NAND/NOR: 4 transistors
- AND/OR: 6 transistors

### Power-Delay Product (PDP)
Product of power consumption and delay. Lower is better.
Dynamic power ∝ (switching activity × capacitance × V² × frequency).

## Algorithmic Optimization

For a given multiplexer size and optimization goal, we can use:

1. **Exhaustive Search**: Evaluate all structures (feasible for small space)
2. **Heuristic Selection**: Apply design rules based on size
3. **Simulated Annealing**: Probabilistic optimization for larger spaces

## References

- Weste & Harris, "CMOS VLSI Design"
- Rabaey, "Digital Integrated Circuits"
- Sutherland, "Logical Effort"