# sscap
A C++ version of shadowsocks gui client

SSCap是一个Windows下C++版的Shadowsocks客户端. 相比其它的客户端更稳定更快速,不会出现其它版本中常现的接收数据错误.

2015.12.24 1.7
/////////////////////////////////////////////////////
1, 二维码截图方式修改.
2, 修正SOCKS 5的帐号密码的验证时的BUG
3, 修正系统代理PAC模式下的BUG: SSCAP绑定了1080外的其它端口,但是系统代理中PAC地址端口仍然是1080
4, 切换当前代理后,系统托盘中的当前代理信息不会改变.
5, 修改本地SOCKS 5代理端口后,系统设置中的代理端口不会改变.
6, 修改本地SOCKS 5代理端口后,privoxy中转端口未修改.

2015.12.15 1.6

/////////////////////////////////////////////////////

1, 应网友要求: 增加启动时最小化到系统托盘功能. (请在系统设置中启用此功能)

2015.12.12 1.5

/////////////////////////////////////////////////////

1, 增加端口被占用时自动搜索端口功能.

2015.12.08 1.4

/////////////////////////////////////////////////////

1, 增加'复制到'功能,可以将一个SS节点复制到二维码/JSON/SS链接

2, 可以手动修改本地SOCKS监听端口.

3, 可以通过将SSCap拷贝到新目录来实现运行多份SSCap实例.


2015.11.30 1.3

/////////////////////////////////////////////////////

1, 修正在rc4-md5,salsa20兩種加密方式下的嚴重BUG導至無法工作.

2, 修改系統代理的工作方式.


2015.11.28 1.2

/////////////////////////////////////////////////////

first issue, Key Features:

1, 支持aes-256-cfb/aes-192-cfb/aes-128-cfb,md5,rc-md5,chacha20,salsa20 加密.

2, 更稳定,更快速. 不再会出现其它版本中的在某些系统中偶然性的接收数据错误.

3, 能在sscap中测试一个SS节点是否可用.

4, 可以显示每个节点的的速度,流量,错误等统计信息.

5, 支持从win xp 到 win 10 系统.

6, 支持从二维码/JSON格式批量/SS链接方式加入节点.

7, 更简洁,清晰的操作方式.
