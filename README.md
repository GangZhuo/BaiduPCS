C/C++写的一个百度网盘工具，可以在linux终端中使用，目的是备份vps服务器数据。
这是通过分析网盘网站得到的直接接口，不需要创建应用。
如果有错误、建议、问题等，请发邮件gang.zhuo[AT]gmail.com。


编译安装：
===================================
### 1. 安装 libcurl
    apt-get install libcurl4-openssl-dev
### 2. 获取源代码
    git clone https://github.com/GangZhuo/baidupcs.git
### 3. 编译源代码
    cd baidupcs
    mkdir bin
    make clean
    make
    make install #将安装到/usr/local/bin下
### 4. 安装到 /usr/local/bin 下 
    cp ./bin/pcs /usr/local/bin/
    chmod a+x /usr/local/bin/pcs
### 5. 配置自动执行备份(Debian为例)
    a) 创建配置文件
        mkdir /etc/default/pcs /*创建存储配置文件的目录*/
        mkdir /var/local/pcs /*创建存储cookie和本地缓存文件的目录*/
        vim /etc/default/pcs/default.json 
        default.json 内容如下：
          /*配置文件 - 开始*/
          {
            "cookieFilePath": "/var/local/pcs/default.cookie", /*执行任务时，使用的cookie文件路径*/
            "cacheFilePath": "/var/local/pcs/cache.db", /*本地缓存文件的路径*/
            "logFilePath": "/var/log/pcs.log", /*日志文件路径*/
            "items": [{ 
                /*
                  items为数组，定义了一些在服务模式下执行的任务。此项只有在服务模式下运行才会起作用。*/
                  /*服务模式，见 “命令” 节中的 “以服务模式运行”
                */
              "enable": 1,                 /* 0 - 不启用该项；1 - 启用该项。 */
              "localPath": "/var/www",     /*备份本地 /var/www 目录*/
              "remotePath": "/backup/www", /*备份到网盘 /backup/www 目录*/
              "method": "backup", /*执行的操作。
                                     reset   - 重置本地缓存。
                                               注意：只清理因为程序Crash导致的，提示另一个程序在运行的错误
                                     update  - 更新 /backup/www 的本地缓存。
                                     backup  - 备份 /var/www 到 /backup/www。
                                               如果网盘中存在的文件或目录在本地不存在，将删除网盘中文件和目录，
                                               即一切以本地为准（备份完成后，网盘和本地的目录结构一模一样）。
                                               如果本地文件在网盘中已经存在，则根据是否启用MD5来分情况处理。
                                               启用MD5时，如果MD5值不同，则备份文件到网盘，此时网盘文件被覆盖。
                                               未启用MD5时，如果本地文件的修改时间大于网盘文件备份时间，将备份文件到网盘，
                                               此时网盘文件被覆盖，否则跳过该文件。
                                               如果本地是文件，但是网盘是目录，则会删除网盘目录后再备份本地文件，
                                               如果本地是目录，但是网盘是文件，则会删除网盘文件后再备份本地目录。
                                               如果本地文件或目录在网盘中不存在，则会备份本地文件或目录到网盘。
                                               如果网盘文件或目录在本地文件系统中不存在，则会删除网盘文件或目录。
                                     restore - 还原 /backup/www 到 /var/www，
                                               如果本地存在的文件或目录在网盘中不存在，将删除本地文件或目录，
                                               即一切以网盘为准（还原完成后，本地和网盘的目录结构一模一样）。
                                               如果本地文件在网盘中已经存在，则根据是否启用MD5来分情况处理。
                                               启用MD5时，如果MD5值不同，则还原文件到本地文件系统，此时本地文件被覆盖。
                                               未启用MD5时，如果网盘文件备份时间大于本地文件的修改时间，
                                               将还原文件到本地文件系统，
                                               此时本地文件被覆盖，否则跳过该文件。
                                               如果本地是文件，但是网盘是目录，则会删除本地文件后再还原网盘目录
                                               到本地文件系统，
                                               如果本地是目录，但是网盘是文件，则会删除本地目录后再还原文件
                                               到本地文件系统。
                                               如果本地文件或目录在网盘中不存在，则会删除本地文件或目录。
                                               如果网盘文件或目录在本地文件系统中不存在，则会还原网盘文件或目录到本地。
                                     combin  - 合并/var/www 和 /backup/www。
                                               未启用MD5时，比较本地文件修改时间和网盘文件备份时间，如果本地文件
                                               修改时间大于网盘文件备份时间，则备份该文件到网盘，
                                               如果本地文件修改时间小于网盘文件备份时间时，
                                               则还原网盘文件到本地，否则跳过文件。
                                               如果本地是文件，但是网盘是目录，则会删除网盘目录后再备份文件，
                                               如果本地是目录，但是网盘是文件，则会删除网盘文件后再备份本地目录。
                                               如果本地文件或目录在网盘中不存在，则会备份本地文件或目录到网盘。
                                               如果网盘文件或目录在本地文件系统中不存在，则会还原网盘文件或目录到本地。
                                               启用MD5时，如果MD5值不同，则按照未启用MD5时的时间比较逻辑，唯一区别是
                                               当时间相等时，抛出错误，并停止执行，MD5值相同时，跳过文件。
                                               注意：合并时，如果本地存在文件或目录A，网盘存在同名目录或文件A，
                                                     则网盘中目录或文件A的内容将丢失
                                                     （丢失内容可在网盘回收站中找到）。
                                                     例：本地存在一个文件/var/www/data.bak，而网盘中存在一个
                                                         目录/backup/www/data.bak，当执行合并时，
                                                         将使用本地文件覆盖网盘中的data.bak目录，此时网盘中的
                                                         data.bak目录被移入网盘回收站。
                                                     例2：本地存在一个目录/var/www/upload.bak，而网盘中存在一个
                                                          文件/backup/www/upload.bak，当执行合并时，
                                                          将使用本地目录覆盖网盘中文件，此时网盘中文件将
                                                          移入到网盘回收站。*/
              "schedule": "01:20:00",     /*任务开始时间。格式：hh:mm:ss。01:20:00表示：任务开始于凌晨1点20分0秒。 */
              "interval": "01:00:00:00",  /*任务完成后，到下次执行间的间隔。格式：dd:hh:mm:ss。
                                            01:02:03:04表示，任务完成后，
                                            本次任务开始时间加上1天2小时3分钟4秒得到时间作为下次任务的执行时间*/
              "md5": 1                    /*执行backup,restore和combin时，是否先使用MD5判断文件内容不同后，
                                            再比较时间。0 - 不使用MD5； 1 - 使用MD5。*/
            }, {
                "enable": 1,
                "localPath": "/var/data/data.db",
                "remotePath": "/backup/data/data.db",
                "method": "backup",
                "schedule": "01:20:00",
                "interval": "01:00:00:00"
            }]
          }
          /*配置文件 - 结束*/
          
     b) 登录一次，产生cookie文件
        pcs -u<account> quota --urlc --cookie=/var/local/pcs/default.cookie
     c) 使用 screen 启动服务
        screen -S backup
        pcs svc --config=/etc/default/pcs/default.json
        按下 ctrl+a 然后按下d来分离screen。
     d) 查看任务
        pcs ls-op --config=/etc/default/pcs/default.json
        查看 [SVC TASK] 节下的任务计划情况。
     e) 查看日志
        tail -20 /var/log/pcs.log
     f) 比较本地文件和网盘文件
        pcs compare --config=/etc/default/pcs/default.json /var/www /backup/www
     g) 如果出现类似"There have another * thread running, which is start by..." 的错误，执行一次以下命令来重置缓存状态
        pcs reset --config=/etc/default/pcs/default.json
        
    
