cp UserServer/UserServer bin/UserServer/UserServer_centos_x64
cp ManagerServer/ManagerServer bin/ManagerServer/ManagerServer_centos_x64
cp InterfaceServer/InterfaceServer bin/InterfaceServer/InterfaceServer_centos_x64
cp FileServer/FileServer bin/FileServer/FileServer_centos_x64

# 运行库目录
## make到指定目录
 ./configure --prefix=/usr/local/test
export LD_LIBRARY_PATH=/root/GraphicServer2.0/runtimes/protobuf:$LD_LIBRARY_PATH

# 修改hosts
vim /etc/hosts
127.0.0.1 http://www.baidu.com
## 立即生效
/etc/init.d/network restart

#后台启动
nohup dotnot GraphicServer.dll 1>/dev/null 2>&1 &
nohup UserServer UserServer.json 1>/dev/null 2>&1 &