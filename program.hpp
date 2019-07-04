//
// Created by yujie6 on 19-7-2.
//

#include <fstream>
#include <cstring>
#include <cstdio>
#include "parser.hpp"
#include "memory.hpp"

namespace yujie6 {
    struct PipeRegister {
        uint32_t Imm1;
        int64_t op1, op2;
        uint32_t dest;
    };

    struct WLRegister {
        bool writeback, mem;
        int64_t out;
        uint32_t Memdest;
        uint32_t RegTowrite;
    };


    inline bool isBranchOrStore(Inst inst) {
        return inst == BEQ || inst == BNE || inst == BLT || inst == BGE ||
               inst == BLTU || inst == BGEU || inst == SW || inst == SH || inst == SB;
    }

    inline bool isJump(Inst inst) {
        return inst == JAL || inst == JALR || inst == BEQ || inst == BNE ||
               inst == BLT || inst == BGE || inst == BLTU || inst == BGEU;
    }

    inline bool isReadMem(Inst inst) {
        return inst == LB || inst == LH || inst == LW  || inst == LBU ||
               inst == LHU || inst == LWU || inst == SB || inst == SW || inst == SH;
    }
}


using namespace yujie6;
using std::cin;
using std::cout;

class program {
public:
    parser *par;
    MemoryController *MemControl;
    int ProgramCounter;
    Inst CurrentInst;
    PipeRegister FReg;
    WLRegister Wreg;
    uint32_t inst;
    int Reg[32];

    program() {
        par = new parser();
        memset(Reg, 0, sizeof(Reg));
        MemControl = new MemoryController();
        ProgramCounter = 0;
    }

    inline void Run() {
        while (true) {
            auto ans = (((unsigned int) Reg[10]) & 255u);
            Fetch();
            if (inst == 13009443) {
                cout << ans << "\n";
                break;
            }
            Decode();
            Execute();
            MemoryAccess();
            WriteBack();
            //cout << ans << "\n";
            if (Reg[0] != 0) Reg[0] = 0;
        }
    }

    void Fetch() {
        inst = MemControl->GetInstruction(ProgramCounter);
    }

