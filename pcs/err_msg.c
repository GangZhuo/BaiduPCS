
const char *get_login_errmsg(int error)
{
	const char *errmsg = 0;
	switch (error) {
	case -1:// "系统错误,请您稍后再试 ";
		errmsg = "System error, try again later";
		break;
	case 0:
		errmsg = "Success";
		break;
	case 1: // "您输入的帐号格式不正确";
		errmsg = "Incorrect username";
		break;
	case 2: // "您输入的帐号不存在";
		errmsg = "Username not exist";
		break;
	case 3: // "验证码不存在或已过期,请重新输入";
		errmsg = "Captcha expired";
		break;
	case 4: // "您输入的帐号或密码有误";
		errmsg = "Wrong password";
		break;
	case 5: // "请在弹出的窗口操作,或重新登录";
	case 120019: //请在弹出的窗口操作,或重新登录
	case 120021: //登录失败,请在弹出的窗口操作,或重新登录
	case 400031: //请在弹出的窗口操作,或重新登录
		errmsg = "Try again";
		break;
	case 6: // "您输入的验证码有误";
		errmsg = "Wrong captcha";
		break;
	case 7: // "密码错误，可以试试短信登录";
		errmsg = "Wrong password, try sms password";
		break;
	case 16: // "您的帐号因安全问题已被限制登录";
		errmsg = "Your account locked";
		break;
	case 17: //您的帐号已锁定,请解锁后登录
		errmsg = "Your account has been locked";
		break;
	case 18:
	case 120016:
	case 400032:
	case 400034:
	case 400037:
	case 400401:
		errmsg = "";// no message, see https://github.com/GangZhuo/BaiduPCS/issues/29
		break;
	case 257: // "请输入验证码";
		errmsg = "Wrong captcha";
		break;
	case 100005: //系统错误,请您稍后再试
		errmsg = "System error, please try again later";
		break;
	case 100023: //开启Cookie之后才能登录
		errmsg = "Cookie not open";
		break;
	case 100027: // "百度正在进行系统升级，暂时不能提供服务，敬请谅解";
		errmsg = "System upgrading..., unable to provide services, please forgive me";
		break;
	case 110024: //此帐号暂未激活,重发验证邮件
		errmsg = "The account is not activate, resend the activation email";
		break;
	case 200010: //验证码不存在或已过期
		errmsg = "Captcha not exist or expired";
		break;
	case 401007: // "您的手机号关联了其他帐号，请选择登录";
		errmsg = "Your phone number associated with more than one account, please login with the user name";
		break;
	case 500010: //登录过于频繁,请24小时后再试
		errmsg = "Login too frequently, please try again after 24 hours";
		break;
	default:
		errmsg = "Unknow";
		break;
	}
	return errmsg;
}

