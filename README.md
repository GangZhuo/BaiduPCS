C/C++写的一个百度网盘工具，可以在linux终端中使用，目的是备份vps服务器数据。<br />
<br />
编译安装：
===================================
### 安装 libcurl
    apt-get install libcurl4-openssl-dev
### 安装 libiconv
    wget http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
    tar zxvf libiconv-1.14.tar.gz
    cd libiconv-1.14
    ./configure --prefix=/usr/local
    make
    make install
### 安装 libpcre
    apt-get install libpcre3 libpcre3-dev
### 获取源代码
    git clone https://github.com/GangZhuo/baidupcs.git
### 编译源代码
    cd baidupcs/pcs
    make
### 安装到 /usr/bin 下 
    cp ./pcs /usr/bin/
    chmod a+x /usr/bin/pcs

使用方法：
===================================
  * 登录网盘：执行任意命令，如果未登录会提示登录。登录时，可能需要输入验证码，提供两种方式可以查看验证码：<br />
      * 保存验证码图片到~/.baidupcs/verify_code.gif, 自己scp到本地查看。此为默认方式。
      * 上传验证码图片到[http://urlc.cn http://urlc.cn]，自己找到并打开返回的网址查看。使用--urlc选项可开启此模式。
      * 另一种选择是：在本地运行pcs并登录后，然后把本地~/.baidupcs/default.cookie文件复制到服务器~/.baidupcs/default.cookie，服务器中pcs将自动使用该cookie。
    {{{
    pcs -u<username> --urlc quota
    }}}
  * 显示网盘配额
    {{{
    pcs quota
    }}}
  * 直接保存文本到网盘中
    {{{
    pcs echo <path> <text>
    pcs echo /data.txt "The text that saved by pcs."
    }}}
  * 直接显示网盘中文本文件内容
    {{{
    pcs cat <path>
    pcs cat /data.txt
    }}}
  * 把文本添加到网盘文件的末尾
    {{{
    pcs -a echo <path> <text>
    pcs -a echo /data.txt "The text that will append to the end of the file."
    }}}
  * 显示网盘文件或目录的元数据
    {{{
    pcs meta <path>
    pcs meta /data.txt
    }}}
  * 列出网盘根目录下的文件或目录，且按最后修改日期倒序排序
    {{{
    pcs [options] list <path>
    pcs --sort=time -i list /
    }}}
    输出格式为：
        * 第一列指示是否是目录，如果是目录则输出 _d _，否则输出 -
        * 第二列是文件或目录的最后修改时间
        * 第三列是文件的完整路径
  * 重命名网盘文件或目录
    {{{
    pcs rename <path> <new name>
    pcs rename /data.txt data_20140118.txt
    }}}
  * 创建目录
    {{{
    pcs mkdir <path>
    pcs mkdir /subdir
    }}}
  * 移动网盘文件或目录
    {{{
    pcs move <path> <new path>
    pcs move /data_20140118.txt /subdir/data.txt
    }}}
  * 复制网盘文件或目录
    {{{
    pcs copy <path> <new path>
    pcs copy /subdir/data.txt /subdir/data_20140118.txt
    }}}
  * 删除文件或目录
    {{{
    pcs remove <path>
    pcs remove /subdir/data_20140118.txt
    }}}
  * 递归搜索文件或目录
    {{{
    pcs [-r] search <dir path> <key>
    pcs -r search / data
    }}}
  * 备份目录
    {{{
    pcs [-rf] [--synch] <local path> <remote path>
    pcs mkdir /backup
    pcs -r upload /var/wwwroot/data /backup/data_20140118
    }}}
  * 备份文件
    {{{
    pcs [-f] <local file> <remote path>
    pcs mkdir /backup
    pcs upload ~/data.tar.gz /backup/data.20140118.tar.gz
    }}}
  * 下载目录
    {{{
    pcs [-rf] [--synch] <remote path> <local path>
    pcs -r download /backup/data_20140118 ~/download/data_20140118
    }}}
  * 下载文件
    {{{
    pcs [-f] <remote path> <local path>
    pcs download /backup/data.20140118.tar.gz ~/download/data.20140118.tar.gz
    }}}
  * 查看帮助
    {{{
    pcs --help
    }}}
<br />
==帮助==
{{{
Usage: pcs [OPTION...] <command> [<args>]

Provide some commands that can access the baidu network disk.

Fields: 
  name                    Sort by name
  time                    Sort by last modify time
  size                    Sort by size

Commands：
  quota                    Show the quota
  meta                     Show the meta data
  list                     List files
  rename                   Rename the file
  move                     Move the file
  copy                     Copy the file
  mkdir                    Create a new directory
  remove                   Remove the file
  cat                      Show the content directly
  echo                     Save the text into baidu network disk directly
  search                   Search the baidu network disk
  download                 Download the file
  upload                   Upload the file

 Options:
  -a, --append               Append the text at the end of the file instead of
                             override the file when execute echo command.
  -f, --force                Force delete the files that not exists in source
                             when download or upload.
  -i, --inverte              Inverte the result when list files, if have no -s
                             the -i option will be ignored.
  -p, --password[=<password>]   Specify the password.
  -r, --recursion            Recursive execute the command.
  -s, --sort[=<field>]       Sort the result when list files. 
  -u, --username[=<username>]   Specify the account.

 More Options:
      --cookie[=<cookiefile>]   Specify the cookie file.
      --synch                Synch the tree. Delete the files or directories
                             that not exists in the local file system when
                             upload, Delete the files or directories that not
                             exists in the net disk when download.
  -U, --urlc                 Use http://urlc.cn to hold the verify code image.
                             The default behavior write the image into
                             ~/.baidupcs/ .
  -v, --verbose              Show the response text.

  -?, --help                 give this help list
      --usage                give a short usage message
  -V, --version              print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.


If you are first use the program, or you session is time out, you should be use
`pcs -u<username> quota` to login the disk. May be need input the verify code
when login, there are two ways to view the image. One is save the image into
~/.baidupcs/verfy_code.gif, this is default behavior. Another is upload image
to http://urlc.cn , you can use -U option to use this behavior. After upload,
you can find the url from the response html, and then you can view the image by
browser (i.e.g Firefox, Chrome, IE..), 

Simples:
  Get the quota:
      pcs -u<username> quota

  List the files or directories that is in / folder:
      pcs list /

  Rename the /server_file.txt to new_server_file.txt:
      pcs rename /server_file.txt new_server_file.txt
  The second argument have no slash, if you want move the file into other
folder, use command move.

  Move the /server_file.txt into /sub_folder/new_name.txt:
      pcs move /server_file.txt /sub_folder/new_name.txt

  Create a new folder:
      pcs mkdir /new_dir

  Remove the file:
      pcs remove /server_file1.txt

  Download file:
      pcs download /server_file.txt /home/user/download/local_file.txt

  Upload file:
      pcs upload /home/user/documents/local_file.txt /server_file.txt

  Show text file content:
      pcs cat /server_file.txt

  Append the text at end of the file:
      pcs echo -a /server_file.txt "The text append by echo command.\n"

  Search file in /temp folder:
      pcs search /temp file.txt

  Copy /temp/file.txt to /temp/2/new_file.txt:
      pcs copy /temp/file.txt /temp/2/new_file.txt


}}}
<br />