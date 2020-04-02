# 添加库路径
echo "/usr/local/lib" >> /etc/ld.so.conf
echo "/root/GraphicServer2.0/runtimes/protobuf" >> /etc/ld.so.conf
echo "/root/GraphicServer2.0/runtimes/librdkafka-1.3.0/lib" >> /etc/ld.so.conf
echo "/root/GraphicServer2.0/runtimes/sqlite-3/lib" >> /etc/ld.so.conf
echo "/usr/local/lib64" >> /etc/ld.so.conf
ldconfig

在/etc/rc.d/init.d/中创建脚本文件

## 脚本文件如下

#!/bin/bash
#/etc/rc.d/init.d/GraphicServer

# Source function library.
. /etc/init.d/functions


start() {
        echo -n "Starting GraphicServer... "
        nohup dotnet /root/GraphicServer2.0/GraphicServer/netcoreapp3.1/GraphicServer.dll /root/GraphicServer2.0/GraphicServer/netcoreapp3.1/GraphicServer.json 1>/dev/null 2>&1 &
        return 0
}

stop() {
        echo -n "Shutting down GraphicServer... "
        kill -9 `pgrep GraphicServer`
        return 0
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    status)
        ;;
    restart)
        stop
        start
        ;;
    reload)
        ;;
    *)
        echo "Usage: kafka {start|stop|status|reload|restart}"
 exit 1
        ;;
esac
exit $?

