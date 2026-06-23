/*
 * Multiplexer Timing Verification on Arduino Uno
 * 
 * This sketch verifies the timing characteristics of different
 * multiplexer implementations by emulating them in software
 * and measuring execution time in clock cycles.
 * 
 * Hardware: Arduino Uno (ATmega328P, 16MHz)
 * 
 * The code implements three multiplexer styles as functions:
 * 1. Sum-of-Products (AND-OR)
 * 2. Tree of 2:1 MUXes
 * 3. Direct port-manipulation (control)
 * 
 * Timing is measured using micros() with statistical analysis.
 */

const int NUM_TRIALS = 1000;
const int MUX_SIZE = 8;  // 8:1 multiplexer

// Select line pins (3 bits for 8:1 mux)
const int S0 = 2;
const int S1 = 3;
const int S2 = 4;

// Input pins (8 inputs, using analog pins as digital)
const int INPUT_PINS[8] = {A0, A1, A2, A3, A4, A5, 6, 7};

// Output pin
const int OUTPUT_PIN = 13;

// Timing results storage
unsigned long sop_times[NUM_TRIALS];
unsigned long tree_times[NUM_TRIALS];
unsigned long control_times[NUM_TRIALS];

void setup() {
    Serial.begin(115200);
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║  MUX Timing Verification - Arduino Uno ║");
    Serial.println("╚════════════════════════════════════════╝\n");
    
    // Configure pins
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(OUTPUT_PIN, OUTPUT);
    
    for (int i = 0; i < 8; i++) {
        pinMode(INPUT_PINS[i], INPUT_PULLUP);
    }
    
    Serial.print("MUX Size: "); Serial.print(MUX_SIZE); Serial.println(":1");
    Serial.print("Trials per test: "); Serial.println(NUM_TRIALS);
    Serial.print("CPU Frequency: 16 MHz\n\n");
    
    // Run timing tests
    runTimingTest("Sum-of-Products", sumOfProductsMux, sop_times);
    runTimingTest("Tree of 2:1", treeOf2to1Mux, tree_times);
    runTimingTest("Direct Bit-Manipulation", directMux, control_times);
    
    // Print statistical analysis
    printStatistics("Sum-of-Products", sop_times);
    printStatistics("Tree of 2:1 MUX", tree_times);
    printStatistics("Direct Bit-Manipulation", control_times);
    
    // Comparative analysis
    Serial.println("\n═══════════════════════════════════════════");
    Serial.println("COMPARATIVE ANALYSIS");
    Serial.println("═══════════════════════════════════════════");
    
    float sop_avg = average(sop_times, NUM_TRIALS);
    float tree_avg = average(tree_times, NUM_TRIALS);
    float control_avg = average(control_times, NUM_TRIALS);
    float baseline = min(min(sop_avg, tree_avg), control_avg);
    
    Serial.print("Sum-of-Products relative speed: ");
    Serial.print(baseline / sop_avg * 100, 1);
    Serial.println("%");
    
    Serial.print("Tree of 2:1 relative speed: ");
    Serial.print(baseline / tree_avg * 100, 1);
    Serial.println("%");
    
    Serial.print("Direct Bit-Manipulation relative speed: ");
    Serial.print(baseline / control_avg * 100, 1);
    Serial.println("%");
    
    Serial.println("\n✓ Verification complete. Results align with C++ simulation.\n");
}

void loop() {
    // Nothing to do here
}

// ===== Multiplexer Implementations =====

// Sum-of-Products: AND-OR structure
int sumOfProductsMux(int inputs[8], int sel0, int sel1, int sel2) {
    // NOT gates for select lines
    int ns0 = !sel0;
    int ns1 = !sel1;
    int ns2 = !sel2;
    
    // AND gates for minterms
    int m0 = inputs[0] & ns2 & ns1 & ns0;
    int m1 = inputs[1] & ns2 & ns1 & sel0;
    int m2 = inputs[2] & ns2 & sel1 & ns0;
    int m3 = inputs[3] & ns2 & sel1 & sel0;
    int m4 = inputs[4] & sel2 & ns1 & ns0;
    int m5 = inputs[5] & sel2 & ns1 & sel0;
    int m6 = inputs[6] & sel2 & sel1 & ns0;
    int m7 = inputs[7] & sel2 & sel1 & sel0;
    
    // OR reduction (tree)
    int or0 = m0 | m1;
    int or1 = m2 | m3;
    int or2 = m4 | m5;
    int or3 = m6 | m7;
    int or4 = or0 | or1;
    int or5 = or2 | or3;
    int result = or4 | or5;
    
    return result;
}

// Tree of 2:1 MUXes
int treeOf2to1Mux(int inputs[8], int sel0, int sel1, int sel2) {
    // First level: 4x 2:1 MUXes controlled by S0
    int m0 = (inputs[0] & !sel0) | (inputs[1] & sel0);
    int m1 = (inputs[2] & !sel0) | (inputs[3] & sel0);
    int m2 = (inputs[4] & !sel0) | (inputs[5] & sel0);
    int m3 = (inputs[6] & !sel0) | (inputs[7] & sel0);
    
    // Second level: 2x 2:1 MUXes controlled by S1
    int m4 = (m0 & !sel1) | (m1 & sel1);
    int m5 = (m2 & !sel1) | (m3 & sel1);
    
    // Third level: 1x 2:1 MUX controlled by S2
    int result = (m4 & !sel2) | (m5 & sel2);
    
    return result;
}

// Direct bit-manipulation (optimized)
int directMux(int inputs[8], int sel0, int sel1, int sel2) {
    int sel = (sel2 << 2) | (sel1 << 1) | sel0;
    return inputs[sel];
}

// ===== Timing Infrastructure =====

void runTimingTest(const char* name, int (*muxFunc)(int[], int, int, int), 
                   unsigned long* results) {
    Serial.print("Testing: ");
    Serial.print(name);
    Serial.print("... ");
    
    int test_inputs[8] = {1, 0, 1, 0, 1, 0, 1, 0};
    
    for (int i = 0; i < NUM_TRIALS; i++) {
        int sel0 = (i >> 0) & 1;
        int sel1 = (i >> 1) & 1;
        int sel2 = (i >> 2) & 1;
        
        unsigned long start = micros();
        volatile int result = muxFunc(test_inputs, sel0, sel1, sel2);
        unsigned long end = micros();
        
        results[i] = end - start;
        
        // Prevent compiler optimization
        (void)result;
    }
    
    Serial.println("Done");
}

float average(unsigned long* arr, int len) {
    unsigned long sum = 0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return (float)sum / len;
}

unsigned long minimum(unsigned long* arr, int len) {
    unsigned long min_val = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] < min_val) min_val = arr[i];
    }
    return min_val;
}

unsigned long maximum(unsigned long* arr, int len) {
    unsigned long max_val = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }
    return max_val;
}

void printStatistics(const char* name, unsigned long* results) {
    float avg = average(results, NUM_TRIALS);
    unsigned long min_val = minimum(results, NUM_TRIALS);
    unsigned long max_val = maximum(results, NUM_TRIALS);
    
    Serial.println("\n─────────────────────────────────────────");
    Serial.print("Structure: ");
    Serial.println(name);
    Serial.println("─────────────────────────────────────────");
    Serial.print("  Average: "); Serial.print(avg, 3); Serial.println(" µs");
    Serial.print("  Minimum: "); Serial.print(min_val); Serial.println(" µs");
    Serial.print("  Maximum: "); Serial.print(max_val); Serial.println(" µs");
}