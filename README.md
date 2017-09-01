C/C++写的一个百度网盘工具，可以在linux终端中使用。
这是通过分析网盘网站得到的直接接口，不需要创建应用。
* 支持多线程下载和下载时断点续传。
* 支持快速上传和多线程分片上传。
* 支持线程限速。具体查看'pcs set'和'pcs context'命令的说明。
* 支持AES-CBC-128, AES-CBC-192, AES-CBC-256加密。

[![Build Status]][Appveyor]

查看 [初级使用教程]

打包安装 (Debian)：
===================================

### 1. 安装编译依赖
    apt update
    apt install build-essential dh-autoreconf dpkg-dev libssl-dev libcurl4-openssl-dev
### 2. 获取源代码
    git clone https://github.com/GangZhuo/BaiduPCS.git
### 3. 构建打包
    cd BaiduPCS
    dpkg-buildpackage -us -uc -i -b
### 4. 安装
    cd ..
    sudo apt install ./baidupcs_*.deb

编译 (Debian) (新方法)：
===================================
程序依赖于 libcurl。

### 1. 安装依赖
    apt-get install build-essential libcurl4-openssl-dev libssl-dev
### 2. 获取源代码
    git clone https://github.com/GangZhuo/BaiduPCS.git
### 3. 编译源代码
    cd BaiduPCS
    ./configure && make
    make install #将安装到/usr/local/bin下
### 4. 手动安装到其他目录，例如 /usr/bin 下 
    cp ./baidupcs /usr/bin/

编译 (Debian)：
===================================
程序依赖于 libcurl。

### 1. 安装依赖
    apt-get install build-essential libcurl4-openssl-dev libssl-dev
### 2. 获取源代码
    git clone https://github.com/GangZhuo/BaiduPCS.git
### 3. 编译源代码
    cd BaiduPCS
    make clean -f Makefile.old
    make -f Makefile.old
    make install -f Makefile.old #将安装到/usr/local/bin下
### 4. 手动安装到其他目录，例如 /usr/bin 下 
    cp ./bin/pcs /usr/bin/

编译 (Windows)：
===================================
### 1. 获取源代码
    git clone https://github.com/GangZhuo/BaiduPCS.git
### 2. 安装依赖
    到 https://github.com/GangZhuo/BaiduPCS/releases/tag/0.2.6 下载需要的依赖包 [deps-win32.zip] 。
    
    下载后，把压缩包解压到 deps 目录。
    
    最终目录结构为：

	somedirectory
	 |-BaiduPCS
	 |  |-deps
	 |  |  |-include
	 |  |  |  |-curl
	 |  |  |  |-openssl
	 |  |  |-lib
	 |  |  |  |-x64
	 |  |  |  |-x86
	 |  |-docs
	 |  |-openwrt
	 |  |-pcs
	 |  |-rbtree
	 |  |-(etc ...)

### 3. 编译源代码
    使用 Microsoft Visual Studio Community 2015 (Update 3) 打开源码根目录下的 baidupcs.sln，并执行编译操作，
	将在 Debug 或 Release 目录下产生 baidupcs.exe 文件。

#### 下载 [适用Windows的预编译版本] 或 下载 .net4.0 带窗体版本 [BaiduCloudDisk for .Net 4.0] 

编译 (Mac)：
===================================
### 1. 获取源代码
    git clone https://github.com/GangZhuo/BaiduPCS.git
### 2. 安装依赖
	brew install openssl
### 3. 编译源代码
    cd BaiduPCS
    export LDFLAGS=-L/usr/local/opt/openssl/lib
    export CPPFLAGS=-I/usr/local/opt/openssl/include
    autoreconf
    ./configure LD=/usr/bin/ld OPENSSL_LIBS='-lssl -lcrypto -lz'
    make clean
    make
    make install #将安装到/usr/local/bin下
### 4. 手动安装到其他目录，例如 /usr/bin 下
    cp ./baidupcs /usr/bin/

编译 (Openwrt)：
===================================
### 进入 [Openwrt SDK] 目录，然后执行如下命令：
        pushd package
        git clone https://github.com/GangZhuo/BaiduPCS.git
        popd
        make menuconfig # select Utilities/BaiduPCS
        make V=99 package/BaiduPCS/openwrt/compile

