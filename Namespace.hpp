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


    struct FReg {
        // Control Signals
        bool bubble;
        uint32_t stall;

        uint64_t pc;
        uint32_t inst;
    };
    struct DReg {
        // Control Signals
        bool bubble;
        uint32_t rs1, rs2;
        uint64_t pc;
        Inst inst;
        uint32_t srcInst;
    };
    struct EReg {
        // Control Signals
        bool bubble;
        bool writeback;
        uint32_t stall;
        uint64_t pc;
        Inst inst;
        int64_t op1;
        int64_t op2;
        uint64_t Imm;
        uint32_t dest;
        int64_t out;
    };
    struct MReg {
        MReg(){
            bubble = mem = writeback = false;
        }
        bool bubble;
        bool mem;
        bool writeback;
        uint64_t pc;
        Inst inst;
        int len;
        int64_t out;
        uint32_t Memdest;
        uint32_t RegTowrite;
    };

    struct WReg {
        bool bubble;
        uint64_t pc;
        Inst inst;
        int64_t out;
        bool writeback;
        uint32_t RegTowrite;
    };



}

using namespace yujie6;

