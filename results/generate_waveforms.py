"""
Multiplexer Timing Waveform Generator
Visualizes gate-level propagation delays for different MUX structures.
Generates a timing diagram showing input-to-output delay.
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

def draw_mux_waveform(mux_size=8, structure_name="Sum-of-Products", 
                       delay_ps=230, filename="mux_timing.png"):
    """
    Draws a digital timing waveform diagram for a multiplexer.
    
    Parameters:
        mux_size: Size of multiplexer (4, 8, or 16)
        structure_name: Name of the logic structure used
        delay_ps: Total propagation delay in picoseconds
        filename: Output image file name
    """
    
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 8), 
                                     gridspec_kw={'height_ratios': [2, 1]})
    fig.suptitle(f'{mux_size}:1 Multiplexer — {structure_name}\n'
                 f'Propagation Delay: {delay_ps} ps', 
                 fontsize=14, fontweight='bold')
    
    # ============================================================
    # TOP PLOT: Timing Diagram (Digital Waveforms)
    # ============================================================
    
    time_ns = np.linspace(0, 5, 1000)
    delay_ns = delay_ps / 1000.0  # Convert ps to ns for display
    
    # Select line transitions
    sel_transition_time = 1.0  # Select line changes at t=1ns
    input_ready_time = 0.5      # Input is stable at t=0.5ns
    output_transition_time = sel_transition_time + delay_ns  # Output responds after delay
    
    # Draw waveforms
    signals = [
        ('S[2:0]', sel_transition_time, 'blue'),
        ('I[7:0]', input_ready_time, 'green'),
        ('Y (Output)', output_transition_time, 'red'),
    ]
    
    y_positions = [3, 2, 1]
    
    for idx, (name, transition, color) in enumerate(signals):
        y = y_positions[idx]
        
        if 'S' in name:
            # Select lines: step from old value to new value
            t = np.array([0, 0.99, 1.0, 5.0])
            v = np.array([0, 0, 1, 1])  # Select changes from 000 to some value
            ax1.step(t, v + y * 0.1, where='post', color=color, linewidth=2)
        elif 'I' in name:
            # Inputs: stable throughout
            t = np.array([0, 5.0])
            v = np.array([1, 1])  # Active input is HIGH
            ax1.step(t, v + y * 0.1, where='post', color=color, linewidth=2)
        else:
            # Output: transitions after delay
            t = np.array([0, transition - 0.01, transition, 5.0])
            v = np.array([0, 0, 1, 1])
            ax1.step(t, v + y * 0.1, where='post', color=color, linewidth=2.5)
        
        ax1.text(-0.5, y, name, fontsize=11, fontweight='bold', va='center')
    
    # Highlight propagation delay
    ax1.annotate('', xy=(output_transition_time, 1.1), 
                 xytext=(sel_transition_time, 1.1),
                 arrowprops=dict(arrowstyle='<->', color='orange', lw=2))
    ax1.text((sel_transition_time + output_transition_time) / 2, 1.3, 
             f'{delay_ps} ps', fontsize=12, color='orange', 
             fontweight='bold', ha='center',
             bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.3))
    
    # Highlight input setup time
    ax1.annotate('', xy=(sel_transition_time, 2.1),
                 xytext=(input_ready_time, 2.1),
                 arrowprops=dict(arrowstyle='<->', color='purple', lw=1.5, ls='--'))
    ax1.text((input_ready_time + sel_transition_time) / 2, 2.3,
             'Setup\ntime', fontsize=9, color='purple', ha='center')
    
    ax1.set_ylim(0.5, 3.8)
    ax1.set_xlim(-1, 5.5)
    ax1.set_xlabel('Time (ns)', fontsize=11)
    ax1.set_ylabel('Signal', fontsize=11)
    ax1.grid(True, alpha=0.3, linestyle='--')
    ax1.set_title('Timing Diagram', fontsize=12)
    
    # Legend
    legend_elements = [
        mpatches.Patch(color='blue', label='Select Lines'),
        mpatches.Patch(color='green', label='Data Inputs'),
        mpatches.Patch(color='red', label='Output'),
    ]
    ax1.legend(handles=legend_elements, loc='upper right')
    
    # ============================================================
    # BOTTOM PLOT: Gate-Level Critical Path Visualization
    # ============================================================
    
    # Draw critical path as a horizontal bar chart showing cumulative delay
    gate_names = ['Select\nInput', 'NOT\nGate', 'AND\nGate', 'AND\nGate', 
                   'OR\nGate', 'Output\nLoad']
    gate_delays = [0, 20, 50, 50, 55, 55]  # Example delays in ps
    cumulative = np.cumsum(gate_delays)
    
    colors_gates = ['gray', 'lightblue', 'lightgreen', 'lightgreen', 
                    'salmon', 'orange']
    
    bars = ax2.barh(gate_names, gate_delays, color=colors_gates, 
                     edgecolor='black', linewidth=1)
    
    # Add delay labels on bars
    for bar, delay in zip(bars, gate_delays):
        if delay > 0:
            ax2.text(bar.get_width()/2, bar.get_y() + bar.get_height()/2,
                    f'{delay} ps', ha='center', va='center', fontsize=9)
    
    ax2.set_xlabel('Cumulative Delay (ps)', fontsize=11)
    ax2.set_title(f'Critical Path Breakdown (Total: {delay_ps} ps)', fontsize=12)
    ax2.grid(True, alpha=0.3, axis='x')
    
    # Add total delay marker
    ax2.axvline(x=delay_ps, color='red', linestyle='--', linewidth=1.5)
    ax2.text(delay_ps + 2, len(gate_names) - 1, f'Total: {delay_ps}ps', 
             color='red', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.show()
    print(f"✓ Waveform saved as '{filename}'")


def generate_comparison_chart():
    """
    Generates a bar chart comparing delay, area, and gate count
    across all three multiplexer structures.
    """
    
    structures = ['Sum-of-\nProducts', 'Tree of\n2:1 MUXes', 'NAND-\nBased']
    
    # Data for 8:1 multiplexer
    delays = [230, 210, 210]     # ps
    areas = [90, 72, 72]         # transistor count
    gates = [25, 21, 21]         # gate count
    
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    fig.suptitle('8:1 Multiplexer — Structure Comparison', 
                 fontsize=14, fontweight='bold')
    
    colors = ['#2196F3', '#4CAF50', '#FF9800']
    
    # Delay comparison
    axes[0].bar(structures, delays, color=colors, edgecolor='black')
    axes[0].set_ylabel('Delay (ps)', fontsize=11)
    axes[0].set_title('Propagation Delay\n(lower is better)', fontsize=11)
    for i, v in enumerate(delays):
        axes[0].text(i, v + 3, str(v), ha='center', fontweight='bold')
    
    # Area comparison
    axes[1].bar(structures, areas, color=colors, edgecolor='black')
    axes[1].set_ylabel('Transistor Count', fontsize=11)
    axes[1].set_title('Area Estimate\n(lower is better)', fontsize=11)
    for i, v in enumerate(areas):
        axes[1].text(i, v + 1, str(v), ha='center', fontweight='bold')
    
    # Gate count comparison
    axes[2].bar(structures, gates, color=colors, edgecolor='black')
    axes[2].set_ylabel('Gate Count', fontsize=11)
    axes[2].set_title('Gate Count\n(lower is better)', fontsize=11)
    for i, v in enumerate(gates):
        axes[2].text(i, v + 0.3, str(v), ha='center', fontweight='bold')
    
    for ax in axes:
        ax.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    plt.savefig('structure_comparison.png', dpi=150, bbox_inches='tight')
    plt.show()
    print("✓ Comparison chart saved as 'structure_comparison.png'")


def generate_delay_vs_size_plot():
    """
    Plots how delay scales with multiplexer size for each structure.
    """
    
    sizes = [2, 4, 8, 16, 32]
    
    # Simulated delay data (from logical effort estimation)
    sop_delays = [90, 150, 230, 310, 420]
    tree_delays = [90, 150, 210, 290, 380]
    nand_delays = [85, 145, 210, 290, 375]
    
    plt.figure(figsize=(10, 6))
    
    plt.plot(sizes, sop_delays, 'o-', color='#2196F3', linewidth=2.5, 
             markersize=10, label='Sum-of-Products')
    plt.plot(sizes, tree_delays, 's-', color='#4CAF50', linewidth=2.5,
             markersize=10, label='Tree of 2:1 MUXes')
    plt.plot(sizes, nand_delays, '^-', color='#FF9800', linewidth=2.5,
             markersize=10, label='NAND-Based')
    
    plt.xlabel('Multiplexer Size (N:1)', fontsize=12)
    plt.ylabel('Propagation Delay (ps)', fontsize=12)
    plt.title('Delay Scaling with Multiplexer Size', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    
    # Add annotations at crossover points
    plt.annotate('Tree structure\nbecomes better\nat N≥8', 
                 xy=(8, 210), xytext=(12, 180),
                 arrowprops=dict(arrowstyle='->', color='green'),
                 fontsize=10, color='green')
    
    # X-axis: show as powers of 2
    plt.xticks(sizes, [f'{s}:1' for s in sizes])
    plt.xscale('log', base=2)
    
    plt.tight_layout()
    plt.savefig('delay_scaling.png', dpi=150, bbox_inches='tight')
    plt.show()
    print("✓ Delay scaling plot saved as 'delay_scaling.png'")


if __name__ == "__main__":
    print("=" * 50)
    print("Multiplexer Waveform & Analysis Generator")
    print("=" * 50)
    
    # Generate individual waveform for 8:1 mux with Sum-of-Products
    draw_mux_waveform(mux_size=8, 
                      structure_name="Sum-of-Products", 
                      delay_ps=230,
                      filename="waveform_8to1_sop.png")
    
    # Generate comparison chart
    generate_comparison_chart()
    
    # Generate delay scaling plot
    generate_delay_vs_size_plot()
    
    print("\n✓ All visualizations generated successfully!")
    print("  - waveform_8to1_sop.png")
    print("  - structure_comparison.png")
    print("  - delay_scaling.png")
    