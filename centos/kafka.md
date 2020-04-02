tar -zxvf kafka_2.11-2.4.1.tgz
cp -r kafka_2.11-2.4.1 /usr/local/kafaka_2.11-2.4.1
vim server.properties
修改日志路径 log.dirs=/usr/local/kafaka_2.11-2.4.1/kafak_logs
日志超时时间 log.retention.hours=168000

bin/kafka-server-start.sh config/server.properties 
后台启动 bin/kafka-server-start.sh config/server.properties 1>/dev/null 2>&1 &