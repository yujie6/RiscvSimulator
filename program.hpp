//
// Created by yujie6 on 19-7-2.
//

#include <fstream>
#include <cstring>
#include <cstdio>
#include "Namespace.hpp"
#include "memory.hpp"

namespace yujie6 {
    struct PipeRegister {
        uint64_t Imm1;
        int64_t op1, op2;
        uint32_t dest;
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
    MemoryController *MemControl;
    int ProgramCounter;
    Inst CurrentInst;
    PipeRegister Fereg;
    uint32_t inst;
    int Reg[32];
    bool DataHazard;
    bool ControlHazard;
    int cycle;
    FReg fReg;
    DReg dReg;
    EReg eReg;
    MReg mReg;
    WReg wReg;

    program() {
        memset(Reg, 0, sizeof(Reg));
        MemControl = new MemoryController();
        fReg.pc = 0;
        cycle = 0;
        DataHazard = ControlHazard = false;
        fReg.bubble = false;
        dReg.bubble = eReg.bubble = true;
        mReg.bubble = wReg.bubble = true;
    }

    inline void FiveStageRun() {
        while (true) {
            cycle++;
            WriteBack();
            MemoryAccess();
            Execute();
            Decode();
            Fetch();
            if (!fReg.bubble && !dReg.bubble && !eReg.bubble && !mReg.bubble ) {
                cout << (((unsigned int) Reg[10]) & 255u);
                break;
            }
        }
    }


    inline void Run() {

        while (true) {
            Fetch();
            if (inst == 13009443) {
                auto ans = (((unsigned int) Reg[10]) & 255u);
                cout << ans << "\n";
                break;
            }
            Decode();
            Execute();
            MemoryAccess();
            WriteBack();
            auto ans = (((unsigned int) Reg[10]) & 255u);
            //cout << std::hex << ProgramCounter << " ";
            //cout << std::dec << ans << "\n";
            if (Reg[0] != 0) Reg[0] = 0;
        }
    }

    void Fetch() {
        if (ControlHazard) {
            dReg.bubble = true;
            return;
        }
        else ProgramCounter = fReg.pc;
        dReg.srcInst = MemControl->GetInstruction(ProgramCounter);
        dReg.pc = ProgramCounter;
        dReg.bubble = false;
        fReg.pc = ProgramCounter + 4;
        if (dReg.srcInst == 13009443) fReg.bubble = true;
    }