    void Decode() {
        uint32_t opcode = inst & 0x7F;
        uint32_t funct3 = (inst >> 12) & 0x7;
        uint32_t funct7 = (inst >> 25) & 0x7F;
        uint32_t rd = (inst >> 7) & 0x1F;
        uint32_t rs1 = (inst >> 15) & 0x1F;
        uint32_t rs2 = (inst >> 20) & 0x1F;
        int32_t imm_i = int32_t(inst) >> 20;
        //imm[11:0]
        int32_t imm_s = int32_t(((inst >> 7) & 0x1F) | ((inst >> 20) & 0xFE0)) << 20 >> 20;
        //imm[]
        int32_t imm_sb = int32_t(((inst >> 7) & 0x1E) | ((inst >> 20) & 0x7E0) |
                                 ((inst << 4) & 0x800) | ((inst >> 19) & 0x1000))
                << 19 >>
                19;
        int32_t imm_u = int32_t(inst) >> 12;
        int32_t imm_uj = int32_t(((inst >> 21) & 0x3FF) | ((inst >> 10) & 0x400) |
                                 ((inst >> 1) & 0x7F800) | ((inst >> 12) & 0x80000))
                << 12 >>
                11;
        int32_t csr = imm_i;
        uint32_t reg1, reg2;
        FReg.dest = rd;
        switch (opcode) {
            case OP_LUI: {
                FReg.Imm1 = imm_u;
                CurrentInst = LUI;
                break;
            }
            case OP_AUIPC: {
                FReg.Imm1 = imm_u;
                CurrentInst = AUIPC;
                break;
            }

            case OP_REG: {
                FReg.op1 = Reg[rs1];
                FReg.op2 = Reg[rs2];
                FReg.Imm1 = 0; // R-type Get No imm
                switch (funct3) {
                    case 0x0: {
                        switch (funct7) {
                            case 0x0: {
                                CurrentInst = ADD;
                                break;
                            }
                            case 0x01: {
                                CurrentInst = MUL;
                                break;
                            }
                            case 0x20: {
                                CurrentInst = SUB;
                                break;
                            }
                            default:
                                cout << "unknown";
                        }
                        break;
                    }

                    case 0x1: {
                        if (funct7 == 0x00) {
                            CurrentInst = SLL;
                        } else if (funct7 == 0x01) {
                            CurrentInst = MULH;
                        } else {
                            cout << "unkonwn";
                        }
                        break;
                    }
                    case 0x2: {
                        if (funct7 == 0x00) {
                            CurrentInst = SLT;
                        } else cout << "unknown";
                        break;
                    }
                    case 0x3: {
                        CurrentInst = SLTU;
                        break;
                    }
                    case 0x4: {
                        if (funct7 == 0x00) {
                            CurrentInst = XOR;
                        } else if (funct7 == 0x01) {
                            CurrentInst = DIV;
                        }
                        break;
                    }
                    case 0x5: {
                        switch (funct7) {
                            case 0x0: {
                                CurrentInst = SRL;
                                break;
                            }
                            case 0x20: {
                                CurrentInst = SRA;
                                break;
                            }
                            default: {
                            }
                        }
                        break;
                    }
                    case 0x6: {
                        if (funct7 == 0x00) {
                            CurrentInst = OR;
                        } else if (funct7 == 0x01) {
                            CurrentInst = REM;
                        }
                        break;
                    }
                    case 0x7: {
                        CurrentInst = AND;
                        break;
                    }
                    default:
                        cout << "error!";
                }
                break;
            }
            case OP_IMM: { // No rs2
                FReg.op1 = Reg[rs1];
                FReg.Imm1 = imm_i;
                switch (funct3) {
                    case 0x0:
                        CurrentInst = ADDI;
                        break;
                    case 0x2:
                        CurrentInst = SLTI;  //set less than imm
                        break;
                    case 0x3:
                        CurrentInst = SLTIU;
                        break;
                    case 0x4:
                        CurrentInst = XORI;
                        break;
                    case 0x6:
                        CurrentInst = ORI;
                        break;
                    case 0x7:
                        CurrentInst = ANDI;
                        break;
                    case 0x1:
                        CurrentInst = SLLI;
                        FReg.Imm1 = FReg.Imm1 & 0x3F;
                        break;
                    case 0x5:
                        if (((inst >> 26) & 0x3F) == 0x0) {
                            CurrentInst = SRLI;
                            FReg.Imm1 = FReg.Imm1 & 0x3F;
                        } else if (((inst >> 26) & 0x3F) == 0x10) {
                            CurrentInst = SRAI;
                            FReg.Imm1 = FReg.Imm1 & 0x3F;
                        }
                        break;
                }
                break;
            }
            case OP_LOAD: {
                FReg.op1 = Reg[rs1];
                FReg.Imm1 = imm_i;
                switch (funct3) {
                    case 0x0: {
                        CurrentInst = LB;
                        break;
                    }
                    case 0x1: {
                        CurrentInst = LH;
                        break;
                    }
                    case 0x2: {
                        CurrentInst = LW;
                        break;
                    }
                    case 0x4: {
                        CurrentInst = LBU;
                        break;
                    }
                    case 0x5: {
                        CurrentInst = LHU;
                        break;
                    }
                    default: {
                    }
                }
                break;
            }
            case OP_STORE: {
                FReg.op1 = Reg[rs1];
                FReg.op2 = Reg[rs2];
                FReg.Imm1 = imm_s;
                switch (funct3) {
                    case 0x0: {
                        CurrentInst = SB;
                        break;
                    }
                    case 0x1: {
                        CurrentInst = SH;
                        break;
                    }
                    case 0x2: {
                        CurrentInst = SW;
                        break;
                    }
                    default: {
                    }
                }
                break;
            }
            case OP_BRANCH: {
                FReg.op1 = Reg[rs1];
                FReg.op2 = Reg[rs2];
                FReg.Imm1 = imm_sb;
                switch (funct3) {
                    case 0x0: {
                        CurrentInst = BEQ;
                        break;
                    }
                    case 0x1: {
                        CurrentInst = BNE;
                        break;
                    }
                    case 0x4: {
                        CurrentInst = BLT;
                        break;
                    }
                    case 0x5: {
                        CurrentInst = BGE;
                        break;
                    }
                    case 0x6: {
                        CurrentInst = BLTU;
                        break;
                    }
                    case 0x7: {
                        CurrentInst = BGEU;
                        break;
                    }
                    default: {
                    }
                }
                break;
            }
            case OP_JAL: {
                CurrentInst = JAL;
                FReg.Imm1 = imm_uj;
                break;
            }
            case OP_JALR: {
                CurrentInst = JALR;
                FReg.op1 = Reg[rs1];
                FReg.Imm1 = imm_i;
                break;
            }
            case OP_SYSTEM: {
                FReg.op1 = Reg[rs1];
                FReg.Imm1 = csr;
                switch (funct3) {
                    case 0x1: {
                        CurrentInst = CSRRW;
                        break;
                    }
                    case 0x2: {
                        CurrentInst = CSRRS;
                        break;
                    }
                    case 0x3: {
                        CurrentInst = CSRRC;
                        break;
                    }
                    case 0x5: {
                        CurrentInst = CSRRWI;
                        break;
                    }
                    case 0x6: {
                        CurrentInst = CSRRSI;
                        break;
                    }
                    case 0x7: {
                        CurrentInst = CSRRCI;
                        break;
                    }
                    default: {
                    }
                }
                break;
            }
            default: {
            }
        }
    }

