C/C++写的一个百度网盘工具，可以在linux终端中使用，目的是备份vps服务器数据。
这是通过分析网盘网站得到的直接接口，不需要创建应用。
如果有错误、建议、问题等，请发邮件gang.zhuo[AT]gmail.com。

编译安装：
===================================
### 1. 安装 libcurl
    apt-get install libcurl4-openssl-dev
### 2. 安装 libiconv
    wget http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
    tar zxvf libiconv-1.14.tar.gz
    cd libiconv-1.14
    ./configure --prefix=/usr/local
    make
    make install
### 3. 获取源代码
    git clone https://github.com/GangZhuo/baidupcs.git
### 4. 编译源代码
    cd baidupcs/pcs
    make
### 5. 安装到 /usr/bin 下 
    cp ./pcs /usr/bin/
    chmod a+x /usr/bin/pcs

使用方法：
===================================
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


