//
// Created by yujie6 on 19-7-2.
//


using namespace yujie6;
using std::cin;
using std::cout;

class MemoryController {
public:
    uint8_t memory[0x30000];
    int MaxPc;
    MemoryController() {
        init();
    }

    void init() {
        int32_t AddrNow = 0;
        std::string aa, bb, cc, dd;
        char tmp[20];
        while (scanf("%s", tmp) != EOF) {
            if (tmp[0] == 'q') {
                MaxPc = AddrNow;
                break;
            }
            if (tmp[0] == '@') {
                aa.clear();
                for (int i = 1; i <= 8; i++) { aa.push_back(tmp[i]);}
                AddrNow = (HexToInt(aa.substr(0, 2)) << 24) +
                          (HexToInt(aa.substr(2, 2)) << 16) +
                          (HexToInt(aa.substr(4, 2)) << 8) +
                          (HexToInt(aa.substr(6, 2)));

            } else {
                aa.clear();
                aa.push_back(tmp[0]); aa.push_back(tmp[1]);
                cin >> bb >> cc >> dd;
                memory[AddrNow] = HexToInt(aa);
                memory[AddrNow + 1] = HexToInt(bb);
                memory[AddrNow + 2] = HexToInt(cc);
                memory[AddrNow + 3] = HexToInt(dd);
                AddrNow += 4;
            }
        }
    }

    uint32_t GetInstruction(int addr) {
        return (memory[addr]) + (memory[addr + 1] << 8)
               + (memory[addr + 2] << 16) + (memory[addr + 3] << 24);
    }

    uint16_t GetHalfWord (int addr) {
        return (uint16_t) ((memory[addr]) + (memory[addr+1]<<8) );
    }

    uint8_t GetByte(int addr) {
        return (uint8_t)(memory[addr]);
    }

    void StoreWord(int64_t value, int addr) {
        uint32_t part1 = value & 0xFF;
        uint32_t part2 = (value>>8) & (0xFF);
        uint32_t part3 = (value>>16) & (0xFF);
        uint32_t part4 = (value>>24 & 0xFF);
        memory[addr] = part1;
        memory[addr + 1] = part2;
        memory[addr + 2] = part3;
        memory[addr + 3] = part4;
    }

    void StoreHalfWord(int64_t value, int addr) {
        uint32_t part1 = value & 0xFF;
        uint32_t part2 = (value>>8) & 0xFF;
        memory[addr] = part1;
        memory[addr + 1] = part2;
    }

    void StoreByte(int64_t value, int addr) {
        memory[addr] = (int32_t)value;
    }

    uint32_t GetCode(std::string a, std::string b, std::string c, std::string d) {
        uint32_t aa = HexToInt(a);
        uint32_t bb = HexToInt(b);
        uint32_t cc = HexToInt(c);
        uint32_t dd = HexToInt(d);
        return aa + (bb << 8) + (cc << 16) + (dd << 24);
    }

    uint32_t HexToInt(std::string str) {
        uint32_t ans = 0;
        if (str[0] >= '0' && str[0] <= '9') {
            ans += 16 * (str[0] - '0');
        } else {
            ans += 16 * (str[0] - 'A' + 10);
        }
        if (str[1] >= '0' && str[1] <= '9') {
            ans += (str[1] - '0');
        } else {
            ans += (str[1] - 'A' + 10);
        }
        return ans;
    }


};