    void Execute() {
        Wreg.RegTowrite = FReg.dest;
        switch (CurrentInst) {
            case ADD: {
                Wreg.out = FReg.op1 + FReg.op2;
                break;
            }
            case ADDI: {
                Wreg.out = FReg.op1 + FReg.Imm1;
                /*
                 *   TODO if Wreg.out is larger than 1<<32,
                 *    then forsake the high unit
                 */
                break;
            }
            case SUB: {
                Wreg.out = FReg.op1 - FReg.op2;
                break;
            }
            case LUI: {
                Wreg.out = FReg.Imm1 << 12;
                Wreg.writeback = true;
                break;
            }
            case AUIPC: {
                ProgramCounter += FReg.Imm1 << 12;
                Wreg.out = ProgramCounter + 4;
                break;
            }
            case JAL: {
                Wreg.out = ProgramCounter + 4;
                ProgramCounter += FReg.Imm1; //already * 2
                break;
            }
            case JALR: {
                Wreg.out = ProgramCounter + 4;
                ProgramCounter = (FReg.op1 + FReg.Imm1) & (~(uint64_t) 1);
                break;
            }
            case SLL: {
                Wreg.out = FReg.op1 << FReg.op2;
                break;
            }
            case SLLI: {
                Wreg.out = (FReg.op1 << FReg.Imm1);
                /*
                 *  TODO check shamt
                 */
                break;
            }
            case SLTU: {
                Wreg.out = ((uint64_t) FReg.op1 < (uint64_t) FReg.op2) ? 1 : 0;
                break;
            }
            case SLT: {
                Wreg.out = (FReg.op1 < FReg.op2) ? 1 : 0;
                break;
            }
            case SLTIU: {
                Wreg.out = ((uint64_t) FReg.op1 < (uint64_t) FReg.Imm1) ? 1 : 0;
            }
            case SLTI: {
                Wreg.out = (FReg.op1 < FReg.Imm1) ? 1 : 0;
            }
            case SRL: {
                Wreg.out = (uint64_t) FReg.op1 >> (uint64_t) FReg.op2;
                break;
            }
            case SRLI: {
                Wreg.out = (uint64_t) FReg.op1 >> (uint64_t) FReg.Imm1; //符号拓展？？
                break;
            }
            case SRA: {
                Wreg.out = FReg.op1 >> FReg.op2;
                // TODO shift right arith
                break;
            }
            case SRAI: {
                Wreg.out = FReg.op1 >> FReg.Imm1;
                break;
            }
            case XOR: {
                Wreg.out = FReg.op1 ^ FReg.op2;
                break;
            }
            case XORI: {
                Wreg.out = FReg.op1 ^ FReg.Imm1;
                break;
            }
            case OR: {
                Wreg.out = FReg.op1 | FReg.op2;
                break;
            }
            case ORI: {
                Wreg.out = FReg.op1 | FReg.Imm1;
                break;
            }
            case ANDI: {
                Wreg.out = FReg.op1 & FReg.Imm1;
                break;
            }
            case AND: {
                Wreg.out = FReg.op1 & FReg.op2;
                break;
            }
            case BEQ: {
                if (FReg.op1 == FReg.op2) {
                    ProgramCounter += FReg.Imm1;
                } else {
                    ProgramCounter += 4;
                }
                break;
            }
            case BNE: {
                if (FReg.op1 != FReg.op2) {
                    ProgramCounter += FReg.Imm1;
                } else {
                    ProgramCounter += 4;
                }
                break;
            }
            case BLTU: {
                if ((uint64_t) FReg.op1 < (uint64_t) FReg.op2) {
                    ProgramCounter += FReg.Imm1;
                } else {
                    ProgramCounter += 4;
                }
                break;
            }
            case BLT: { //有符号数？？
                if (FReg.op1 < FReg.op2) {
                    ProgramCounter += FReg.Imm1;
                } else {
                    ProgramCounter += 4;
                }
                break;
            }
            case BGEU: {
                if ((uint64_t) FReg.op1 >= (uint64_t) FReg.op2) {
                    ProgramCounter += FReg.Imm1;
                } else {
                    ProgramCounter += 4;
                }
                break;
            }
            case BGE: {
                if (FReg.op1 >= FReg.op2) {
                    ProgramCounter += FReg.Imm1;
                } else {
                    ProgramCounter += 4;
                }
                break;
            }
            case SH: {
                Wreg.Memdest = FReg.op1 + FReg.Imm1;
                Wreg.out = FReg.op2 & 0xFFFF;
                break;
            }
            case SW: {
                Wreg.Memdest = FReg.op1 + FReg.Imm1;
                Wreg.out = FReg.op2 & 0xFFFFFFFF;
                break;
            }
            case SB: {
                Wreg.Memdest = FReg.op1 + FReg.Imm1;
                Wreg.out = FReg.op2 & 0xFF;
                break;
            }
            case LHU:
            case LBU:
            case LWU:
            case LB:
            case LH:
            case LW: {
                Wreg.Memdest = FReg.op1 + FReg.Imm1;
                break;
            }
            default: {
            }
        }
        // Prepare for the next task
        Wreg.writeback = !isBranchOrStore(CurrentInst);
        if (!isJump(CurrentInst)) {
            ProgramCounter += 4; // not jump, go forward
        }
        Wreg.mem = isReadMem(CurrentInst);
    }

