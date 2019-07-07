# RiscV Big homework
---

代码结构
1. `namespace.hpp` 一些变量与riscv指令
2. `memory.hpp` 控制内存读写，数组单位为8bit
3. `program.hpp` 主要程序与五级流水
    1. `Fetch()` 读入`fReg.pc`的指令并且存入`dReg`. `dReg.bubble = false`
    表示`decode`过程有输入。
    2. `Decode` 对`dReg`中的指令进行解码，存入`eReg`，若读取到J类或者
    B类指令，则令`ControlHazard = true`,此时不再进行`fetch()`操作.
    3. `Execute` 其余三步同理操作即可。
    4. `MemortAccess`
    5. `WriteBack`
    
4. `score`, shell脚本，`./score`可测试正确性。