#### 或下载 [预编译 ipk for Openwrt] ，CPU: ar71xx, brcm71xx, brcm47xx, ramips\_24kec

命令列表：
===================================
    如果出现中文乱码，请检查操作系统的当前编码是否为UTF-8。
    命令中涉及到的网盘文件路径，可以是绝对路径，也可以是相对于当前工作目录的相对路径。
    例：
        /data/data.txt 即是网盘绝对路径，表示网盘根目录下的data目录中的data.txt文件。
        data.txt 即是相对路径，表示当前工作目录下的data.txt文件。
        当前工作目录可通过 'baidupcs pwd' 命令来查看，当前工作目录可通过 'baidupcs cd'命令切换。
        可通过 'baidupcs help' 查看支持的子命令；可在子命令后加 '-h' 查看子命令帮助，
        也可调用类似于 'baidupcs help <command>' 的命令来查看子命令的帮助。
        例如：
            baidupcs move -h。
            baidupcs help move 此命令等价于上一行的命令


### 直接显示网盘中文本文件内容
    baidupcs cat <file>
    
    示例：
      baidupcs cat note.txt
      baidupcs cat /my/todo.txt

### 切换工作目录
    baidupcs cd <dir>
    
    示例：
      baidupcs cd baidupcs
      baidupcs cd /backup/20140618

### 复制网盘文件或目录
    baidupcs copy <old path> <new path>
    
    示例：
      baidupcs copy data.txt data_20140118.txt

### 比较异同
    baidupcs compare [-cderu] <local path> <remote path>
    
    比较本地文件和远端文件、本地目录和远端目录的异同。
    默认选项是'-cdu'，即打印出需要上传的文件、
    需要下载的文件和无法确定上传下载的文件。
    
    比较规则：
      只通过时间来进行比较。
      I)  如果本地和远端都是文件：
            a) 本地最后修改时间大于网盘文件上传时间，则认为需要上传；
            b) 本地最后修改时间小于网盘文件上传时间，则认为需要下载；
               （此处注意：当一个文件上传到网盘后，其网盘时间肯定比本地最后修改时间大。
                 如果此时执行比较的话，则会认为该文件需要从网盘下载。
                 下载则没有此问题，因为一个文件下载后，程序会使用
                 网盘时间来更新本地文件的最后修改时间。）
      II)  本地存在，网盘不存在，则认为需要上传
      III) 本地不存在，网盘存在，则认为需要下载
      IV)  如果一端是文件，另一端是目录，则认为无法确认是上传还是下载。
    
    选项：
      -c  只打印出无法确定上传或下载的项。
          一般是因为本地是文件，远端是目录，或本地是目录远端是文件
      -d  只打印出需要下载的文件或目录
      -e  只打印相同的文件或目录
      -r  递归比较其子目录
      -u  只打印出需要上传的文件或目录
    
    示例：
       baidupcs compare music music
       baidupcs compare -r ~/music music
    