/* 从网盘JS获取，应该是网盘API的错误消息。然后通过 Google 翻译，翻译为英文。 */
const char *get_errmsg_by_errno(int error)
{
	const char *errmsg = 0;
	switch (error) {
	case 0:
		errmsg = "Success";// "成功";
		break;
	case 1:
		errmsg = "Server Error";// "服务器错误 ";
		break;
	case 2:
		errmsg = "The folder can not be moved";// "该文件夹不可以移动";
		break;
	case 3:
		errmsg = "One operation file can't over 100";// "一次操作文件不可超过100个";
		break;
	case 4:
		errmsg = "The new file name error";// "新文件名错误";
		break;
	case 5:
		errmsg = "Target directory illegal";// "目标目录非法";
		break;
	case 6:
		errmsg = "Alternate";// "备用";
		break;
	case 7:
		errmsg = "NS illegal or do not have access";// "NS非法或无权访问";
		break;
	case 8:
		errmsg = "ID illegally or do not have access";// "ID非法或无权访问";
		break;
	case 9:
		errmsg = "Application key failure";// "申请key失败";
		break;
	case 10:
		errmsg = "Failed to create a file of superfile";// "创建文件的superfile失败";
		break;
	case 11:
		errmsg = "user_id (or user_name) illegal or non-existent";// "user_id(或user_name)非法或不存在";
		break;
	case 12:
		errmsg = "Batch processing is not successful in all";// "批量处理未全部成功";
		break;
	case 13:
		errmsg = "This directory can not be shared";// "此目录无法共享";
		break;
	case 14:
		errmsg = "System error";// "系统错误";
		break;
	case 103:
		errmsg = "Extraction code error";// "提取码错误";
		break;
	case 104:
		errmsg = "Invalid authentication cookie";// "验证cookie无效";
		break;
	case 201:
		errmsg = "System error";// "系统错误";
		break;
	case 202:
		errmsg = "System error";// "系统错误";
		break;
	case 203:
		errmsg = "System error";// "系统错误";
		break;
	case 204:
		errmsg = "System error";// "系统错误";
		break;
	case 205:
		errmsg = "System error";// "系统错误";
		break;
	case 211:
		errmsg = "No Permissions or banned";// "无权操作或被封禁";
		break;
	case 301:
		errmsg = "Other request error";// "其他请求出错";
		break;
	case 404:
		errmsg = "Second pass md5 mismatch error code rapidupload";// "秒传md5不匹配 rapidupload 错误码";
		break;
	case 406:
		errmsg = "Sec transfer error code Failed to create file rapidupload";// "秒传创建文件失败 rapidupload 错误码";
		break;
	case 407:
		errmsg = "fileModify interface returns an error, requestid rapidupload does not return an error code";// "fileModify接口返回错误，未返回requestid rapidupload 错误码";
		break;
	case 501:
		errmsg = "LIST format acquired illegally";// "获取的LIST格式非法";
		break;
	case 600:
		errmsg = "json parse error";// "json解析出错";
		break;
	case 601:
		errmsg = "exception thrown";// "exception抛出异常";
		break;
	case 617:
		errmsg = "getFilelist other error";// "getFilelist其他错误";
		break;
	case 618:
		errmsg = "Request curl returns failure";// "请求curl返回失败";
		break;
	case 619:
		errmsg = "pcs returns an error code";// "pcs返回错误码";
		break;
	case 31021:
		errmsg = "Network connection fails, check the network or try again later.";// "网络连接失败，请检查网络或稍候再试";
		break;
	case 31075:
		errmsg = "One in support of Operation 999, minus point try";// "一次支持操作999个，减点试试吧";
		break;
	case 31080:
		errmsg = "Our server error, the wait to try it";// "我们的服务器出错了，稍候试试吧";
		break;
	case 31116:
		errmsg = "Your space inadequate yo, and quickly buy space bar";// "你的空间不足了哟，赶紧购买空间吧";
		break;
	case -1:
		errmsg = "User name and password authentication fails";// "用户名和密码验证失败";
		break;
	case -2:
		errmsg = "Alternate";// "备用";
		break;
	case -3:
		errmsg = "The user does not activate (call init interface)";// "用户未激活（调用init接口）";
		break;
	case -4:
		errmsg = "Can't find host_key and user_key (or BDUSS) from COOKIE";// "COOKIE中未找到host_key&user_key（或BDUSS）";
		break;
	case -5:
		errmsg = "host_key and user_key invalid";// "host_key和user_key无效";
		break;
	case -6:
		errmsg = "bduss invalid";// "bduss无效";
		break;
	case -7:
		errmsg = "File or directory name wrong or do not have access";// "文件或目录名错误或无权访问";
		break;
	case -8:
		errmsg = "This file already exists in this directory";// "该目录下已存在此文件";
		break;
	case -9:
		errmsg = "Owner of the file is deleted, the operation fails";// "文件被所有者删除，操作失败";
		break;
	case -10:
		errmsg = "Network disk space is full";// "网盘空间已满";
		break;
	case -11:
		errmsg = "Parent directory does not exist";// "父目录不存在";
		break;
	case -12:
		errmsg = "Device not registered";// "设备尚未注册";
		break;
	case -13:
		errmsg = "Equipment is already bound";// "设备已经被绑定";
		break;
	case -14:
		errmsg = "Account has been initialized";// "帐号已经初始化";
		break;
	case -21:
		errmsg = "Preset files can not be related operations";// "预置文件无法进行相关操作";
		break;
	case -22:
		errmsg = "The file can not be shared rename, move, and so";// "被分享的文件无法重命名，移动等操作";
		break;
	case -23:
		errmsg = "Database operation fails, contact netdisk administrator";// "数据库操作失败，请联系netdisk管理员";
		break;
	case -24:
		errmsg = "No cancellations public file containing a list of files you want to cancel.";// "要取消的文件列表中含有不允许取消public的文件。";
		break;
	case -25:
		errmsg = "Non-beta user";// "非公测用户";
		break;
	case -26:
		errmsg = "Failure invitation code";// "邀请码失效";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是共享文件的错误消息。然后通过 Google 翻译，翻译为英文。*/
const char *get_share_errmsg_by_errno(int error)
{
	const char *errmsg = 0;
	switch (error) {
	case 0:
		errmsg = "Success";//"成功";
		break;
	case 2:
		errmsg = "Parameter error";// "参数错误";
		break;
	case 3:
		errmsg = "Not logged or invalid account number";// "未登录或帐号无效";
		break;
	case 4:
		errmsg = "Storage like a problem, please try again later";// "存储好像出问题了，请稍候再试";
		break;
	case 108:
		errmsg = "Filename sensitive word, optimization about it";// "文件名有敏感词，优化一下吧";
		break;
	case 110:
		errmsg = "Share the number exceeds the limit, you can go to \"my share\" in the view shared file links";// "分享次数超出限制，可以到“我的分享”中查看已分享的文件链接";
		break;
	case 114:
		errmsg = "The current mandate does not exist, save fails";// "当前任务不存在，保存失败";
		break;
	case 115:
		errmsg = "The document prohibits Share";// "该文件禁止分享";
		break;
	case -1:
		errmsg = "Because you share a file in violation of relevant laws and regulations, sharing has been disabled before the share out files are not affected.";// "由于您分享了违反相关法律法规的文件，分享功能已被禁用，之前分享出去的文件不受影响。";
		break;
	case -2:
		errmsg = "User does not exist, please refresh the page after retry";// "用户不存在,请刷新页面后重试";
		break;
	case -3:
		errmsg = "File does not exist, please refresh the page and try again";// "文件不存在,请刷新页面后重试";
		break;
	case -4:
		errmsg = "Login information is incorrect, please try to log in again";// "登录信息有误，请重新登录试试";
		break;
	case -5:
		errmsg = "host_key and user_key invalid";// "host_key和user_key无效";
		break;
	case -6:
		errmsg = "Please sign in again";// "请重新登录";
		break;
	case -7:
		errmsg = "The share has been deleted or canceled";// "该分享已删除或已取消";
		break;
	case -8:
		errmsg = "The share has been deleted or canceled";//"该分享已经过期";
		break;
	case -9:
		errmsg = "Access password error";// "访问密码错误";
		break;
	case -10:
		errmsg = "Share the chain has reached the maximum upper limit 100 000, can not share again";// "分享外链已经达到最大上限100000条，不能再次分享";
		break;
	case -11:
		errmsg = "Invalid authentication cookie";// "验证cookie无效";
		break;
	case -14:
		errmsg = "Sorry, SMS share 20 daily limit, you have to share finished today, please come back tomorrow to share it!";// "对不起，短信分享每天限制20条，你今天已经分享完，请明天再来分享吧！";
		break;
	case -15:
		errmsg = "Sorry, the message share limit 20 per day, you have to share finished today, please come back tomorrow to share it!";// "对不起，邮件分享每天限制20封，你今天已经分享完，请明天再来分享吧！";
		break;
	case -16:
		errmsg = "Sorry, the document has restricted share!";// "对不起，该文件已经限制分享！";
		break;
	case -17:
		errmsg = "File sharing over the limit";// "文件分享超过限制";
		break;
	case -30:
		errmsg = "File already exists";// "文件已存在";
		break;
	case -31:
		errmsg = "File Save Failed";// "文件保存失败";
		break;
	case -33:
		errmsg = "Once supported operating 10,000, minus point try";// "一次支持操作10000个，减点试试吧";
		break;
	case -32:
		errmsg = "Your space inadequate yo, and quickly buy space bar";// "你的空间不足了哟，赶紧购买空间吧";
		break;
	case -70:
		errmsg = "You share the file contains a virus or virus-like, to you and to others data security, another file-sharing it";// "你分享的文件中包含病毒或疑似病毒，为了你和他人的数据安全，换个文件分享吧";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是离线下载的错误消息。然后通过 Google 翻译，翻译为英文。*/
const char *get_download_errmsg_by_errno(int error)
{
	const char *errmsg = 0;
	switch (error) {
	case 36000:
		errmsg = "Network busy, please try again later";// "网络繁忙，请稍候再试";
		break;
	case 36001:
		errmsg = "Parameter error";// "参数错误";
		break;
	case 36002:
		errmsg = "appid error";// "appid错误";
		break;
	case 36003:
		errmsg = "Please try refresh";// "请刷新再试";
		break;
	case 36004:
		errmsg = "Please sign in again";// "请重新登录";
		break;
	case 36005:
		errmsg = "User is not logged";// "用户未登录";
		break;
	case 36006:
		errmsg = "The user does not activate";// "用户未激活";
		break;
	case 36007:
		errmsg = "User is not authorized";// "用户未授权";
		break;
	case 36008:
		errmsg = "User does not exist";// "用户不存在";
		break;
	case 36009:
		errmsg = "User space shortage";// "用户空间不足";
		break;
	case 36010:
		errmsg = "File does not exist";// "文件不存在";
		break;
	case 36012:
		errmsg = "The operation timed out, please try again";// "操作超时，请重试";
		break;
	case 36013:
		errmsg = "At the same task much download can not be downloaded";// "同时下载的任务过多，不能下载";
		break;
	case 36014:
		errmsg = "Storage path has been used";// "存储路径已使用";
		break;
	case 36016:
		errmsg = "Task has been deleted";// "任务已删除";
		break;
	case 36017:
		errmsg = "Task completed";// "任务已完成";
		break;
	case 36018:
		errmsg = "Resolution fails, the seed file is corrupted";// "解析失败，种子文件损坏";
		break;
	case 36019:
		errmsg = "The task is being processed";// "任务正在处理中";
		break;
	case 36020:
		errmsg = "Task address does not exist";// "任务地址不存在";
		break;
	case 36021:
		errmsg = "Max ordinary users download a task Oh! Immediately turn off download packages Download More!";// "普通用户最多同时下载1个任务哦！马上开通离线下载套餐，立即下载更多！";
		break;
	case 36022:
		errmsg = "At the same task much download can not be downloaded";// "同时下载的任务过多，不能下载";
		break;
	case 36023:
		errmsg = "Ordinary users can only download offline 5 monthly task Oh! Immediately turn off download packages Download More!";// "普通用户每月只能离线下载5个任务哦！马上开通离线下载套餐，立即下载更多！";
		break;
	case 36024:
		errmsg = "This month downloads exceeded limit";// "本月下载数已超限制";
		break;
	case 36025:
		errmsg = "Share link has expired";// "分享链接已失效";
		break;
	case 36026:
		errmsg = "Link malformed";// "链接格式有误";
		break;
	case 36027:
		errmsg = "Link malformed";// "链接格式有误";
		break;
	case 36028:
		errmsg = "Temporarily unable to find the relevant seed information";// "暂时无法找到相关种子信息";
		break;
	case 36031:
		errmsg = "Network busy, please try again later";// "网络繁忙，请稍候再试";
		break;
	case -19:
		errmsg = "Please enter the verification code";// "请输入验证码";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是购买套餐的错误消息。然后通过 Google 翻译，翻译为英文。*/
const char *get_buy_errmsg_by_errno(int error)
{
	const char *errmsg = 0;
	switch (error) {
	case 1000:
		errmsg = "Uh oh, you can not upgrade from Advanced Package to lower packages, please re-purchase it.";// "啊哦，不能从高级套餐升级到低级套餐，请重新购买吧。";
		break;
	case 1001:
		errmsg = "Uh oh, you can not upgrade from Advanced Package to lower packages, please re-purchase it.";// "啊哦，不能从高级套餐升级到低级套餐，请重新购买吧。";
		break;
	case 1002:
		errmsg = "Lol, create orders failed, please try again later about it.";// "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1003:
		errmsg = "Lol, create orders failed, please try again later about it.";// "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1004:
		errmsg = "Lol, create orders failed, please try again later about it.";// "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1005:
		errmsg = "Lol, create orders failed, please try again later about it.";// "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1006:
		errmsg = "Lol, can not buy the same product on the same day, please re-purchase it.";// "啊哦，不能同一天购买相同产品，请重新购买吧。";
		break;
	case 1007:
		errmsg = "Lol, can not buy the same product on the same day, please re-purchase it.";// "啊哦，不能同一天购买相同产品，请重新购买吧。";
		break;
	case 3002:
		errmsg = "Lol, the order has been successful payment, please re-purchase it.";// "啊哦，该订单已经支付成功，请重新购买吧。";
		break;
	case 3003:
		errmsg = "Lol, the order has been paid fail, please re-purchase it.";// "啊哦，该订单已经支付失败，请重新购买吧。";
		break;
	case 36000:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36001:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36002:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36003:
		errmsg = "The visit anomaly, an authority restricted";// "该次访问异常，权限受限";
		break;
	case 36005:
		errmsg = "Verification code input errors, please refresh retry";// "验证码输入错误，请刷新重试";
		break;
	case 36006:
		errmsg = "Uh oh, we did not launch this product ah";// "啊哦，我们还没推出这款产品呀";
		break;
	case 36007:
		errmsg = "As an exception, a cup of tea next to retry?";// "作异常，喝杯茶重试下？";
		break;
	case 36008:
		errmsg = "Oh, abnormal your operation, please refresh and try again.";// "哎呀，你的操作异常，请刷新重试。";
		break;
	case 36009:
		errmsg = "Foundation course allowed to buy";// "基础套餐不允许购买";
		break;
	case 36010:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36011:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36012:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36013:
		errmsg = "The order can not be re-paid";// "该订单无法重新支付";
		break;
	case 36014:
		errmsg = "Lol, the order has been successful payment, please re-purchase it.";// "啊哦，该订单已经支付成功，请重新购买吧。";
		break;
	case 36015:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36016:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36017:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36018:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36019:
		errmsg = "Lol, server cold, urgent medical treatment in ......";// "啊哦，服务器感冒了，紧急医治中……";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是购买套餐的错误消息。然后通过 Google 翻译，翻译为英文。*/
const char *get_record_errmsg_by_errno(int error)
{
	const char *errmsg = 0;
	switch (error) {
	case 36000:
		errmsg = "Internal error";// "内部错误";
		break;
	case 36001:
		errmsg = "Unsupported API";// "不支持的API";
		break;
	case 36002:
		errmsg = "Parameter error";// "参数错误";
		break;
	case 36003:
		errmsg = "No access";// "无权限访问";
		break;
	case 36005:
		errmsg = "Code invalid or illegal";// "验证码失效或非法";
		break;
	case 36006:
		errmsg = "To purchase the product does not exist";// "要购买的产品不存在";
		break;
	case 36007:
		errmsg = "Users operate in parallel in the process of buying the product, but could not get the error code";// "用户在购买产品的过程中并行操作，一般不会出现该错误码";
		break;
	case 36016:
		errmsg = "This function is not property values and can not get access systems concern";// "该功能不是属性值，不能获取，接入系统关注";
		break;
	case 36017:
		errmsg = "This feature is not a consumer value, not consumption, access systems concern";// "该功能不是消费值，不能消费，接入系统关注";
		break;
	case 36018:
		errmsg = "Function items to be consumed is not found, access systems concern";// "要消费的功能项未找到，接入系统关注";
		break;
	case 36019:
		errmsg = "Refusal to consume, the user does not have quotas, access systems concern";// "拒绝消费，用户没有配额了，接入系统关注";
		break;
	case 36020:
		errmsg = "Request replay";// "请求重放";
		break;
	case 36021:
		errmsg = "Request expired, or a third party request forgery";// "请求过期，或者是第三方伪造的请求";
		break;
	case 36031:
		errmsg = "Third party api parameter error";// "第三方api参数错误";
		break;
	case 36032:
		errmsg = "Third party api signature error";// "第三方api签名错误";
		break;
	case 36033:
		errmsg = "Third party api file error";// "第三方api文件错误";
		break;
	case 36034:
		errmsg = "Database Error";// "数据库错误";
		break;
	case 36035:
		errmsg = "Orders already exist";// "订单已经存在";
		break;
	case 36036:
		errmsg = "Order token failure";// "订单token失效";
		break;
	case 36037:
		errmsg = "Check Order does not exist";// "查询订单不存在";
		break;
	case 36038:
		errmsg = "Provinces parameter error";// "省市参数错误";
		break;
	case 36039:
		errmsg = "Price does not exist";// "单价不存在";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}
