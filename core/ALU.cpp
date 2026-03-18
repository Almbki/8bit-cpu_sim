#include <iostream>
#include <cstdint>

enum {
    ADD = 0x01,
    SUB = 0x02,
    MUL = 0x03,
    NOT = 0x04,
    OR  = 0x05,
    AND = 0x06,
    XOR = 0x07,
};

struct Flags {
    bool ZF;    // Zero Flag
    bool CF;    // Carry Flag
    bool OF;    // Overflow Flag
    bool SF;    // Sign Flag

    uint8_t pack() {
        return (ZF << 0) | (CF << 1) | (OF << 2) | (SF << 3);
    }

    void unpack(uint8_t flags) {
        ZF = flags & 1;
        CF = (flags >> 1) & 1;
        OF = (flags >> 2) & 1;
        SF = (flags >> 3) & 1;
    }

    void update_logic(uint8_t F) {
        ZF = (F == 0);
        SF = (F >> 7) & 1;
    }

    void update_add(uint8_t A, uint8_t B, uint8_t F, uint16_t tmp) {
        CF = tmp > 0xFF;
        OF = ((A ^ F) & (B ^ F)) >> 7;
    }

    void update_sub(uint8_t A, uint8_t B, uint8_t F) {
        CF = (A < B);
        OF = ((A ^ B) & (A ^ F)) >> 7;
    }

    void update_mul(uint16_t tmp, uint8_t F) {
        CF = tmp > 0xFF;
        OF = tmp > 0xFF; 
    }
};

struct Alu_Result {
    uint8_t F;
    Flags flags;
    bool COUT;
};

Alu_Result Alu_calc(uint8_t ALU_OP, uint8_t DATA_A, uint8_t DATA_B, uint8_t CIN = 0) {
    Alu_Result r{};
    uint16_t tmp = 0;

    switch(ALU_OP) {
        case ADD:
            tmp = DATA_A + DATA_B + CIN;
            r.F = tmp & 0xFF;
            r.flags.update_logic(r.F);
            r.flags.update_add(DATA_A, DATA_B, r.F, tmp);
            r.COUT = r.flags.CF;
            break;

        case SUB:
            r.F = DATA_A - DATA_B - CIN;
            r.flags.update_logic(r.F);
            r.flags.update_sub(DATA_A, DATA_B, r.F);
            r.COUT = r.flags.CF;
            break;

        case MUL:
            tmp = DATA_A * DATA_B;
            r.F = tmp & 0xFF;
            r.flags.update_logic(r.F);
            r.flags.update_mul(tmp, r.F);
            r.COUT = r.flags.CF;
            break;

        case NOT:
            r.F = ~DATA_A;
            r.flags.update_logic(r.F);
            r.COUT = 0; r.flags.CF = 0; r.flags.OF = 0;
            break;

        case AND:
            r.F = DATA_A & DATA_B;
            r.flags.update_logic(r.F);
            r.COUT = 0; r.flags.CF = 0; r.flags.OF = 0;
            break;

        case OR:
            r.F = DATA_A | DATA_B;
            r.flags.update_logic(r.F);
            r.COUT = 0; r.flags.CF = 0; r.flags.OF = 0;
            break;

        case XOR:
            r.F = DATA_A ^ DATA_B;
            r.flags.update_logic(r.F);
            r.COUT = 0; r.flags.CF = 0; r.flags.OF = 0;
            break;

        default:
            r.F = 0;
            r.flags.ZF = 1; r.flags.SF = 0; r.flags.CF = 0; r.flags.OF = 0;
            r.COUT = 0;
            break;
    }

    return r;
}

int main() {
    unsigned int ta, tb;
    std::cout << "Input A B: ";
    std::cin >> ta >> tb;
    uint8_t a = ta & 0xFF;
    uint8_t b = tb & 0xFF;

    Alu_Result res = Alu_calc(ADD, a, b);

    std::cout << "F = " << +res.F 
              << ", ZF=" << res.flags.ZF
              << ", SF=" << res.flags.SF
              << ", CF=" << res.flags.CF
              << ", OF=" << res.flags.OF
              << std::endl;
    return 0;
}