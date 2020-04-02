# 下载镜像
docker pull centos:7.2.1511
# 查看镜像
docker image ls
https://www.runoob.com/docker/docker-run-command.html
# 查看运行中的容器
docker ps
# 查看所有容器，包括停止的
docker ps -a
# 端口映射
docker run -p 6000-6100:6000-6100/tcp
# 后台启动
docker run -d
# 命名
docker run --name myname
# 综合启动 
                                                                             (镜像ID)
docker run -itd -p 60000-60010:60000-60010/tcp -p 9092:9092/tcp --name test_image abd097e3cae1 
docker run -itd -p 60000-60010:60000-60010/tcp -p 9092:9092/tcp -p 61000:61000/tcp --name centos7 08d05d1d5859
docker run -itd -p 60000-60010:60000-60010/tcp -p 9092:9092/tcp -p 61000:61000/tcp --name centos7 --privileged=true 08d05d1d5859 /usr/sbin/init
docker kill test_image
docker start test_image
# 进入容器
                (容器ID)
docker exec -it df6aa0bd59fe /bin/bash
# 退出容器 
Ctrl+P+Q
# 修改

首先修改hostconfig.json
vim /var/lib/docker/containers/[hash_of_the_container]/hostconfig.json
然后修改config.v2.json
重启 service docker restart
docker inspect 容器名

yum list | grep initscripts

# 导出容器到镜像
docker export containID > filename
# 导入镜像文件
docker import filename [newname]

# 删除本地镜像
docker rim 镜像ID

# 删除容器
docker rm containerID

# 启动docker 
systemctl start docker