### 显示当前上下文
    baidupcs context
    
    上下文包括：
        当前使用的Cookie文件、验证码图片保存路径、
        当前的工作目录、列出目录时分页大小、列出目录时的排序字段、
        列出目录时的排序方向、启用的加密方法、加密密钥、是否启用加密
    
    程序开启时会自动读取上下文文件；结束时，会自动保存上下文到文件。
    可通过'PCS_CONTEXT'环境变量指定上下文文件路径。程序判断规则是：
       1) 如果通过'--context'选项指定了上下文文件，则使用它；
       2) 如果未通过'--context'指定，但是指定了环境变量'PCS_CONTEXT'则使用它；
       3) 如果也未指定环境变量'PCS_CONTEXT'则使用'~/.pcs/pcs.context'

    上下文为JSON格式的文件：
    
    {
	    "cookiefile":	        "/home/gang/.pcs/default.cookie", /*指定Cookie文件*/
	    "captchafile":	        "/home/gang/.pcs/captcha.gif",    /*指定验证码图片保存路径*/
	    "workdir":	            "/",                              /*指定当前的网盘工作目录*/
	    "list_page_size":	    20,                               /*指定列出目录时分页大小*/
	    "list_sort_name":	    "name",                           /*指定列出目录时排序字段*/
	    "list_sort_direction":	"asc",                            /*指定列出目录时的排序方向*/
	    "secure_method":	    "aes-cbc-128",                    /*指定上传时的加密方式*/
	    "secure_key":	        "12345678",                       /*指定上传时的加密密钥，*/
	                                                              /*下载时如检测到文件被加密，也使用此密钥解密*/
	    "secure_enable":	    true,                              /*指定是否启用加密解密*/
	                                                              /*如果设置为false，*/
	                                                              /*下载时即使检查到文件加密，也不会解密*/
	    "timeout_retry":	    true,                             /*当执行'synch'和'compare'命令时，
	                                                                因为频繁调用api去获取目录下文件名称
	                                                                将导致一些api调用超时，此选项用于控制超时时
	                                                                是否重试。*/
	    "max_thread":	        5,                                /*下载时允许的最大线程数*/
	    "max_speed_per_thread": 0                                 /*设置的是单个线程的最大下载速度。0表示不限速。
	                                                                单位为KiB。例，如果设置为100，共有5线程，
	                                                                则总的最大下载速度将在500KiB/s上下浮动。*/
	    "max_upload_speed_per_thread": 0,                         /*设置的是单个线程的最大上传速度。0表示不限速。
	                                                                单位为KiB。例，如果设置为100，共有5线程，
	                                                                则总的最大上传速度将在500KiB/s上下浮动。*/
	    "user_agent": "netdisk;5.2.7;PC;PC-Windows;6.2.9200;WindowsBaiduYunGuanJia", /*指定 User-Agent。*/
		"cache_size": 1024	                                      /* 设置磁盘缓存大小 */
    }
    

### 下载文件
    baidupcs download [-f] <remote file> <local file>
	baidupcs d [-f] <remote file> <local file>
    
    只能下载文件，如果需要下载目录，请使用 'baidupcs synch -d <local dir> <remote dir>'。
    
    选项：
      -f   如果本地文件存在的话，强制替换
    
    示例：
      baidupcs download /backup/data.20140118.tar.gz ~/download/data.20140118.tar.gz

### 直接保存文本到网盘中
    baidupcs echo [-a] <remote file> <text>
    
    选项：
      -a  指定把文本添加到文件末尾，而不是完全替换
      
    示例：
      baidupcs echo data.txt "The text that saved by baidupcs."

### 编码/解码本地文件
    baidupcs encode [-def] <src> <dst>
	
    选项：
      -d  使用上下文中的密钥来解密<src>文件并输出结果到<dst>中。
	      程序会读取<src>文件并检测其加密方法。
	  -e  使用上下文中的密钥和加密方法来加密<src>文件并输出结果到<dst>中。
	  -f  强制覆盖<dst>文件。
      
    示例：
      baidupcs encode -e data.txt data-sec.txt
	  baidupcs encode -d data-sec.txt data-plain.txt
	  baidupcs encode -h

### 修复文件（实验性功能）
    baidupcs fix [-fh] <md5> <length> <scrap> <remote path>
	  
	  <md5>          指定文件的 MD5 值
	  <length>       指定文件的字节大小
	  <scrap>        下载的前 256KiB 的文件碎片
	  <remote path>  修复成功后保存的网盘路径
    
    修复文件。从国外网站下载文件时，如果速度比较慢，而你又知道文件的 MD5 值和文件大小，
	那么你可以只下载前 256KiB 的内容，然后使用此命令来尝试修复文件。修复成功后，文件将
	保存在 <remote path> 指定的网盘目录中，因此你将可以从百度网盘以较快的速度下载。
	如果这个文件已经存在于百度网盘中的话（无论是否在自己的网盘中），修复的成功率应该是 100%。
    
    示例：
       baidupcs fix -h
       baidupcs fix 39d768542cd2420771f28b9a3652412f 5849513984 ~/xxx.iso xxx.iso

### 查看帮助
    baidupcs help [command name]
    
    查看帮助。
    
    示例：
       baidupcs help
       baidupcs help compare
       baidupcs help -h

### 列出网盘根目录下的文件或目录
    baidupcs list [dir]
    baidupcs ls [dir]
	
    输出格式为：
        * 第一列指示是否是目录，如果是目录则输出 d，否则输出 -
        * 第二列是文件或目录的最后修改时间
        * 第三列是文件路径
        * 如果指定了 --thumb 选项，则第四列是缩略图地址（如果有的话）

    示例：
       baidupcs list
       baidupcs list /music
       baidupcs list -h
       
    列出目录时会自动分页显示，如果需要修改分页大小的话，
    使用'baidupcs set --list_page_size=20'来修改，把list_page_size设置为0，则关闭分页。

