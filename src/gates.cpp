#include "gates.h"
#include <stdexcept>

int evaluateGate(GateType type, int in1, int in2) {
    switch (type) {
        case GateType::AND2:  return in1 & in2;
        case GateType::OR2:   return in1 | in2;
        case GateType::NOT:   return !in1;
        case GateType::NAND2: return !(in1 & in2);
        case GateType::NOR2:  return !(in1 | in2);
        case GateType::XOR2:  return in1 ^ in2;
        default:
            throw std::runtime_error("Unknown gate type");
    }
}

double getTypicalDelay(GateType type) {
    // Approximate delays in picoseconds for educational modeling
    switch (type) {
        case GateType::NOT:   return 20.0;
        case GateType::NAND2: return 30.0;
        case GateType::NOR2:  return 35.0;
        case GateType::AND2:  return 50.0;  // NAND + NOT
        case GateType::OR2:   return 55.0;  // NOR + NOT
        case GateType::XOR2:  return 80.0;
        default: return 50.0;
    }
}

std::string gateTypeToString(GateType type) {
    switch (type) {
        case GateType::AND2:  return "AND2";
        case GateType::OR2:   return "OR2";
        case GateType::NOT:   return "NOT";
        case GateType::NAND2: return "NAND2";
        case GateType::NOR2:  return "NOR2";
        case GateType::XOR2:  return "XOR2";
        default: return "UNKNOWN";
    }
}