baidupcs
========
百度网盘客户端，可以在终端中执行。 <br />
依赖项：libcurl, libiconv, libpcre <br />
<br />
安装： <br />
wget https://github.com/GangZhuo/baidupcs/blob/master/bin/pcs-v0.0.1 <br />
cp ./pcs-v0.0.1 /usr/bin/pcs <br />
chmod a+x /usr/bin/pcs <br />
<br />
使用：<br />
1. 初次使用：·pcs -u<username> quota --urlc·登录网盘 <br />
此步骤可能需要输入验证码，指定了--urlc选项，程序会把验证码图片上传到http://urlc.cn,在输出的html中找到验证码地址，在浏览器中打开来浏览。 <br />
2. 登录后， `pcs list /` 列出网盘根目录 <br />
3. `pcs --help` 查看帮助 <br />
<br />
编译安装： <br />
1. 安装好 libcurl, libiconv, libpcre 库。安装方法自己GOOGLE <br />
2. git clone https://github.com/GangZhuo/baidupcs.git <br />
3. cd baidupcs/pcs <br />
4. make <br />
5. cp ./pcs /usr/bin/ <br />
6. chmod a+x /usr/bin/pcs <br />
