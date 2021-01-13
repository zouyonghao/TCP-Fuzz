- [x] 增加对断开连接、半开连接的测试

- [x] 敏感度实验

        coverage(old/new) 随时间变化情况,及总数
        bug 数
        bug 出现时间

1. 传统coverage, 能否找到bug, 看if变化的coverage * **此实验效果较差，需要注意**

    1.1 统计new cov下old cov的变化

        TLDK
            -tldk/new_cov_normal_output
        F-stack
            -f-stack/new_cov_normal_output
        mTCP
            -mtcp/new_cov_normal_output

2. 只fuzz包和只有syscall的对比

    2.1 old coverage + syscall ~= syzkaller

            TLDK
                -tldk/old_cov_syscall_output
            F-stack
                -f-stack/old_cov_syscall_output
            mTCP
                -mtcp/old_cov_syscall_output
        old coverage + packet ~= AFL
            TLDK
                -tldk/old_cov_packet_output
            F-stack
                -f-stack/old_cov_packet_output
            mTCP
                -mtcp/old_cov_packet_output
                -mtcp/old_cov_packet_output2
    2.2 old coverage + 单个包/syscall

            TLDK
                -tldk/old_cov_fuzz_once_output
            F-stack
                -f-stack/old_cov_fuzz_once_output
            mTCP
                -mtcp/old_cov_fuzz_once_output

        old coverage + 不考虑包和syscall关系, 随机
            TLDK
                -tldk/old_cov_no_dependency_output
            F-stack
                -f-stack/old_cov_no_dependency_output
            mTCP
                -mtcp/old_cov_no_dependency_output

对比实验
1. <del>AFLNet</del>
2. boofuzz ✓
3. fuzzotron ✓

扩展

    细粒度的序列mutation
        目前的序列变化无法判断序列中某些影响较大的包
        而且对于不影响覆盖率的包和syscall也未进行处理
        后续可能增加更加细粒度的变化

    上下文敏感的分支覆盖
        目前的分支数组为定长数组，后续可以改为上下文敏感的分支覆盖（加入调用栈因素）

- [x] Asan 实验
- [x] 统计 bug 数目

- [x] 统计 bug root cause
- [x] 统计 bug fix effort

- [x] 具体的RFC违反
- [?] 做图表
        coverage 两个
        bug 统计图 柱状图分为 semantic 和 memory

- [ ] 跑太慢的问题

- [ ] kernel 覆盖率实验
- [ ] 与Filesystem论文对比
        使用 code coverage，去掉dep, 使用它的贪心算法