### 登录网盘
    baidupcs login [--username=<username>] [--password=<password>]
    
    登录可能需要输入验证码。目前的处理办法是把验证码图片写入到本地文件，用户打开文件识别验证码。
    可通过 'baidupcs set --captcha_file=<path> 来指定验证码保存路径，
    例如：'baidupcs set --captcha_file=/var/www/xxx.com/captch.gif'将文件保存到网站目录下。
    可通过 'baidupcs context' 查看当前的执行上下文。执行上下文包括验证码图片保存路径等其他信息。
    
    示例：
      baidupcs login -h 可查看login命令的使用方法
      baidupcs login    会提示输入用户名和密码
      baidupcs login --username=gang 指定用户名登录
      baidupcs login --username=gang --password=123456 指定用户名和密码登录
      
### 退出网盘
    baidupcs logout

### 显示网盘文件或目录的元数据
    baidupcs meta <file>
    
    示例：
       baidupcs meta
       baidupcs meta note.txt

### 创建目录
    baidupcs mkdir <dir>
    
    示例：
      baidupcs mkdir subdir
      baidupcs mkdir /music/china

### 移动网盘文件或目录
    baidupcs move <src> <dst>
    baidupcs mv <src> <dst>
    
    示例：
      baidupcs move /data_20140118.txt /subdir/data.txt
      baidupcs move music /my/music

### 显示当前网盘的工作目录
    baidupcs pwd

    示例：
      baidupcs pwd

### 显示网盘配额
    baidupcs quota [-e]
    
    选项：
      -e   打印精确的网盘配额
      
    示例：
       baidupcs quota
       baidupcs quota -e

### 删除文件或目录
    baidupcs remove <path>
	baidupcs rm <path>
    
    示例：
      baidupcs remove /subdir/data_20140118.txt

### 重命名网盘文件或目录
    baidupcs rename <src> <new name>
	baidupcs ren <src> <new name>
    
    注意：<new name>是新的文件名字，而不是文件路径。如果需要移动文件到另一个目录，请使用 'pcs move'。
    
    示例：
      baidupcs rename /data.txt data_20140118.txt

### 设置上下文
    baidupcs set [--captcha_file=<path>] [--cookie_file=<path>] ...
    
    选项：
	--captcha_file=<file path>         设置验证码图片保存路径
	--cookie_file=<file path>          设置cookie文件路径
	--list_page_size=<page size>       设置列出目录时分页大小
	--list_sort_direction=[asc|desc]   设置列出目录时排序方向
	--list_sort_name=[name|time|size]  设置列出目录时排序字段
	--secure_enable=[true|false]       设置上传下载时是否启用加密
	--secure_key=<key string>          设置加密解密密钥
	--secure_method=[plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256] 设置加密方式
	--timeout_retry=[true|false]       设置执行synch和compare时，获取目录下文件超时时，是否允许重试
	--max_thread=<num>                 设置下载时允许的最大线程数
	--max_speed_per_thread=<num>       设置单线程的最大下载速度。单位为KiB。详细查看'pcs context'命令中对上下文文件的说明
	--max_upload_speed_per_thread=<num>设置单线程的最大上传速度。单位为KiB。详细查看'pcs context'命令中对上下文文件的说明
	--user_agent=<user-agent>          设置 User-Agent。
	--cache_size=<num>                 设置下载时磁盘缓存的大小。单位为 KiB。CTRL + C 中断下载时，磁盘缓存中的内容将丢失，需要再次下载。

    示例：
      baidupcs set -h
      baidupcs set --cookie_file="/tmp/pcs.cookie"
      baidupcs set --captcha_file="/tmp/vc.git"
      baidupcs set --list_page_size=20 --list_sort_name=name --list_sort_direction=desc
      baidupcs set --secure_enable=true --secure_key=123456 --secure_method=aes-cbc-256

### 搜索文件
    baidupcs search [-r] [dir] <key>
    
    示例：
       baidupcs search note.txt          在当前工作目录搜索 note.txt
       baidupcs search /music desc.mp3   在/music目录搜索 desc.mp3
       baidupcs search -r note.txt       在当前工作目录递归搜索 note.txt
       baidupcs search -r /music desc.mp3 在/music目录递归搜索 desc.mp3