    void MemoryAccess() {    //写到内存
        if (!Wreg.mem) return;
        switch (CurrentInst) {
            case SB: {
                MemControl->StoreByte(Wreg.out, Wreg.Memdest);
                break;
            }
            case SH: {
                MemControl->StoreHalfWord(Wreg.out, Wreg.Memdest);
                break;
            }
            case SW: {
                MemControl->StoreWord(Wreg.out, Wreg.Memdest);
                break;
            }
            case LBU: {
                Wreg.out = (uint64_t) (MemControl->GetInstruction(Wreg.Memdest) & 0xFF);
                break;
            }
            case LHU: {
                Wreg.out = (uint64_t) (MemControl->GetInstruction(Wreg.Memdest) & 0xFFFF);
                break;
            }
            case LWU: {
                Wreg.out = (uint64_t) (MemControl->GetInstruction(Wreg.Memdest) & 0xFFFFFFFF);
                break;
            }
            case LB: {
                Wreg.out = (int64_t) (MemControl->GetInstruction(Wreg.Memdest) & 0xFF);
                break;
            }
            case LH: {
                Wreg.out = (int64_t) (MemControl->GetInstruction(Wreg.Memdest) & 0xFFFF);
                break;
            }
            case LW: {
                Wreg.out = (int64_t) (MemControl->GetInstruction(Wreg.Memdest) & 0xFFFFFFFF);
                break;
            }
        }
    }

    void WriteBack() {       //写到寄存器
        if (!Wreg.writeback) return;
        else {
            Reg[Wreg.RegTowrite] = Wreg.out;
        }
    }

};

