https://blog.zfanw.com/centos-postgresql-command/

# 切换到 postgres 用户

$ sudo -i -u postgres
$ psql

# 退出 PostgreSQL 命令行
postgres=# \q

# 查看所有数据库
postgres=# \l

# 连接数据库
postgres=# \c test

# 查看表
postgres=# \d

# 查看用户创建的表
postgres=# \dt

# 查看单个数据表的结构

postgres=# \d test