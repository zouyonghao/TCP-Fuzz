# Fuzzing 工具代码说明

`tcp_fuzzer`主要基于`packetdrill`修改而来，加入了包和系统调用的生成功能，主要代码集中在以下几个文件中:

`fuzz_test_helper.c, ~.h`: 一些辅助函数，用于读取Fuzzing文件。

`fuzz_test_packet.c, ~.h`: 有关包的一部分函数，主要为生成正常包，收集和验证回包的线程函数。

`fuzz_test_result.c, ~.h`: 运行过程中记录结果的一些函数。

`fuzz_test_script.c, ~.h`: 运行过程中记录生成的脚本的一些函数。

`fuzz_test_syscall.c, ~.h`: 生成和运行`syscall`的一些函数。

`fuzz_test.c`: 主要逻辑在这个文件中，`main`函数的逻辑主要是分别两次运行程序，然后比较结果。还有一些有关错误包的函数也在这个文件中。

其他文件: 修改了很多packetdrill的验证逻辑，主要是去掉了一些限制，还有一些地方增加了记录逻辑，用于后来比较，比如`run_system_call.c`的`syscall_read`记录了read结果。