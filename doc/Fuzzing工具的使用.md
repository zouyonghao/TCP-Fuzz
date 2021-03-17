# Fuzzing 工具的使用

## 编译和部署

### Fuzzing工具

- Fuzzer，负责Fuzzing核心逻辑，根据覆盖率进行种子挑选和程序运行

  此工具为`AFLCplusplus`，我们直接提供二进制程序，使用时拷贝到`/bin`目录下。

- 编译和插桩工具，负责对待测程序进行编译和插桩，使Fuzzer可以检测到覆盖率
  
	此工具为`state_monitor_compiler`、`state_monitor_instrument`、`branch_clear`和`lib_exfunc.o`，以及一个配置文件，`config.json`。我们也提供了二进制程序， 使用时拷贝到`/bin`目录下。

- 包和syscall生成工具，负责把Fuzzing文件"翻译"为包和syscall序列

	此工具为`fuzz_test`(即`tcp_fuzzer`，源代码编译出来还是叫`fuzz_test`)，我们提供源代码，编译时需要修改`Makefile`中的`TLDK_ROOT`变量，此工具需要编译完`TLDK`之后，直接`make`即可。
	
- 提供的工具均在`Ubuntu 18.04.5 LTS`下编译。

### TLDK

编译 `TLDK` 时，首先启用 `PACKETDRILL` 选项，进行一次正常编译，这一步的主要目的是使`DPDK`编译完成。然后`make clean`，再使用上述步骤中的`state_monitor_compiler`编译，编译脚本为：
```bash
cp /bin/config.json /tmp/config.json
/bin/branch_clear -c /tmp/config.json
make DEBUG_DPDK=y PACKETDRILL=y EXTRA_CFLAGS="-g -O0 -fPIC -march=native -w" all
make clean
make clean -C test/packetdrill
make CC=/bin/state_monitor_compiler DEBUG_DPDK=y PACKETDRILL=y EXTRA_CFLAGS="-g -O0 -fPIC -march=native -w" all
```

 编译完`TLDK`后，再编译`tcp_fuzzer`，修改`tcp_fuzzer/Makefile`中的`TLDK_ROOT`即可。

## 运行

单进程运行可以直接使用如下命令：

```bash
/bin/AFLCplusplus -i input -o output -f fuzz_test_input1 -M fuzzer1 -b \
  ../../fuzz_test --local_ip=192.168.1.1 --remote_ip=192.0.1.1 --so_filename=1 \
  --tolerance_usecs=200000000 \
  --fuzz_file_name=fuzz_test_input1 \
  --tun_id=1
```

**注意**

- `-i` 指定输入的种子的目录，测试前保证该目录存在且目录中存在与`-f`指定文件相同文件名的文件，例如上述命令中，需要保证`input`文件夹存在且`input/fuzz_test_input1`文件存在，多进程时，这个目录最好不同
- `-o` 指定输出目录，多进程最好相同，因为会自动建不同子目录
- `-M` 指定该进程为多进程中的`master`，如果是`slave`，则使用`-S`。后面需要跟参数作为名字，如`fuzzer1`
- `fuzz_file_name` 参数的值需要与 `-f` 的值一致
- `-b`指定使用的覆盖率指标为分支变化，如果去掉则覆盖率指标为`AFL`的代码覆盖率
- 如果多进程运行，则`-f`、`-M`、`--tun_id`、`--local_ip`和`--fuzz_file_name`的值需要不同，`-o`的值相同，具体可以看`scripts/tldk/fuzz_test_afl_slave[2-5].sh`这几个脚本

我已经写了几个脚本，放在`scripts/tldk`下，包括`fuzz_test_afl_master1.sh`，一般可以直接启动。

如果多进程测试，可以直接用`fuzz_test_afl_tmux`这个脚本，会启动一个`tmux`窗口，自动运行5个进程的测试，如果想要更多进程，可以修改该脚本。如果想要分开启动，记得先启动`master`，再启动`slave`。

## bug验证

- 是否产生`bug`，可以看`output/[fuzzer_name]`目录下的`plot-curve`文件，如`output/fuzzer1/plot-curve`，其中第三列表示是否有`bug`. 也可以直接看`output/fuzzer1/crash`下是否有文件，如果有，则说明该文件可以造成`crash`。

- `bug`复现，可以使用如下脚本

	```bash
	../../fuzz_test --local_ip=192.168.1.1 --remote_ip=192.0.1.1 \
		--so_filename=1 --tolerance_usecs=200000000 \
		--fuzz_file_name=./000001-000001-xxx \
		--tun_id=0
	```
- 复现时，运行`fuzz_test`可能会产生较多日志，通过分析日志可以看出`TLDK`与`Linux`的不同（比如会输出`TLDK is xxx，Linux is xxx`）。

- 日志中还包括产生的包和`syscall`，可以复制出脚本使用`packetdrill`复现。日志中的脚本不包括建立连接的语句，一般情况下使用的是`fuzz_test_pkt_accept.pkt`这个模板，即被动接收连接的情况。如果日志前几行有`use fuzz_test_pkt_connect.pkt`的语句，则使用的是`fuzz_test_pkt_connect.pkt`这个模板，拷贝脚本时需要注意。

- 还可以在编译时加入 `-DDEBUG_FUZZ_LOGGING` 可以看到很多Debug日志。