命令：
===================================
如果出现中文乱码，请检查操作系统的当前编码是否为UTF8。
注意：网盘文件或目录全部使用完整路径，否则情况未知。
      例：网盘文件 /backup/data/info.txt，网盘目录 /backup/data 不能省略最前边的斜杠，也不能只写info.txt或data。

### 登录网盘
    mkdir ~/.baidupcs
    pcs -u<username> --urlc quota
    
    执行任意命令，如果未登录会提示登录。登录时，可能需要输入验证码，提供两种方式可以查看验证码：
      * 保存验证码图片到~/.baidupcs/verify_code.gif, 自己scp到本地查看。此为默认方式。
      * 上传验证码图片到[http://urlc.cn/g/]，自己找到并打开返回的网址查看。
        使用--urlc选项可开启此模式。
      * 另一种选择是：在本地运行pcs并登录后，然后把本地~/.baidupcs/default.cookie文件
        复制到服务器~/.baidupcs/default.cookie，服务器中pcs将自动使用该cookie。

### 显示网盘配额
    pcs quota
### 直接保存文本到网盘中
    pcs echo <path> <text>
    pcs echo /data.txt "The text that saved by pcs."

    注意：
      * 将使用当前的系统编码来保存内容到/data.txt文件。如果当前系统编码为UTF-8，
        则在非UTF-8编码的机器上使用cat显示时会出现中文乱码。
      * 安全起见，请在本地编辑好文件，使用upload上传。

### 直接显示网盘中文本文件内容
    pcs cat <path>
    pcs cat /data.txt

    注意：
      * 将使用当前的系统编码来显示/data.txt文件内容。如果data.txt文件是以ASCII编码保存的，
        而当前系统为UTF8编码，则可能出现中文乱码。此时可使用download下载到本地后使用第三方工具查看其内容。

### 把文本添加到网盘文件的末尾
    pcs -a echo <path> <text>
    pcs -a echo /data.txt "The text that will append to the end of the file."

    注意：
      * 将使用当前的系统编码来保存内容到/data.txt文件。如果当前系统编码为UTF-8，
        则在非UTF-8编码的机器上使用cat显示时会出现中文乱码。
        此时可使用download下载到本地后再查看其内容。
      * 如果原始文件的编码和当前系统编码不同，则原始内容会出现中文乱码。
      * 安全起见，请在本地编辑好文件，使用upload上传。

### 显示网盘文件或目录的元数据
    pcs meta <path>
    pcs meta /data.txt
### 列出网盘根目录下的文件或目录，且按最后修改日期倒序排序
    pcs [options] list <path>
    pcs --sort=time -i list /
    
    输出格式为：
        * 第一列指示是否是目录，如果是目录则输出 _d _，否则输出 -
        * 第二列是文件或目录的最后修改时间
        * 第三列是文件的完整路径
### 重命名网盘文件或目录
    pcs rename <path> <new name>
    pcs rename /data.txt data_20140118.txt
### 创建目录
    pcs mkdir <path>
    pcs mkdir /subdir
### 移动网盘文件或目录
    pcs move <path> <new path>
    pcs move /data_20140118.txt /subdir/data.txt
### 复制网盘文件或目录
    pcs copy <path> <new path>
    pcs copy /subdir/data.txt /subdir/data_20140118.txt
### 删除文件或目录
    pcs remove <path>
    pcs remove /subdir/data_20140118.txt
### 递归搜索文件或目录
    pcs [-r] search <dir path> <key>
    pcs -r search / data
### 备份目录
    pcs [-rf] [--synch] <local path> <remote path>
    pcs mkdir /backup
    pcs -r upload /var/wwwroot/data /backup/data_20140118
### 备份文件
    pcs [-f] <local file> <remote path>
    pcs mkdir /backup
    pcs upload ~/data.tar.gz /backup/data.20140118.tar.gz
### 下载目录
    pcs [-rf] [--synch] <remote path> <local path>
    pcs -r download /backup/data_20140118 ~/download/data_20140118
### 下载文件
    pcs [-f] <remote path> <local path>
    pcs download /backup/data.20140118.tar.gz ~/download/data.20140118.tar.gz
### 查看帮助
    pcs --help
### 以服务模式运行
    pcs svc --config=<config file path>
    以服务模式运行时，程序不会退出，而是不停的循环检查配置文件中的任务，当任务的执行时间到达后，
    将执行该任务。任务见编译安装中第5节的配置文件。配置文件中的items设置，只有在服务模式下起作用。
### 重置本地缓存（只有reset, update, backup, restore, combin, compare, ls-op 7个命令使用本地缓存）
    pcs reset --cache=<cache file path>
    or
    pcs reset --config=<config file path>
    行为见编译安装中第5节的配置文件
### 更新<remote path>的本地缓存
    pcs update --cookie=<cookie file> --cache=<cache file> <remote path>
    or
    pcs update --config=<config file path> <remote path>
    行为见编译安装中第5节的配置文件
### 备份本地文件或目录<local path>到网盘文件或目录<remote path>
    pcs backup --cookie=<cookie file> --cache=<cache file> <local path> <remote path>
    or
    pcs backup --config=<config file path> <local path> <remote path>
    行为见编译安装中第5节的配置文件
### 还原网盘文件或目录<remote path>到本地文件或目录<local path>
    pcs restore --cookie=<cookie file> --cache=<cache file> <local path> <remote path>
    or
    pcs restore --config=<config file path> <local path> <remote path>
    行为见编译安装中第5节的配置文件
### 合并本地文件或目录<local path>和网盘文件或目录<remote path>
    pcs combin --cookie=<cookie file> --cache=<cache file> <local path> <remote path>
    or
    pcs combin --config=<config file path> <local path> <remote path>
    行为见编译安装中第5节的配置文件
### 比较本地文件或目录<local path>和网盘文件或目录<remote path>的异同
    pcs compare --cookie=<cookie file> --cache=<cache file> <local path> <remote path>
    or
    pcs compare --config=<config file path> <local path> <remote path>
    未启用MD5时，比较本地文件修改时间和网盘文件备份时间，如果本地文件修改时间大于
    网盘文件备份时间，则认为该文件需要备份到网盘，如果本地文件修改时间小于网盘文件备份时间时，
    则认为需要还原网盘文件到本地，否则跳过文件。
    启用MD5时，如果MD5值不同，则按照未启用MD5时的时间比较逻辑，唯一区别是
    当时间相等时，抛出错误，并停止继续执行，MD5值相同时，跳过文件。
    如果本地是文件，但是网盘是目录，则会输出2项，1)删除网盘目录 2)备份本地文件，
    如果本地是目录，但是网盘是文件，则会输出2项，1)删除网盘文件 2)备份本地目录。
    如果本地文件或目录在网盘中不存在，则认为需备份本地文件或目录到网盘。
    如果网盘文件或目录在本地文件系统中不存在，则认为需还原网盘文件或目录到本地。
### 打印服务器的本地日期和时间
    pcs time
### 列出svc模式下各任务的情况
    pcs ls-op --cache=<cache file>
    or
    pcs ls-op --config=<config file path>
    
    
可参考示例教程： http://www.cnblogs.com/beikx/p/3697149.html