    void Decode() {
        if (dReg.bubble || ControlHazard) {
            if (dReg.bubble) eReg.bubble = true;
            return;
        }
        inst = dReg.srcInst;
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
        switch (opcode) {
            case OP_LUI: {
                eReg.Imm = imm_u;
                CurrentInst = LUI;
                break;
            }
            case OP_AUIPC: {
                ControlHazard = true;
                eReg.Imm = imm_u;
                CurrentInst = AUIPC;
                break;
            }

            case OP_REG: {
                eReg.op1 = Reg[rs1];
                eReg.op2 = Reg[rs2];
                eReg.Imm = 0; // R-type Get No imm
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
                eReg.op1 = Reg[rs1];
                eReg.Imm = imm_i;
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
                        Fereg.Imm1 = Fereg.Imm1 & 0x3F;
                        break;
                    case 0x5:
                        if (((inst >> 26) & 0x3F) == 0x0) {
                            CurrentInst = SRLI;
                            Fereg.Imm1 = Fereg.Imm1 & 0x3F;
                        } else if (((inst >> 26) & 0x3F) == 0x10) {
                            CurrentInst = SRAI;
                            Fereg.Imm1 = Fereg.Imm1 & 0x3F;
                        }
                        break;
                    default: {}
                }
                break;
            }
            case OP_LOAD: {
                eReg.op1 = Reg[rs1];
                eReg.Imm = imm_i;
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
                eReg.op1 = Reg[rs1];
                eReg.op2 = Reg[rs2];
                eReg.Imm = imm_s;
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
                ControlHazard = true;
                eReg.op1 = Reg[rs1];
                eReg.op2 = Reg[rs2];
                eReg.Imm = imm_sb;
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
                ControlHazard = true;
                CurrentInst = JAL;
                eReg.Imm = imm_uj;
                break;
            }
            case OP_JALR: {
                ControlHazard = true;
                CurrentInst = JALR;
                eReg.op1 = Reg[rs1];
                eReg.Imm = imm_i;
                break;
            }
            case OP_SYSTEM: {
                eReg.op1 = Reg[rs1];
                eReg.Imm = csr;
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
        eReg.dest = rd;
        eReg.pc = dReg.pc;
        eReg.inst = CurrentInst;
        eReg.bubble = false;
    }

    void Execute() {
        if (eReg.bubble) {
            mReg.bubble = true;
            return;
        }
        mReg.RegTowrite = eReg.dest;
        switch (eReg.inst) {
            case ADD: {
                mReg.out = eReg.op1 + eReg.op2;
                break;
            }
            case ADDI: {
                mReg.out = eReg.op1 + eReg.Imm;
                /*
                 *   TODO if Wreg.out is larger than 1<<32,
                 *    then forsake the high unit
                 */
                break;
            }
            case SUB: {
                mReg.out = eReg.op1 - eReg.op2;
                break;
            }
            case LUI: {
                mReg.out = eReg.Imm<<12;
                break;
            }
            case AUIPC: {
                fReg.pc = eReg.pc + (eReg.Imm<<12);
                mReg.out = eReg.pc + 4;
                break;
            }
            case JAL: {
                mReg.out = eReg.pc + 4;
                fReg.pc = eReg.pc + eReg.Imm;
                break;
            }
            case JALR: {
                mReg.out = eReg.pc + 4;
                fReg.pc =  (eReg.op1 + eReg.Imm) & (~(uint64_t) 1);
                break;
            }
            case SLL: {
                mReg.out = eReg.op1 << eReg.op2;
                break;
            }
            case SLLI: {
                mReg.out = (unsigned)eReg.op1 << (int)((eReg.Imm<<27)>>27);
                break;
            }
            case SLTU: {
                mReg.out = ((uint64_t)eReg.op1 < (uint64_t)eReg.op2) ? 1 : 0;
                break;
            }
            case SLT: {
                mReg.out = (eReg.op1 < eReg.op2) ? 1 : 0;
                break;
            }
            case SLTIU: {
                mReg.out = ((uint64_t) eReg.op1 < (uint64_t) eReg.Imm) ? 1 : 0;
            }
            case SLTI: {
                mReg.out = (eReg.op1 < eReg.Imm) ? 1 : 0;
            }
            case SRL: {
                mReg.out = (uint64_t) eReg.op1 >> (uint64_t) eReg.op2;
                break;
            }
            case SRLI: {
                mReg.out =  (unsigned)eReg.op1 >> (int) ((eReg.Imm<<27)>>27); //符号拓展？？
                break;
            }
            case SRA: {
                mReg.out = eReg.op1 >> (eReg.op2 & 0x3F);
                break;
            }
            case SRAI: {
                mReg.out = eReg.op1 >> (int) ((eReg.Imm<<27)>>27);
                break;
            }
            case XOR: {
                mReg.out = eReg.op1 ^ eReg.op2;
                break;
            }
            case XORI: {
                mReg.out = eReg.op1 ^ eReg.Imm;
                break;
            }
            case OR: {
                mReg.out = eReg.op1 | eReg.op2;
                break;
            }
            case ORI: {
                mReg.out = eReg.op1 | eReg.Imm;
                break;
            }
            case ANDI: {
                mReg.out = eReg.op1 & eReg.Imm;
                break;
            }
            case AND: {
                mReg.out = eReg.op1 & eReg.op2;
                break;
            }
            case BEQ: {
                if (eReg.op1 == eReg.op2) {
                    fReg.pc = eReg.pc + eReg.Imm;
                }
                break;
            }
            case BNE: {
                if (eReg.op1 != eReg.op2) {
                    fReg.pc = eReg.pc + eReg.Imm;
                }
                break;
            }
            case BLTU: {
                if ((uint64_t) eReg.op1 < (uint64_t) eReg.op2) {
                    fReg.pc = eReg.pc + eReg.Imm;
                }
                break;
            }
            case BLT: { //有符号数？？
                if (eReg.op1 < eReg.op2) {
                    fReg.pc = eReg.pc + eReg.Imm;
                }
                break;
            }
            case BGEU: {
                if ((uint64_t) eReg.op1 >= (uint64_t) eReg.op2) {
                    fReg.pc = eReg.pc + eReg.Imm;
                }
                break;
            }
            case BGE: {
                if (eReg.op1 >= eReg.op2) {
                    fReg.pc = eReg.pc + eReg.Imm;
                }
                break;
            }
            case SH: {
                mReg.Memdest = eReg.op1 + eReg.Imm;
                mReg.out = eReg.op2 & 0xFFFF;
                break;
            }
            case SW: {
                mReg.Memdest = eReg.op1 + eReg.Imm;
                mReg.out = eReg.op2 & 0xFFFFFFFF;
                break;
            }
            case SB: {
                mReg.Memdest = eReg.op1 + eReg.Imm;
                mReg.out = eReg.op2 & 0xFF;
                break;
            }
            case LHU:
            case LBU:
            case LWU:
            case LB:
            case LH:
            case LW: {
                mReg.Memdest = eReg.op1 + eReg.Imm;
                break;
            }
            default: {
            }
        }
        if (ControlHazard) {
            if (isJump(eReg.inst)) {
                ControlHazard = false;
            }
        }
        // Prepare for the next task
        mReg.writeback = !isBranchOrStore(CurrentInst);
        mReg.mem = isReadMem(CurrentInst);
        mReg.pc = eReg.pc;
        mReg.bubble = false;
        mReg.inst = eReg.inst;

    }

    void MemoryAccess() {    //写到内存
        if (mReg.bubble) {
            wReg.bubble = true;
            return;
        }
        wReg.bubble = false;
        wReg.pc = mReg.pc;
        wReg.inst = mReg.inst;
        wReg.out = mReg.out;
        wReg.RegTowrite = mReg.RegTowrite;
        wReg.writeback = mReg.writeback;
        if (!mReg.mem) {
            return;
        }
        switch (mReg.inst) {
            case SB: {
                MemControl->StoreByte(mReg.out, mReg.Memdest);
                break;
            }
            case SH: {
                MemControl->StoreHalfWord(mReg.out, mReg.Memdest);
                break;
            }
            case SW: {
                MemControl->StoreWord(mReg.out, mReg.Memdest);
                break;
            }
            case LBU: {
                wReg.out = (uint64_t) (MemControl->GetInstruction(mReg.Memdest) & 0xFF);
                break;
            }
            case LHU: {
                wReg.out = (uint64_t) (MemControl->GetInstruction(mReg.Memdest) & 0xFFFF);
                break;
            }
            case LWU: {
                wReg.out = (uint64_t) (MemControl->GetInstruction(mReg.Memdest) & 0xFFFFFFFF);
                break;
            }
            case LB: {
                wReg.out = (int64_t) (MemControl->GetInstruction(mReg.Memdest) & 0xFF);
                break;
            }
            case LH: {
                wReg.out = (int64_t) (MemControl->GetInstruction(mReg.Memdest) & 0xFFFF);
                break;
            }
            case LW: {
                wReg.out = (int64_t) (MemControl->GetInstruction(mReg.Memdest) & 0xFFFFFFFF);
                break;
            }
            default: {}
        }
    }

    void WriteBack() {       //写到寄存器
        if (wReg.bubble) return;
        if (!wReg.writeback) {
            return;
        } else {
            Reg[wReg.RegTowrite] = wReg.out;
        }
    }

};