### 同步目录
    baidupcs synch [-cdenru] <local path> <remote path>
	baidupcs s [-cdenru] <local path> <remote path>
    
    同步本地文件和远端文件、本地目录和远端目录。
    默认选项是'-cdu'，即上传需要上传的文件、下载需要下载的文件和打印无法确定上传下载的文件。
    
    比较规则：（同'compare'一样）
      只通过时间来进行比较。
      I)  如果本地和远端都是文件：
            a) 本地最后修改时间大于网盘文件上传时间，则认为需要上传；
            b) 本地最后修改时间小于网盘文件上传时间，则认为需要下载；
               （此处注意：当一个文件上传到网盘后，其网盘时间肯定比本地最后修改时间大。
                 如果此时执行比较的话，则会认为该文件需要从网盘下载。
                 下载则没有此问题，因为一个文件下载后，程序会使用
                 网盘时间来更新本地文件的最后修改时间。）
      II)  本地存在，网盘不存在，则认为需要上传
      III) 本地不存在，网盘存在，则认为需要下载
      IV)  如果一端是文件，另一端是目录，则认为无法确认是上传还是下载。
    
    选项：
      -c  只打印出无法确定上传或下载的项。
          一般是因为本地是文件，远端是目录，或本地是目录远端是文件
      -d  只下载需要下载的文件或目录
      -e  只打印相同的文件或目录
      -n  只是打印，而不真正的执行上传和下载，等价于'compare'
      -r  递归比较其子目录
      -u  只上传需要上传的文件或目录
    
    示例：
       baidupcs synch music music
       baidupcs synch -r ~/music music
       baidupcs synch -u music music     只上传需要上传的文件，等价于备份
       baidupcs synch -d music music     只下载需要下载的文件，等价于还原
       baidupcs synch -du music music    上传需要上传的文件，并且下载需要下载的文件，等价于同步
       
    注意：推荐每次都带上'-c'选项，可以打印出不知道如何处理的文件或目录，防止漏上传或下载。

### 上传文件
    baidupcs upload [-f] <local file> <remote file>
	baidupcs u [-f] <local file> <remote file>
    
    只能上传文件，如果需要上传目录，请使用 'pcs synch' 命令。
    
    示例：
       baidupcs upload ~/data.tar.gz /backup/data.20140118.tar.gz

### 显示程序版本
    baidupcs version
    
    示例：
       baidupcs version

### 显示当前登录用户
    baidupcs who

    示例：
       baidupcs who

注意：
===================================
* 1. 代码比较乱，以后空的时候重构。
* 2. 启用加密后，分片上传逻辑未仔细测试。

问题：
===================================
* 1. 上传大文件时，程序会计算文件的MD5值，此过程耗时较久。
* 2. 下载大文件时，程序会在磁盘上预分配文件空间，此过程耗时较久。
* 3. 上传大文件后，通过 'pcs meta'获取到的md5值与本地文件计算出的md5不匹配，这个应是百度网盘的bug。（测试时，把上传的文件下载到本地后，对比md5值是匹配的）

[Appveyor]: https://ci.appveyor.com/project/GangZhuo/baidupcs/branch/master
[Build Status]: https://ci.appveyor.com/api/projects/status/etib00a5ta70cpfr?svg=true
[baidupcs-dependencies.zip]: https://sourceforge.net/projects/baidupcs/files/Dependencies/
[适用Windows的预编译版本]:   https://github.com/GangZhuo/BaiduPCS/releases
[BaiduCloudDisk for .Net 4.0]: https://github.com/GangZhuo/BaiduPCS_NET/releases
[Openwrt SDK]:               http://wiki.openwrt.org/doc/howto/obtain.firmware.sdk
[预编译 ipk for Openwrt]:    http://sourceforge.net/projects/baidupcs/files/Openwrt/
[初级使用教程]:    https://github.com/GangZhuo/BaiduPCS/wiki/BaiduPCS-%E5%9F%BA%E6%9C%AC%E4%BD%BF%E7%94%A8
[windows.php.net]:    http://windows.php.net/downloads/php-sdk/deps/vc14/x86/
[deps-win32.zip]:   https://github.com/GangZhuo/BaiduPCS/releases/download/0.2.6/deps-win32.zip
