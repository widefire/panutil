
mkdir /usr/local/java/
tar -zxvf jdk.tar.gz -C /usr/local/java/
vim /etc/profile

export JAVA_HOME=/usr/local/java/jdk1.8.0_241
export JRE_HOME=${JAVA_HOME}/jre
export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib
export PATH=${JAVA_HOME}/bin:$PATH

ln -s /usr/local/java/jdk1.8.0_241/bin/java /usr/bin/java