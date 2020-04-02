tar -zxvf apache-zookeeper-3.6.0-bin.tar.gz -C /usr/local/zookeeper/

export ZOOKEEPER_HOME=/usr/local/zookeeper/apache-zookeeper-3.6.0-bin
export PATH=$PATH:$ZOOKEEPER_HOME/bin:$ZOOKEEPER_HOME/conf

最后，需要创建两个必备的目录（可以在任意路径下，本例在zookeeper的主目录下创建），一个用于装数据，另一个用于装日志，这两个目录必须创建，否则将会在使用过程中报错；

建议不要使用同一个目录，目录名建议采用data和log：

cd /usr/local/zookeeper/apache-zookeeper-3.6.0-bin
mkdir data
mkdir log

cd conf
cp zoo_sample.cfg zoo.cfg
vim zoo.cfg

修改
dataDir=/usr/local/zookeeper/apache-zookeeper-3.6.0-bin/data
dataLogDir=/usr/local/zookeeper/apache-zookeeper-3.6.0-bin/log

启动
zkServer.sh start
停止
zkServer.sh stop