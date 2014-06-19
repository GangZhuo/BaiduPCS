C/C++写的一个百度网盘工具，可以在linux终端中使用，目的是备份vps服务器数据。
这是通过分析网盘网站得到的直接接口，不需要创建应用。
支持AES-CBC-128, AES-CBC-192, AES-CBC-256加密。
如果有错误、建议、问题等，请发邮件gang.zhuo[AT]gmail.com。


编译安装：
===================================
程序依赖于 libcurl。

### 1. 安装依赖
    apt-get install libcurl4-openssl-dev
### 2. 获取源代码
    git clone https://github.com/GangZhuo/baidupcs.git
### 3. 编译源代码
    cd baidupcs
    make clean
    make
    make install #将安装到/usr/local/bin下
### 4. 手动安装到其他目录，例如 /usr/bin 下 
    cp ./bin/pcs /usr/bin/
    chmod a+x /usr/bin/pcs

命令：
===================================
如果出现中文乱码，请检查操作系统的当前编码是否为UTF8。
命令中涉及到的网盘文件路径，可以是绝对路径，也可以是相对于当前工作目录的相对路径。
例：
    /data/data.txt 即是网盘绝对路径，表示网盘根目录下的data目录中的data.txt文件。
    data.txt 即是相对路径，表示当前工作目录下的data.txt文件。
    当前工作目录可通过 'pcs pwd' 命令来查看。
    可通过 'pcs --help' 查看支持的子命令；可在子命令后加 '-h' 查看子命令帮助，例如 'pcs move -h'。


### 登录网盘
    pcs login

### 显示当前登录用户
    pcs who

### 显示当前网盘的工作目录
    pcs pwd

### 显示网盘配额
    pcs quota

### 切换工作目录
    pcs cd <path>

### 直接保存文本到网盘中
    pcs echo <path> <text>
    pcs echo data.txt "The text that saved by pcs."

    注意：
      * 将使用当前的系统编码来保存内容到/data.txt文件。如果当前系统编码为UTF-8，
        则在非UTF-8编码的机器上使用cat显示时会出现中文乱码。
      * 安全起见，请在本地编辑好文件，使用upload上传。

### 直接显示网盘中文本文件内容
    pcs cat <path>
    pcs cat data.txt

    注意：
      * 将使用当前的系统编码来显示/data.txt文件内容。如果data.txt文件是以ASCII编码保存的，
        而当前系统为UTF8编码，则可能出现中文乱码。此时可使用download下载到本地后使用第三方工具查看其内容。

### 显示网盘文件或目录的元数据
    pcs meta <path>
    pcs meta /data.txt

### 列出网盘根目录下的文件或目录
    pcs list [path]

    输出格式为：
        * 第一列指示是否是目录，如果是目录则输出 _d _，否则输出 -
        * 第二列是文件或目录的最后修改时间
        * 第三列是文件路径

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

### 搜索文件
    pcs search [dir] <key>

### 上传文件
    pcs [-f] <local file> <remote path>
    pcs upload ~/data.tar.gz /backup/data.20140118.tar.gz

### 下载文件
    pcs [-f] <remote path> <local path>
    pcs download /backup/data.20140118.tar.gz ~/download/data.20140118.tar.gz

### 同步目录
    pcs synch <local path> <remote path>

### 查看帮助
    pcs --help

### 查看子命令帮助
    pcs <command> -h

### 比较异同
    pcs compare <local path> <remote path>

### 设置上下文
    pcs set ...
    pcs set secure_method=aes-cbc-128 secure_key=123456 secure_enable=true

### 显示当前上下文
    pcs context

    
