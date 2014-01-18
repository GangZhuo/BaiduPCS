baidupcs
========
百度网盘工具，可以在终端中执行。 <br />
使用c语言实现，直接接口，不是REST API。 <br />
依赖项：libcurl, libiconv, libpcre <br />
<br />
使用：<br />
1. 初次使用：·pcs -u<username> quota --urlc·登录网盘 <br />
此步骤可能需要输入验证码，指定了--urlc选项，程序会把验证码图片上传到http://urlc.cn,在输出的html中找到验证码地址，在浏览器中打开来浏览。 <br />
2. 登录后， `pcs list /` 列出网盘根目录 <br />
3. `pcs --help` 查看帮助 <br />
<br />
编译安装： <br />
1. 安装好 libcurl, libiconv, libpcre 库。安装方法自己GOOGLE <br />
  apt-get install libcurl3 libcurl4-openssl-dev <br />
  wget http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz <br />
  tar zxvf libiconv-1.14.tar.gz <br />
  cd libiconv-1.14 <br />
  ./configure --prefix=/usr/local <br />
  make <br />
  make install <br />
  apt-get install libpcre3 libpcre3-dev  <br />
2. git clone https://github.com/GangZhuo/baidupcs.git <br />
3. cd baidupcs/pcs <br />
4. make <br />
5. cp ./pcs /usr/bin/ <br />
6. chmod a+x /usr/bin/pcs <br />
<br />
Help: <br />
Usage: pcs [OPTION...] <command> [<args>] <br />
 <br />
Provide some commands that can access the baidu network disk. <br />
 <br />
Fields:  <br />
  name                    Sort by name <br />
  time                    Sort by last modify time <br />
  size                    Sort by size <br />
 <br />
Commands： <br />
  quota                    Show the quota <br />
  meta                     Show the meta data <br />
  list                     List files <br />
  rename                   Rename the file <br />
  move                     Move the file <br />
  copy                     Copy the file <br />
  mkdir                    Create a new directory <br />
  remove                   Remove the file <br />
  cat                      Show the content directly <br />
  echo                     Save the text into baidu network disk directly <br />
  search                   Search the baidu network disk <br />
  download                 Download the file <br />
  upload                   Upload the file <br />
 <br />
 Options: <br />
  -a, --append               Append the text at the end of the file instead of <br />
                             override the file when execute echo command. <br />
  -f, --force                Force delete the files that not exists in source <br />
                             when download or upload. <br />
  -i, --inverte              Inverte the result when list files, if have no -s <br />
                             the -i option will be ignored. <br />
  -p, --password[=<password>]   Specify the password. <br />
  -r, --recursion            Recursive execute the command. <br />
  -s, --sort[=<field>]       Sort the result when list files.  <br />
  -u, --username[=<username>]   Specify the account. <br />
 <br />
 More Options: <br />
      --cookie[=<cookiefile>]   Specify the cookie file. <br />
      --synch                Synch the tree. Delete the files or directories <br />
                             that not exists in the local file system when <br />
                             upload, Delete the files or directories that not <br />
                             exists in the net disk when download. <br />
  -U, --urlc                 Use http://urlc.cn to hold the verify code image. <br />
                             The default behavior write the image into <br />
                             ~/.baidupcs/ . <br />
  -v, --verbose              Show the response text. <br />
 <br />
  -?, --help                 give this help list <br />
      --usage                give a short usage message <br />
  -V, --version              print program version <br />
 <br />
Mandatory or optional arguments to long options are also mandatory or optional <br />
for any corresponding short options. <br />


If you are first use the program, or you session is time out, you should be use <br />
`pcs -u<username> quota` to login the disk. May be need input the verify code <br />
when login, there are two ways to view the image. One is save the image into <br />
~/.baidupcs/verfy_code.gif, this is default behavior. Another is upload image <br />
to http://urlc.cn , you can use -U option to use this behavior. After upload, <br />
you can find the url from the response html, and then you can view the image by <br />
browser (i.e.g Firefox, Chrome, IE..),  <br />
 <br />
Simples: <br />
  Get the quota: <br />
      pcs -u<username> quota <br />
 <br />
  List the files or directories that is in / folder: <br />
      pcs list / <br />
 <br />
  Rename the /server_file.txt to new_server_file.txt: <br />
      pcs rename /server_file.txt new_server_file.txt <br />
  The second argument have no slash, if you want move the file into other <br />
folder, use command move. <br />
 <br />
  Move the /server_file.txt into /sub_folder/new_name.txt: <br />
      pcs move /server_file.txt /sub_folder/new_name.txt <br />
 <br />
  Create a new folder: <br />
      pcs mkdir /new_dir <br />
 <br />
  Remove the file: <br />
      pcs remove /server_file1.txt <br />
 <br />
  Download file: <br />
      pcs download /server_file.txt /home/user/download/local_file.txt <br />
 <br />
  Upload file: <br />
      pcs upload /home/user/documents/local_file.txt /server_file.txt <br />
 <br />
  Show text file content: <br />
      pcs cat /server_file.txt <br />
 <br />
  Append the text at end of the file: <br />
      pcs echo -a /server_file.txt "The text append by echo command.\n" <br />
 <br />
  Search file in /temp folder: <br />
      pcs search /temp file.txt <br />
 <br />
  Copy /temp/file.txt to /temp/2/new_file.txt: <br />
      pcs copy /temp/file.txt /temp/2/new_file.txt <br />
 <br />
 <br />