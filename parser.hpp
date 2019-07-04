#include <deque>
#include <sstream>

namespace yujie6 {
    /*
     *  op_code
     */
    const int OP_REG = 0x33;   // 0110011
    const int OP_IMM = 0x13;   // 0010011
    const int OP_LUI = 0x37;   // 0110100
    const int OP_BRANCH = 0x63;// 1100011
    const int OP_STORE = 0x23; // 0100011
    const int OP_LOAD = 0x03;  // 0000011
    const int OP_SYSTEM = 0x73;// 1110011
    const int OP_AUIPC = 0x17; // 0010111
    const int OP_JAL = 0x6F;   // 1101111
    const int OP_JALR = 0x67;  // 1100111
    const int OP_IMM32 = 0x1B; // 0011011
    const int OP_32 = 0x3B;    // 0111011

    enum Reg {
        REG_ZERO = 0,
        REG_RA = 1,
        REG_SP = 2,
        REG_GP = 3,
        REG_TP = 4,
        REG_T0 = 5,
        REG_T1 = 6,
        REG_T2 = 7,
        REG_S0 = 8,
        REG_S1 = 9,
        REG_A0 = 10,
        REG_A1 = 11,
        REG_A2 = 12,
        REG_A3 = 13,
        REG_A4 = 14,
        REG_A5 = 15,
        REG_A6 = 16,
        REG_A7 = 17,
        REG_S2 = 18,
        REG_S3 = 19,
        REG_S4 = 20,
        REG_S5 = 21,
        REG_S6 = 22,
        REG_S7 = 23,
        REG_S8 = 24,
        REG_S9 = 25,
        REG_S10 = 26,
        REG_S11 = 27,
        REG_T3 = 28,
        REG_T4 = 29,
        REG_T5 = 30,
        REG_T6 = 31,
    };

    enum InstType {
        R_TYPE,
        I_TYPE,
        S_TYPE,
        SB_TYPE,
        U_TYPE,
        UJ_TYPE,
    };

    enum Inst {
        LUI = 0,
        AUIPC = 1,
        JAL = 2,
        JALR = 3,
        BEQ = 4,
        BNE = 5,
        BLT = 6,
        BGE = 7,
        BLTU = 8,
        BGEU = 9,
        LB = 10,
        LH = 11,
        LW = 12,
        LD = 13,
        LBU = 14,
        LHU = 15,
        SB = 16,
        SH = 17,
        SW = 18,
        SD = 19,
        ADDI = 20,
        SLTI = 21,
        SLTIU = 22,
        XORI = 23,
        ORI = 24,
        ANDI = 25,
        SLLI = 26,
        SRLI = 27,
        SRAI = 28,
        ADD = 29,
        SUB = 30,
        SLL = 31,
        SLT = 32,
        SLTU = 33,
        XOR = 34,
        SRL = 35,
        SRA = 36,
        OR = 37,
        AND = 38,
        ECALL = 39,
        ADDIW = 40,
        MUL = 41,
        MULH = 42,
        DIV = 43,
        REM = 44,
        LWU = 45,
        CSRRW = 46,
        CSRRS = 47,
        CSRRC = 48,
        CSRRWI = 49,
        CSRRSI = 50,
        CSRRCI = 51,
        UNKNOWN = -1,
    };


}

using namespace yujie6;

class parser {
    // 0x3FF -> 1111111111
    // 0xFF  ->   11111111
public:
    std::deque<uint32_t> token;


    parser() {

    }

    uint32_t getPageOffset(uint32_t addr) { return addr & 0xFFF; }


};