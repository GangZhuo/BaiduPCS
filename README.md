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
    make ver=release
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
              "enable": 1, /* 0 - 不启用该项；1 - 启用该项。 */
              "localPath": "/var/www", /*备份本地 /var/www 目录*/
              "remotePath": "/backup/www", /*备份到网盘 /backup/www 目录*/
              "method": "backup", /*执行的操作。
                                     reset   - 重置本地缓存。
                                     update  - 更新本地缓存。
                                     backup  - 备份 /var/www 到 /backup/www。
                                               如果网盘中存在的文件或目录在本地不存在，将删除网盘中文件和目录，
                                               即一切以本地为准（备份完成后，网盘和本地的目录结构一模一样）。
                                               如果本地文件在网盘中已经存在，
                                               则如果本地文件的修改时间后于网盘文件备份时间时，将重新备份该文件，
                                               否则跳过该本地文件。
                                     restore - 还原 /backup/www 到 /var/www，
                                               如果本地存在的文件或目录在网盘中不存在，将删除本地文件或目录，
                                               即一切以网盘为准（还原完成后，本地和网盘的目录结构一模一样）。
                                               如果本地文件的修改时间先于网盘文件的备份时间时，
                                               将使用网盘文件覆盖本地文件，否则保留本地文件。
                                     combin  - 合并/var/www 和 /backup/www,
                                               即先执行backup把本地未备份的或改动过的文件和目录备份到服务器
                                               （执行combin时不会删除网盘中文件），
                                               再执行restore把网盘中的新文件或改动时间比本地后的文件目录还原到本地
                                               （执行combin时不会删除本地文件）。
                                               注意：合并时，如果本地存在文件或目录A，网盘存在同名目录或文件A，
                                                     则网盘中目录或文件A的内容将丢失（丢失内容可在网盘回收站中找到）。*/
              "schedule": "01:20:00", /*任务开始时间。格式：hh:mm:ss。01:20:00表示：任务开始于凌晨1点20分。 */
              "interval": "01:00:00:00" /*任务完成后，到下次执行间的间隔。格式：dd:hh:mm:ss。
                                          01:00:00:00表示，任务完成后，间隔1天将再次执行*/
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
### 重置本地缓存（只有reset, update, backup, restore, combin, compare, ls-op 7个命令使用本地缓存）
    pcs reset --cache=<cache file path>
    or
    pcs reset --config=<config file path>
### 更新<remote path>的本地缓存
    pcs update --cookie=<cookie file> --cache=<cache file> <remote path>
    or
    pcs update --config=<config file path> <remote path>
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
    行为见编译安装中第5节的配置文件
### 打印服务器的本地日期和时间
    pcs time
### 列出svc模式下各任务的情况
    pcs ls-op --cache=<cache file>
    or
    pcs ls-op --config=<config file path>
    
    
