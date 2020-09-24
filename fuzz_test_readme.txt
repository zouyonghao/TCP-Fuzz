fcntl 支持F_SETFL、F_GETFL
ioctl 支持FIONREAD/SIOCINQ、FIONBIO、SIOCGSTAMP
setsockopt 支持IPV6_V6ONLY、SO_REUSEADDR、SO_KEEPALIVE
                SO_REUSEPORT、SO_TIMESTAMP、TCP_CORK、TCP_KEEPIDLE
                TCP_KEEPINTVL、TCP_KEEPCNT、TCP_QUICKACK

* 对建立链接的两个文件描述符都测试syscall

* 构造异常包,支持多种异常包的模式
  - 损坏包
  - 重复包
  - 乱续包

* 异常syscall


* 增加状态变量

* 多连接