**1) 使用pegsus上自带的程序测试camera能否正常启动**

> /usr/local/driveworks/bin/sample_camera_multiple_gmsl
> Tips
> 如果要验证A和B都能正常开启camera，则需要分别在A和B上分别执行测试程序，如果B上相应路径没有，则可以从A上拷贝一份。运行时加上掩码以区分两边各自需要开启的camera id，如：
>
> * A: /pathTo/sample_camera_multiple_gmsl --selector-mask=1111
> * B: /pathTo/sample_camera_multiple_gmsl --selector-mask=00000111

**2）如果测试程序无法正常启动，则需要重启系统**

> a) 笔记本连接pegsus串口：pegsus上接debug口
> b) 查看笔记本系统（linux）的/dev目录下存在十几个新设备
>
> * ls /dev/USB*
>
> c) 使用minicom工具连接/dev/ttyUSB3
>   sudo minicom -D /dev/ttyUSB3 -b 115200
>
> d) 进入界面后运行如下命令，重启系统
>
>   tegrareset x1  # 重启XA
>   tegrareset x2  # 重启XB

**3）如果双系统都无法启动camera的时候，建议**
> 先单独重启A，并验证A能否启动camera，如果A无法启动camera，则通过2）所述方法再次重启A，直到A能正常启动camera
> 然后再通过2）所述方式重启B，直到B也能启动camera

**4）camera启动不了基本上是常见问题，冷静的反复重复2）或3）步骤直到问题解决即可**
