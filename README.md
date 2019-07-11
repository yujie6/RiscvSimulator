# RiscV Big homework
---

## 代码结构
* `namespace.hpp` 一些变量与riscv指令
* `memory.hpp` 控制内存读写，数组单位为8bit
* `program.hpp` 主要程序与五级流水
* `score`, shell脚本，`./score`可测试正确性。
* `src\` 测试文件
## master分支
为普通暴力模拟
## FiveStage分支
带有五级流水和三周期的模拟。
#### **Private函数**
1. `Fetch()` 读入`fReg.pc`的指令并且存入`dReg`. `dReg.bubble = false`
    表示`decode`过程有输入。
2. `Decode` 对`dReg`中的指令进行解码，存入`eReg`，若读取到J类或者
    B类指令，则令`ControlHazard = true`,此时不再进行`fetch()`操作.
3. `Execute` 其余三步同理操作即可。
4. `MemortAccess` 三周期模拟， 用一个`bool`变量控制即可
5. `WriteBack`

#### **Public函数**
1. `Run()`，普通模拟，只输出答案。
2. `void FiveStageRun(bool slow)`, 如果`slow == true`,将储存历史命令。
3. `DumpHistory()` 打印历史命令。
    

