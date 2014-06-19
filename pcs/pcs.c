#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef WIN32
# include <malloc.h>
#include "openssl_aes.h"
#include "openssl_md5.h"
#else
# include <alloca.h>
#include <openssl/aes.h>
#include <openssl/md5.h>
#endif

#include "pcs_defs.h"
#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_http.h"
#include "cJSON.h"
#include "pcs.h"

#define PCS_MD5_SIZE	16 /*MD5的长度，固定为16。修改为其他值将导致校验错误*/

#define PCS_SKIP_SPACE(p) while((*p) && (*p == ' ' || *p == '\f' || *p == '\n' || *p == '\r' || *p == '\t' || *p == '\v')) p++

#define PCS_IS_TOKEN_CHAR(ch) (((ch) >= '0' && (ch) <= '9')\
								   || ((ch) >= 'a' && (ch) <= 'z')\
								   || ((ch) >= 'A' && (ch) <= 'Z')\
								   || (ch) == '_'\
								   || (ch) == '-')

#define URL_HOME			"http://www.baidu.com"
#define URL_DISK_HOME		"http://pan.baidu.com/disk/home"
#define URL_PASSPORT_API	"https://passport.baidu.com/v2/api/?"
#define URL_PASSPORT_LOGOUT	"https://passport.baidu.com/?logout&u=http://pan.baidu.com"
#define URL_CAPTCHA			"https://passport.baidu.com/cgi-bin/genimage?"
#define URL_PAN_API			"http://pan.baidu.com/api/"
#define URL_PCS_REST		"http://c.pcs.baidu.com/rest/2.0/pcs/file"

struct pcs {
	char		*username;
	char		*password;
	char		*bdstoken;
	char		*bduss;
	char		*sysUID;
	char		*errmsg;

	PcsGetCaptchaFunction		captcha_func;
	void						*captcha_data;

	PcsHttp		http;

	int			secure_method;
	char		*secure_key;
	PcsBool		secure_enable;

	PcsHttpWriteFunction	download_func;
	void					*download_data;

	char		*buffer;
	size_t		buffer_size;
};

#define PCS_BUFFER_SIZE			(AES_BLOCK_SIZE * 1024)
#define PCS_ACTION_NONE			0
#define PCS_ACTION_DOWNLOAD		1
#define PCS_ACTION_UPLOAD		2

struct PcsDownloadState
{
	Pcs					handle;
	size_t				contentlength;
	unsigned char		buffer[PCS_BUFFER_SIZE];
	int					buffer_size;
	int					secure;

	void				*userdata;

	PcsHttpWriteFunction write;
	void				 *write_state;

	PcsBool (*finish)(void *finish_state);
	void				*finish_state;

	void (*destroy)(void *destroy_state);
	void				*destroy_state;
};

struct PcsAesHead
{
	int					magic; /*Should be AES*/
	int					bits;
	int					polish;
};

struct PcsAesState
{
	struct PcsAesHead	head;
	int					mod;

	AES_KEY				aes;
	unsigned char		key[AES_BLOCK_SIZE];
	unsigned char		iv[AES_BLOCK_SIZE];
	unsigned char		buffer[PCS_BUFFER_SIZE];
	int					buffer_size;
	unsigned char		last_block[AES_BLOCK_SIZE]; /*128bits block*/
	int					last_block_size;


	MD5_CTX				md5; /*用于文件校验*/
};

struct PcsUploadState
{
	Pcs					handle;
	size_t				contentlength;
	FILE				*file;
	unsigned char		buffer[PCS_BUFFER_SIZE];
	size_t				buffer_size;
	int					secure;

	int					eof;

	struct PcsAesState	*aes;
};

/*从网盘JS获取，应该是网盘API的错误消息*/
const char *get_errmsg_by_errno(int error)
{
	const char *errmsg = NULL;
	switch (error) {
	case 0:
		errmsg = "成功";
		break;
	case 1:
		errmsg = "服务器错误 ";
		break;
	case 2:
		errmsg = "该文件夹不可以移动";
		break;
	case 3:
		errmsg = "一次操作文件不可超过100个";
		break;
	case 4:
		errmsg = "新文件名错误";
		break;
	case 5:
		errmsg = "目标目录非法";
		break;
	case 6:
		errmsg = "备用";
		break;
	case 7:
		errmsg = "NS非法或无权访问";
		break;
	case 8:
		errmsg = "ID非法或无权访问";
		break;
	case 9:
		errmsg = "申请key失败";
		break;
	case 10:
		errmsg = "创建文件的superfile失败";
		break;
	case 11:
		errmsg = "user_id(或user_name)非法或不存在";
		break;
	case 12:
		errmsg = "批量处理未全部成功";
		break;
	case 13:
		errmsg = "此目录无法共享";
		break;
	case 14:
		errmsg = "系统错误";
		break;
	case 103:
		errmsg = "提取码错误";
		break;
	case 104:
		errmsg = "验证cookie无效";
		break;
	case 201:
		errmsg = "系统错误";
		break;
	case 202:
		errmsg = "系统错误";
		break;
	case 203:
		errmsg = "系统错误";
		break;
	case 204:
		errmsg = "系统错误";
		break;
	case 205:
		errmsg = "系统错误";
		break;
	case 211:
		errmsg = "无权操作或被封禁";
		break;
	case 301:
		errmsg = "其他请求出错";
		break;
	case 404:
		errmsg = "秒传md5不匹配 rapidupload 错误码";
		break;
	case 406:
		errmsg = "秒传创建文件失败 rapidupload 错误码";
		break;
	case 407:
		errmsg = "fileModify接口返回错误，未返回requestid rapidupload 错误码";
		break;
	case 501:
		errmsg = "获取的LIST格式非法";
		break;
	case 600:
		errmsg = "json解析出错";
		break;
	case 601:
		errmsg = "exception抛出异常";
		break;
	case 617:
		errmsg = "getFilelist其他错误";
		break;
	case 618:
		errmsg = "请求curl返回失败";
		break;
	case 619:
		errmsg = "pcs返回错误码";
		break;
	case 31021:
		errmsg = "网络连接失败，请检查网络或稍候再试";
		break;
	case 31075:
		errmsg = "一次支持操作999个，减点试试吧";
		break;
	case 31080:
		errmsg = "我们的服务器出错了，稍候试试吧";
		break;
	case 31116:
		errmsg = "你的空间不足了哟，赶紧购买空间吧";
		break;
	case -1:
		errmsg = "用户名和密码验证失败";
		break;
	case -2:
		errmsg = "备用";
		break;
	case -3:
		errmsg = "用户未激活（调用init接口）";
		break;
	case -4:
		errmsg = "COOKIE中未找到host_key&user_key（或BDUSS）";
		break;
	case -5:
		errmsg = "host_key和user_key无效";
		break;
	case -6:
		errmsg = "bduss无效";
		break;
	case -7:
		errmsg = "文件或目录名错误或无权访问";
		break;
	case -8:
		errmsg = "该目录下已存在此文件";
		break;
	case -9:
		errmsg = "文件被所有者删除，操作失败";
		break;
	case -10:
		errmsg = "网盘空间已满";
		break;
	case -11:
		errmsg = "父目录不存在";
		break;
	case -12:
		errmsg = "设备尚未注册";
		break;
	case -13:
		errmsg = "设备已经被绑定";
		break;
	case -14:
		errmsg = "帐号已经初始化";
		break;
	case -21:
		errmsg = "预置文件无法进行相关操作";
		break;
	case -22:
		errmsg = "被分享的文件无法重命名，移动等操作";
		break;
	case -23:
		errmsg = "数据库操作失败，请联系netdisk管理员";
		break;
	case -24:
		errmsg = "要取消的文件列表中含有不允许取消public的文件。";
		break;
	case -25:
		errmsg = "非公测用户";
		break;
	case -26:
		errmsg = "邀请码失效";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是共享文件的错误消息*/
const char *get_share_errmsg_by_errno(int error)
{
	const char *errmsg = NULL;
	switch (error) {
	case 0:
		errmsg = "成功";
		break;
	case 2:
		errmsg = "参数错误";
		break;
	case 3:
		errmsg = "未登录或帐号无效";
		break;
	case 4:
		errmsg = "存储好像出问题了，请稍候再试";
		break;
	case 108:
		errmsg = "文件名有敏感词，优化一下吧";
		break;
	case 110:
		errmsg = "分享次数超出限制，可以到“我的分享”中查看已分享的文件链接";
		break;
	case 114:
		errmsg = "当前任务不存在，保存失败";
		break;
	case 115:
		errmsg = "该文件禁止分享";
		break;
	case -1:
		errmsg = "由于您分享了违反相关法律法规的文件，分享功能已被禁用，之前分享出去的文件不受影响。";
		break;
	case -2:
		errmsg = "用户不存在,请刷新页面后重试";
		break;
	case -3:
		errmsg = "文件不存在,请刷新页面后重试";
		break;
	case -4:
		errmsg = "登录信息有误，请重新登录试试";
		break;
	case -5:
		errmsg = "host_key和user_key无效";
		break;
	case -6:
		errmsg = "请重新登录";
		break;
	case -7:
		errmsg = "该分享已删除或已取消";
		break;
	case -8:
		errmsg = "该分享已经过期";
		break;
	case -9:
		errmsg = "访问密码错误";
		break;
	case -10:
		errmsg = "分享外链已经达到最大上限100000条，不能再次分享";
		break;
	case -11:
		errmsg = "验证cookie无效";
		break;
	case -14:
		errmsg = "对不起，短信分享每天限制20条，你今天已经分享完，请明天再来分享吧！";
		break;
	case -15:
		errmsg = "对不起，邮件分享每天限制20封，你今天已经分享完，请明天再来分享吧！";
		break;
	case -16:
		errmsg = "对不起，该文件已经限制分享！";
		break;
	case -17:
		errmsg = "文件分享超过限制";
		break;
	case -30:
		errmsg = "文件已存在";
		break;
	case -31:
		errmsg = "文件保存失败";
		break;
	case -33:
		errmsg = "一次支持操作10000个，减点试试吧";
		break;
	case -32:
		errmsg = "你的空间不足了哟，赶紧购买空间吧";
		break;
	case -70:
		errmsg = "你分享的文件中包含病毒或疑似病毒，为了你和他人的数据安全，换个文件分享吧";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是离线下载的错误消息*/
const char *get_download_errmsg_by_errno(int error)
{
	const char *errmsg = NULL;
	switch (error) {
	case 36000:
		errmsg = "网络繁忙，请稍候再试";
		break;
	case 36001:
		errmsg = "参数错误";
		break;
	case 36002:
		errmsg = "appid错误";
		break;
	case 36003:
		errmsg = "请刷新再试";
		break;
	case 36004:
		errmsg = "请重新登录";
		break;
	case 36005:
		errmsg = "用户未登录";
		break;
	case 36006:
		errmsg = "用户未激活";
		break;
	case 36007:
		errmsg = "用户未授权";
		break;
	case 36008:
		errmsg = "用户不存在";
		break;
	case 36009:
		errmsg = "用户空间不足";
		break;
	case 36010:
		errmsg = "文件不存在";
		break;
	case 36012:
		errmsg = "操作超时，请重试";
		break;
	case 36013:
		errmsg = "同时下载的任务过多，不能下载";
		break;
	case 36014:
		errmsg = "存储路径已使用";
		break;
	case 36016:
		errmsg = "任务已删除";
		break;
	case 36017:
		errmsg = "任务已完成";
		break;
	case 36018:
		errmsg = "解析失败，种子文件损坏";
		break;
	case 36019:
		errmsg = "任务正在处理中";
		break;
	case 36020:
		errmsg = "任务地址不存在";
		break;
	case 36021:
		errmsg = "普通用户最多同时下载1个任务哦！马上开通离线下载套餐，立即下载更多！";
		break;
	case 36022:
		errmsg = "同时下载的任务过多，不能下载";
		break;
	case 36023:
		errmsg = "普通用户每月只能离线下载5个任务哦！马上开通离线下载套餐，立即下载更多！";
		break;
	case 36024:
		errmsg = "本月下载数已超限制";
		break;
	case 36025:
		errmsg = "分享链接已失效";
		break;
	case 36026:
		errmsg = "链接格式有误";
		break;
	case 36027:
		errmsg = "链接格式有误";
		break;
	case 36028:
		errmsg = "暂时无法找到相关种子信息";
		break;
	case 36031:
		errmsg = "网络繁忙，请稍候再试";
		break;
	case -19:
		errmsg = "请输入验证码";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是购买套餐的错误消息*/
const char *get_buy_errmsg_by_errno(int error)
{
	const char *errmsg = NULL;
	switch (error) {
	case 1000:
		errmsg = "啊哦，不能从高级套餐升级到低级套餐，请重新购买吧。";
		break;
	case 1001:
		errmsg = "啊哦，不能从高级套餐升级到低级套餐，请重新购买吧。";
		break;
	case 1002:
		errmsg = "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1003:
		errmsg = "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1004:
		errmsg = "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1005:
		errmsg = "啊哦，创建订单失败了，请稍候重试一下吧。";
		break;
	case 1006:
		errmsg = "啊哦，不能同一天购买相同产品，请重新购买吧。";
		break;
	case 1007:
		errmsg = "啊哦，不能同一天购买相同产品，请重新购买吧。";
		break;
	case 3002:
		errmsg = "啊哦，该订单已经支付成功，请重新购买吧。";
		break;
	case 3003:
		errmsg = "啊哦，该订单已经支付失败，请重新购买吧。";
		break;
	case 36000:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36001:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36002:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36003:
		errmsg = "该次访问异常，权限受限";
		break;
	case 36005:
		errmsg = "验证码输入错误，请刷新重试";
		break;
	case 36006:
		errmsg = "啊哦，我们还没推出这款产品呀";
		break;
	case 36007:
		errmsg = "作异常，喝杯茶重试下？";
		break;
	case 36008:
		errmsg = "哎呀，你的操作异常，请刷新重试。";
		break;
	case 36009:
		errmsg = "基础套餐不允许购买";
		break;
	case 36010:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36011:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36012:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36013:
		errmsg = "该订单无法重新支付";
		break;
	case 36014:
		errmsg = "啊哦，该订单已经支付成功，请重新购买吧。";
		break;
	case 36015:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36016:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36017:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36018:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	case 36019:
		errmsg = "啊哦，服务器感冒了，紧急医治中……";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

/*从网盘JS获取，应该是购买套餐的错误消息*/
const char *get_record_errmsg_by_errno(int error)
{
	const char *errmsg = NULL;
	switch (error) {
	case 36000:
		errmsg = "内部错误";
		break;
	case 36001:
		errmsg = "不支持的API";
		break;
	case 36002:
		errmsg = "参数错误";
		break;
	case 36003:
		errmsg = "无权限访问";
		break;
	case 36005:
		errmsg = "验证码失效或非法";
		break;
	case 36006:
		errmsg = "要购买的产品不存在";
		break;
	case 36007:
		errmsg = "用户在购买产品的过程中并行操作，一般不会出现该错误码";
		break;
	case 36016:
		errmsg = "该功能不是属性值，不能获取，接入系统关注";
		break;
	case 36017:
		errmsg = "该功能不是消费值，不能消费，接入系统关注";
		break;
	case 36018:
		errmsg = "要消费的功能项未找到，接入系统关注";
		break;
	case 36019:
		errmsg = "拒绝消费，用户没有配额了，接入系统关注";
		break;
	case 36020:
		errmsg = "请求重放";
		break;
	case 36021:
		errmsg = "请求过期，或者是第三方伪造的请求";
		break;
	case 36031:
		errmsg = "第三方api参数错误";
		break;
	case 36032:
		errmsg = "第三方api签名错误";
		break;
	case 36033:
		errmsg = "第三方api文件错误";
		break;
	case 36034:
		errmsg = "数据库错误";
		break;
	case 36035:
		errmsg = "订单已经存在";
		break;
	case 36036:
		errmsg = "订单token失效";
		break;
	case 36037:
		errmsg = "查询订单不存在";
		break;
	case 36038:
		errmsg = "省市参数错误";
		break;
	case 36039:
		errmsg = "单价不存在";
		break;
	default:
		errmsg = "Unknow error";
		break;
	}
	return errmsg;
}

void pcs_clear_errmsg(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->errmsg) {
		pcs_free(pcs->errmsg);
		pcs->errmsg = NULL;
	}
}

void pcs_set_errmsg(Pcs handle, const char *fmt, ...)
{
	struct pcs *pcs = (struct pcs *)handle;
	va_list args;
	if (pcs->errmsg) {
		pcs_free(pcs->errmsg);
		pcs->errmsg = NULL;
	}
	va_start(args, fmt);
	pcs->errmsg = pcs_utils_vsprintf(fmt, args);
	va_end(args);
}

void pcs_cat_errmsg(Pcs handle, const char *fmt, ...)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *p, *errmsg;
	size_t sz = 0;
	va_list args;
	va_start(args, fmt);
	errmsg = pcs_utils_vsprintf(fmt, args);
	va_end(args);
	if (pcs->errmsg) {
		sz = strlen(pcs->errmsg) + strlen(errmsg);
		p = (char *)pcs_malloc(sz + 1);
		strcpy(p, pcs->errmsg);
		pcs_free(pcs->errmsg);
		pcs->errmsg = p;
		strcat(p, errmsg);
	}
	else {
		pcs->errmsg = errmsg;
	}
}

//static inline void pcs_set_errmsg(Pcs handle, PcsRes error)
//{
//	if (error != PCS_NONE)
//		pcs_set_error(handle, pcs_strerror(handle, error));
//}

/*调用用户注册的验证码函数来让用户识别验证码图片，并让用户输入识别结果*/
PcsRes pcs_get_captcha(Pcs handle, const char *code_string, char *captcha, int captchaSize)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *img;
	size_t imgsz;

	captcha[0] = '\0';
	if (!pcs->captcha_func) {
		pcs_set_errmsg(handle, "No captch function, please regist the function by call pcs_setopt(handle, PCS_OPTION_CAPTCHA_FUNCTION, pFun).");
		return PCS_NO_CAPTCHA_FUNC;
	}
	url = pcs_utils_sprintf(URL_CAPTCHA "%s", code_string);
	img = pcs_http_get_raw(pcs->http, url, PcsTrue, &imgsz); 
	if (!img) {
		pcs_set_errmsg(handle, "Can't get captch image from the server. url: %s. Error message: %s", url, pcs_http_strerror(pcs->http));
		pcs_free(url);
		return PCS_NETWORK_ERROR;
	}
	if (!(*pcs->captcha_func)((unsigned char *)img, imgsz, captcha, captchaSize, pcs->captcha_data)) {
		pcs_set_errmsg(handle, "Can't get captch from the captcha, which is regist by user.");
		pcs_free(url);
		return PCS_GET_CAPTCHA_FAIL;
	}
	pcs_free(url);
	return PCS_OK;
}

/*从html中解析出类似于 FileUtils.bdstoken="****" 的值。此例中，key传入"FileUtils.bdstoken"，将返回"****"值*/
static char *pcs_get_value_by_key(const char *html, const char *key)
{
	char *val = NULL;
	const char *p = html,
		*end = NULL,
		*tmp;
	int i = strlen(key);// \f\n\r\t\v
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			PCS_SKIP_SPACE(tmp);
			if (*tmp != '=') continue; tmp++;
			PCS_SKIP_SPACE(tmp);
			if (*tmp != '"') continue; tmp++;
			end = tmp;
			while (*end && *end != '"') end++;
			if (*end == '"') {
				val = (char *)pcs_malloc(end - tmp + 1);
				memcpy(val, tmp, end - tmp);
				val[end - tmp] = '\0';
				return val;
			}
		}
		p++;
	}
	return val;
}

/*从URL地址的QueryString中解析出类似于 &error=123&a= 的值。此例中，key传入"&error"，将返回123*/
static char *pcs_get_embed_query_int_value_by_key(const char *html, const char *key)
{
	char *val = NULL;
	const char *p = html,
		*end = NULL,
		*tmp;
	int i = strlen(key);// \f\n\r\t\v
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			if (*tmp != '=') continue; tmp++;
			end = tmp;
			while (*end && *end >= '0' && *end <= '9') end++;
			if (end > tmp) {
				val = (char *)pcs_malloc(end - tmp + 1);
				memcpy(val, tmp, end - tmp);
				val[end - tmp] = '\0';
				return val;
			}
		}
		p++;
	}
	return val;
}

/*从URL地址的QueryString中解析出类似于 &token=abc_ef-g&a= 的值。此例中，key传入"&token"，将返回abc_ef-g*/
static char *pcs_get_embed_query_token_by_key(const char *html, const char *key)
{
	char *val = NULL;
	const char *p = html,
		*end = NULL,
		*tmp;
	int i = strlen(key);// \f\n\r\t\v
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			if (*tmp != '=') continue; tmp++;
			end = tmp;
			while (*end && PCS_IS_TOKEN_CHAR(*end)) end++;
			if (end > tmp) {
				val = (char *)pcs_malloc(end - tmp + 1);
				memcpy(val, tmp, end - tmp);
				val[end - tmp] = '\0';
				return val;
			}
		}
		p++;
	}
	return val;
}

/*pcs_build_pan_api_url()的矢量模式*/
static char *pcs_build_pan_api_url_v(Pcs handle, const char *action, va_list args)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *baseurl, *url, *tt;
	tt = pcs_utils_sprintf("%d", (int)time(0));
	baseurl = (char *)alloca(strlen(URL_PAN_API) + strlen(action) + 1);
	strcpy(baseurl, URL_PAN_API);
	strcat(baseurl, action);
	baseurl = pcs_http_build_url(pcs->http, baseurl,
		"channel", "chunlei",
		"clienttype", "0",
		"web","1", 
		"t", tt,
		"bdstoken", pcs->bdstoken,
		NULL);
	pcs_free(tt);
	if (!baseurl)
		return NULL;
	url = pcs_http_build_url_v(pcs->http, baseurl, args);
	pcs_free(baseurl);
	return url;
}

/*传入action，即参数，拼接出最终字符串。参数以(key, value)对的形式传入，最后传入一个NULL表示终止
例：pcs_build_pan_api_url(handle, "list", k1, v1, k2, v2, NULL); 
//将返回：http://pan.baidu.com/api/list?channel=chunlei&clienttype=0&web=1&t=189343343&bdstoken=dfjewdfe&k1=v1&k2=v2
*/
static char *pcs_build_pan_api_url(Pcs handle, const char *action, ...)
{
	char *url;
	va_list args;
    va_start(args, action);
	url = pcs_build_pan_api_url_v(handle, action, args);
    va_end(args);
	return url;
}

/*转换JSON对象为PcsFileInfo对象。
JSON对象格式为：
{
	"fs_id": 123,
	"path": "/a/b",
	"server_filename": "b",
	"mtime": 1899383,
	"ctime": 1899383,
	"server_mtime": 1899383,
	"server_ctime": 1899383,
	"local_mtime": 1899383,
	"local_ctime": 1899383,
	"isdir": 1,
	"category": 1,
	"size": 0,
	"dir_empty": 0,
	"empty": 0,
	"ifhassubdir": 0,
	"md5": "",
	"dlink": "",
	"block_list": [ "/a/b/1", "a/b/2" ]
}
*/
static PcsFileInfo *pcs_parse_fileinfo(cJSON * item)
{
	cJSON *val, *list;
	PcsFileInfo *fi = pcs_fileinfo_create();
	val = cJSON_GetObjectItem(item, "fs_id");
	if ((val = cJSON_GetObjectItem(item, "fs_id")))
		fi->fs_id = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "path");
	if (val)
		fi->path = pcs_utils_strdup(val->valuestring);

	val = cJSON_GetObjectItem(item, "server_filename");
	if (val)
		fi->server_filename = pcs_utils_strdup(val->valuestring);

	val = cJSON_GetObjectItem(item, "mtime");
	if (val)
		fi->server_mtime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "ctime");
	if (val)
		fi->server_ctime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "server_mtime");
	if (val)
		fi->server_mtime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "server_ctime");
	if (val)
		fi->server_ctime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "local_mtime");
	if (val)
		fi->local_mtime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "local_ctime");
	if (val)
		fi->local_ctime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "isdir");
	if (val)
		fi->isdir = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "category");
	if (val)
		fi->category = val->valueint;

	val = cJSON_GetObjectItem(item, "size");
	if (val)
		fi->size = (size_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "dir_empty");
	if (val)
		fi->dir_empty = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "empty");
	if (val)
		fi->empty = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "ifhassubdir");
	if (val)
		fi->ifhassubdir = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "md5");
	if (val)
		fi->md5 = pcs_utils_strdup(val->valuestring);

	val = cJSON_GetObjectItem(item, "dlink");
	if (val)
		fi->dlink = pcs_utils_strdup(val->valuestring);

	list = cJSON_GetObjectItem(item, "block_list");
	if (list) {
		int i, cnt = cJSON_GetArraySize(list);
		if (cnt > 0) {
			fi->block_list = (char **) pcs_malloc((cnt + 1) + sizeof(char *));
			if (!fi->block_list) return fi;
			memset(fi->block_list, 0, (cnt + 1) + sizeof(char *));
			for (i = 0; i < cnt; i++) {
				val = cJSON_GetArrayItem(list, i);
				fi->block_list[i] = pcs_utils_strdup(val->valuestring);
			}
		}
	}
	return fi;
}

/*
根据传入参数，执行api函数。参数传入方法，参考pcs_build_pan_api_url()函数
action: list, search
*/
static PcsFileInfoList *pcs_pan_api_1(Pcs handle, const char *action, ...)
{
	struct pcs *pcs = (struct pcs *)handle;
	va_list args;
	cJSON *json, *item, *list;
	char *url, *html;
	int error, cnt, i;
	PcsFileInfoList *filist = NULL;
	PcsFileInfoListItem *filist_item;
	PcsFileInfo *fi;

	pcs_clear_errmsg(handle);
    va_start(args, action);
	url = pcs_build_pan_api_url_v(handle, action, args);
    va_end(args);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build url.");
		return NULL;
	}
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json){
		//printf("%s\n", html);
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		//printf("%s\n", html);
		pcs_set_errmsg(handle, "Can't read res.errno: %s", html);
		cJSON_Delete(json);
		return NULL;
	}
	error = item->valueint;
	if (error != 0) {
		//printf("%s\n", html);
		pcs_set_errmsg(handle, "%s, Error：%d", get_errmsg_by_errno(error), error);
		cJSON_Delete(json);
		return NULL;
	}
	list = cJSON_GetObjectItem(json, "list");
	if (!list) {
		pcs_set_errmsg(handle, "Can't read res.list: %s", html);
		cJSON_Delete(json);
		return NULL;
	}
	cnt = cJSON_GetArraySize(list);
	if (cnt <= 0) {
		cJSON_Delete(json);
		return NULL;
	}
	filist = pcs_filist_create();
	if (!filist) {
		pcs_set_errmsg(handle, "Can't create object: PcsFileInfoList");
		cJSON_Delete(json);
		return NULL;
	}
	for (i = 0; i < cnt; i++) {
		item = cJSON_GetArrayItem(list, i);
		filist_item = pcs_filistitem_create();
		if (!filist_item) {
			pcs_set_errmsg(handle, "Can't create object: PcsFileInfoListItem");
			cJSON_Delete(json);
			if (filist) pcs_filist_destroy(filist);
			return NULL;
		}
		fi = pcs_parse_fileinfo(item);
		if (!fi) {
			pcs_set_errmsg(handle, "Can't create object: PcsFileInfo");
			cJSON_Delete(json);
			pcs_filistitem_destroy(filist_item);
			if (filist) pcs_filist_destroy(filist);
			return NULL;
		}
		filist_item->info = fi;
		pcs_filist_add(filist, filist_item);
	}
	cJSON_Delete(json);
	return filist;
}

/*根据slist字符串链表，构造成数组格式的json字符串，并把数组元素数量写入到count指定的内存中*/
static char *pcs_build_filelist_1(Pcs handle, PcsSList *slist, int *count)
{
	PcsSList *item;
	char *file_list_string;
	size_t sz;
	int cnt = 0;

	if (count) *count = cnt;
	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string);
		sz += 3;
		item = item->next;
		cnt++;
	}
	sz += 2;
	if (count) *count = cnt;

	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
		return NULL;
	}

	strcpy(file_list_string, "[");
	item = slist;
	while(item) {
		strcat(file_list_string, "\"");
		strcat(file_list_string, item->string);
		strcat(file_list_string, "\",");
		item = item->next;
	}
	file_list_string[sz - 2] = ']';
	file_list_string[sz - 1] = '\0';
	return file_list_string;
}

/*根据slist字符串链表，构造成数组格式的json字符串，并把数组元素数量写入到count指定的内存中*/
static char *pcs_build_filelist_2(Pcs handle, PcsSList2 *slist, int *count)
{
	PcsSList2 *item;
	char *file_list_string, *p;
	size_t sz;
	int cnt = 0;

	if (count) *count = cnt;
	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string1);
		sz += strlen(item->string2);
		sz += 25;
		item = item->next;
		cnt++;
	}
	sz += 2;
	if (count) *count = cnt;

	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
		return NULL;
	}

	strcpy(file_list_string, "[");
	p = file_list_string;
	item = slist;
	while(item) {
		while(*p) p++;
		sprintf(p, "{\"path\":\"%s\",\"newname\":\"%s\"},", item->string1, item->string2);
		p += 25;
		item = item->next;
	}
	file_list_string[sz - 2] = ']';
	file_list_string[sz - 1] = '\0';

	return file_list_string;
}

/*根据slist字符串链表，构造成数组格式的json字符串，并把数组元素数量写入到count指定的内存中*/
static char *pcs_build_filelist_3(Pcs handle, PcsSList2 *slist, int *count)
{
	PcsSList2 *item;
	char *file_list_string, *p, *dir, *filename;
	size_t sz;
	int cnt = 0;

	if (count) *count = cnt;
	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string1);
		sz += strlen(item->string2);
		sz += 35;
		item = item->next;
		cnt++;
	}
	sz += 2;
	if (count) *count = cnt;

	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
		return NULL;
	}
	memset(file_list_string, 0, sz);
	strcpy(file_list_string, "[");
	p = file_list_string;
	item = slist;
	while(item) {
		while(*p) p++;
		dir = pcs_utils_basedir(item->string2);
		filename = pcs_utils_filename(item->string2);
		if (!dir || !filename) {
			pcs_free(file_list_string);
			pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
			return NULL;
		}
		sprintf(p, "{\"path\":\"%s\",\"dest\":\"%s\",\"newname\":\"%s\"},", 
			item->string1, dir, filename);
		pcs_free(dir);
		pcs_free(filename);
		p += 35;
		item = item->next;
	}
	p = file_list_string + sz - 1;
	while (p > file_list_string) {
		if (*p == ',')
			break;
		p--;
	}
	if (*p == ',') {
		*p = ']';
		p++;
		*p = '\0';
	}

	return file_list_string;
}

/*
根据传入参数，执行api函数。
opera: delete, rename, move, copy
*/
static PcsPanApiRes *pcs_pan_api_filemanager(Pcs handle, const char *opera, const char *filelist, int file_count)
{
	struct pcs *pcs = (struct pcs *)handle;
	cJSON *json, *item, *list, *val;
	char *url, *html, *postdata;
	int error, cnt, i;
	PcsPanApiRes *res = NULL;
	PcsPanApiResInfoList *tail, *ri;

	PcsBool err_no_space = PcsFalse, //errno = -10 剩余空间不足
		err_target_not_exist = PcsFalse, //errno = -8 文件已存在于目标文件夹中
		err_src_file_not_exist = PcsFalse, //errno = -9 文件不存在
		err_has_succ_items = PcsFalse; //是否存在处理成功的文件项

	url = pcs_build_pan_api_url(handle, "filemanager",
		"opera", opera,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}

	postdata = pcs_http_build_post_data(pcs->http, "filelist", filelist, NULL);
	if (!postdata) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(url);
		return NULL;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(url);
	pcs_free(postdata);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json){
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		return NULL;
	}
	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.errno. Response: %s", html);
		cJSON_Delete(json);
		return NULL;
	}
	error = item->valueint;
	res = pcs_pan_api_res_create();
	if (!res) {
		pcs_set_errmsg(handle, "Can't create the object: PcsPanApiRes");
		cJSON_Delete(json);
		return NULL;
	}
	res->error = error;

	list = cJSON_GetObjectItem(json, "info");
	if (!list) {
		pcs_set_errmsg(handle, "Can't read res.info. Response: %s", html);
		cJSON_Delete(json);
		return NULL;
	}

	cnt = cJSON_GetArraySize(list);
	for (i = 0; i < cnt; i++) {
		item = cJSON_GetArrayItem(list, i);
		ri = pcs_pan_api_res_infolist_create();
		if (!ri) {
			pcs_set_errmsg(handle, "Can't create the object: PcsPanApiResInfoList");
			cJSON_Delete(json);
			if (res) pcs_pan_api_res_destroy(res);
			return NULL;
		}
		val = cJSON_GetObjectItem(item, "path");
		if (val)
			ri->info.path = pcs_utils_strdup(val->valuestring);
		val = cJSON_GetObjectItem(item, "errno");
		ri->info.error = val->valueint;
		if (!res->info_list) {
			res->info_list = tail = ri;
		}
		else {
			tail->next = ri;
			tail = ri;
		}
		switch (ri->info.error) {
		case 0: //I:处理成功
			err_has_succ_items = PcsTrue;
			break;
		case -8: //D:文件已存在于目标文件夹中
			err_target_not_exist = PcsTrue;
			break;
		case -9: //C:文件不存在
			err_src_file_not_exist = PcsTrue;
			break;
		case -10: //G:剩余空间不足
			err_no_space = PcsTrue;
			break;
		}
	}
	cJSON_Delete(json);
	if (res->error != 0) {
		int is_move = strcmp(opera, "move") == 0;
		if (is_move || strcmp(opera, "copy") == 0) {
			if (res->error == 12) {
				if (!err_no_space && !err_src_file_not_exist && !err_target_not_exist) {
					pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，请稍候重试");
				}
				else {
					if (cnt == file_count && !err_has_succ_items) {
						if (err_no_space)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，剩余空间不足");
						else if (err_target_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，文件已存在于目标文件夹中");
						else if (err_src_file_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，待处理文件不存在");
					}
					else {
						if (err_no_space)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，剩余空间不足");
						else if (err_target_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，部分文件已存在于目标文件夹中");
						else if (err_src_file_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，部分待处理文件不存在");
					}
				}
			}
			else {
				pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，未知错误");
			}
		}
		else if (strcmp(opera, "delete") == 0) {
			pcs_set_errmsg(handle, "删除文件失败，请稍候重试");
		}
		else if (strcmp(opera, "rename") == 0) {
			if (res->error == -8 || res->error == -9 || (cnt > 0 && (res->info_list->info.error == -8 || res->info_list->info.error == -9)))
				pcs_set_errmsg(handle, "名称已在使用，请使用其他名称");
			else if (res->error == 12 && cnt > 0 && res->info_list->info.error == -9)
				pcs_set_errmsg(handle, "重命名失败，文件已被删除或移走");
			else
				pcs_set_errmsg(handle, "文件(夹)重命名失败, 网络错误");
		}
	}
	return res;
}

static int pcs_get_errno_from_api_res(Pcs handle, const char *html)
{
	int res;
	cJSON *json, *item;
	
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return 1;
	}

	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.errno: %s", html);
		cJSON_Delete(json);
		return 1;
	}
	res = item->valueint;
	cJSON_Delete(json);
	return res;
}

static PcsFileInfo *pcs_upload_form(Pcs handle, const char *path, PcsBool overwrite, PcsHttpForm form)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *html,
		*dir = pcs_utils_basedir(path),
		*filename = pcs_utils_filename(path);
	cJSON *json, *item;
	PcsFileInfo *meta;

	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "upload",
		"app_id", "250528",
		"ondup", overwrite ? "overwrite" : "newcopy",
		"dir", dir,
		"filename", filename,
		"BDUSS", pcs->bduss, 
		NULL);
	pcs_free(dir);
	pcs_free(filename);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}
	html = pcs_post_httpform(pcs->http, url, form, PcsTrue);
	pcs_free(url);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	//"{\"error_code\":31062,\"error_msg\":\"file name is invalid\",\"request_id\":3071564675}
	item = cJSON_GetObjectItem(json, "error_code");
	if (item) {
		int error_code = item->valueint;
		const char *error_msg = NULL;
		item = cJSON_GetObjectItem(json, "error_msg");
		if (item)
			error_msg = item->valuestring;
		pcs_set_errmsg(handle, "Can't upload file. error_code: %d. error_msg: %s. raw response: %s", error_code, error_msg, html);
		cJSON_Delete(json);
		return NULL;
	}

	meta = pcs_parse_fileinfo(json);
	cJSON_Delete(json);
	if (!meta) {
		pcs_set_errmsg(handle, "Can't parse the response as meta");
		return NULL;
	}
	return meta;
}

PCS_API const char *pcs_version()
{
	return PCS_API_VERSION;
}

PCS_API Pcs pcs_create(const char *cookie_file)
{
	struct pcs *pcs;

	pcs = (struct pcs *) pcs_malloc(sizeof(struct pcs));
	if (!pcs)
		return NULL;
	memset(pcs, 0, sizeof(struct pcs));
	pcs->http = pcs_http_create(cookie_file);
	if (!pcs->http) {
		pcs_free(pcs);
		return NULL;
	}
	return pcs;
}

PCS_API void pcs_destroy(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->http)
		pcs_http_destroy(pcs->http);
	if (pcs->username)
		pcs_free(pcs->username);
	if (pcs->password)
		pcs_free(pcs->password);
	if (pcs->bdstoken)
		pcs_free(pcs->bdstoken);
	if (pcs->bduss)
		pcs_free(pcs->bduss);
	if (pcs->sysUID)
		pcs_free(pcs->sysUID);
	if (pcs->errmsg)
		pcs_free(pcs->errmsg);
	if (pcs->secure_key)
		pcs_free(pcs->secure_key);
	if (pcs->buffer)
		pcs_free(pcs->buffer);
	pcs_free(pcs);
}

PCS_API const char *pcs_sysUID(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs->sysUID;
}

PCS_API const char *pcs_strerror(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs->errmsg;
}

PCS_API PcsRes pcs_setopt(Pcs handle, PcsOption opt, void *value)
{
	PcsRes res = PCS_OK;
	struct pcs *pcs = (struct pcs *)handle;
	switch (opt)
	{
	case PCS_OPTION_USERNAME:
		if (pcs->username) pcs_free(pcs->username);
		pcs->username = pcs_utils_strdup((char *)value);
		break;
	case PCS_OPTION_PASSWORD:
		if (pcs->password) pcs_free(pcs->password);
		pcs->password = pcs_utils_strdup((char *)value);
		break;
	case PCS_OPTION_CAPTCHA_FUNCTION:
		pcs->captcha_func = (PcsGetCaptchaFunction)value;
		break;
	case PCS_OPTION_CAPTCHA_FUNCTION_DATA:
		pcs->captcha_data = value;
		break;
	case PCS_OPTION_DOWNLOAD_WRITE_FUNCTION:
		//pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, value);
		pcs->download_func = (PcsHttpWriteFunction)value;
		break;
	case PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA:
		//pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE, value);
		pcs->download_data = value;
		break;
	case PCS_OPTION_HTTP_RESPONSE_FUNCTION:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION, value);
		break;
	case PCS_OPTION_HTTP_RESPONSE_FUNCTION_DATE:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION_DATE, value);
		break;
	case PCS_OPTION_PROGRESS_FUNCTION:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_PROGRESS_FUNCTION, value);
		break;
	case PCS_OPTION_PROGRESS_FUNCTION_DATE:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_PROGRESS_FUNCTION_DATE, value);
		break;
	case PCS_OPTION_PROGRESS:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_PROGRESS, value);
		break;
	case PCS_OPTION_SECURE_METHOD:
		pcs->secure_method = (int)((long)value);
		break;
	case PCS_OPTION_SECURE_KEY:
		if (pcs->secure_key) pcs_free(pcs->secure_key);
		pcs->secure_key = pcs_utils_strdup((char *)value);
		break;
	case PCS_OPTION_SECURE_ENABLE:
		pcs->secure_enable = (PcsBool)((long)value);
		break;
	case PCS_OPTION_USAGE:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_USAGE, value);
		break;
	default:
		pcs_set_errmsg(handle, "Unknown option");
		res = PCS_UNKNOWN_OPT;
		break;
	}
	return res;
}

PCS_API PcsRes pcs_setopts(Pcs handle, ...)
{
	PcsRes res = PCS_OK;
	PcsOption opt;
	void *val;
    va_list args;
    va_start(args, handle);
	while((opt = va_arg(args, PcsOption)) != PCS_OPTION_END) {
		val = va_arg(args, void *);
		if ((res = pcs_setopt(handle, opt, val)) != PCS_OK) {
			break;
		}
	}
    va_end(args);
	return res;
}

PCS_API PcsRes pcs_islogin(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	const char *html;

	pcs_clear_errmsg(handle);
	html = pcs_http_get(pcs->http, URL_DISK_HOME, PcsTrue);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the server.");
		return PCS_NETWORK_ERROR;
	}
	
	if (pcs->bdstoken) pcs_free(pcs->bdstoken);
	pcs->bdstoken = pcs_get_value_by_key(html, "yunData.MYBDSTOKEN");
	if (!pcs->bdstoken || strlen(pcs->bdstoken) == 0) {
		pcs->bdstoken = pcs_get_value_by_key(html, "FileUtils.bdstoken");
	}
	if (pcs->bdstoken != NULL && strlen(pcs->bdstoken) > 0) {
		//printf("bdstoken: %s\n", pcs->bdstoken);
		if (pcs->bduss) pcs_free(pcs->bduss);
		pcs->bduss = pcs_http_get_cookie(pcs->http, "BDUSS");
		if (pcs->sysUID) pcs_free(pcs->sysUID);
		pcs->sysUID = pcs_get_value_by_key(html, "yunData.MYNAME");
		if (!pcs->sysUID || strlen(pcs->sysUID) == 0) {
			pcs->sysUID = pcs_get_value_by_key(html, "FileUtils.sysUID");
		}
		return PCS_LOGIN;
	}
	return PCS_NOT_LOGIN;
}

PCS_API PcsRes pcs_login(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes res;
	char *p, *html, *url, *token, *code_string, captch[8], *post_data, *tt;
	cJSON *json, *root, *item;
	int error = -1, i;

	pcs_clear_errmsg(handle);
	html = pcs_http_get(pcs->http, URL_HOME, PcsTrue);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the server.");
		return PCS_NETWORK_ERROR;
	}
	html = pcs_http_get(pcs->http, URL_PASSPORT_API "login", PcsTrue);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the server.");
		return PCS_NETWORK_ERROR;
	}
	url = pcs_utils_sprintf(URL_PASSPORT_API "getapi" "&tpl=ik" "&apiver=v3" "&class=login" "&tt=%d", 
		(int)time(0));
	html = pcs_http_get(pcs->http, url, PcsTrue); pcs_free(url);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the server.");
		return PCS_NETWORK_ERROR;
	}
	json = cJSON_Parse(html);
	if (!json){
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		return PCS_WRONG_RESPONSE;
	}

	root = cJSON_GetObjectItem(json, "data");
	if (!root) {
		pcs_set_errmsg(handle, "Can't read res.data. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}

	item = cJSON_GetObjectItem(root, "token");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.token. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	token = pcs_utils_strdup(item->valuestring);

	item = cJSON_GetObjectItem(root, "codeString");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.codeString. Response: %s", html);
		pcs_free(token);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	code_string = pcs_utils_strdup(item->valuestring);

	cJSON_Delete(json);

	p = token;
	while (*p) {
		if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))) {
			pcs_set_errmsg(handle, "Invalid token. Token: %s", token);
			pcs_free(token);
			pcs_free(code_string);
			return PCS_WRONG_RESPONSE;
		}
		p++;
	}

	i = 0;
try_login:
	if (code_string[0]) {
		res = pcs_get_captcha(pcs, code_string, captch, sizeof(captch));
		if (res != PCS_OK) {
			pcs_free(token);
			pcs_free(code_string);
			return res;
		}
	}
	tt = pcs_utils_sprintf("%d", (int)time(0));
	post_data = pcs_http_build_post_data(pcs->http,
		"ppui_logintime", "6852",
		"charset", "UTF-8",
		"codestring", code_string,
		"token", token,
		"isPhone", "false",
		"index", "0",
		"u", "",
		"safeflg", "0",
		"staticpage", "http://www.baidu.com/cache/user/html/jump.html",
		"loginType", "1",
		"tpl", "mn",
		"callback", "parent.bdPass.api.login._postCallback",
		"username", pcs->username,
		"password", pcs->password,
		"verifycode", captch,
		"mem_pass", "on",
		"tt", tt,
		NULL);
	pcs_free(tt);
	if (!post_data) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(token);
		pcs_free(code_string);
		return PCS_BUILD_POST_DATA;
	}
	html = pcs_http_post(pcs->http, URL_PASSPORT_API "login", post_data, PcsTrue);
	pcs_free(post_data);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get the response from the server.");
		pcs_free(token);
		pcs_free(code_string);
		return PCS_NETWORK_ERROR;
	}
	else {
		char *errorStr = pcs_get_embed_query_int_value_by_key(html, "&error");
		if (!errorStr) {
			pcs_set_errmsg(handle, "Can't read the error from the response. Response: %s", html);
			pcs_free(token);
			pcs_free(code_string);
			return PCS_NETWORK_ERROR;
		}
		error = atoi(errorStr);
		pcs_free(errorStr);
	}

	if (error == 0) {
		if (pcs_islogin(pcs) == PCS_LOGIN) {
			pcs_free(token);
			pcs_free(code_string);
			return PCS_OK;
		}
		else {
			pcs_set_errmsg(handle, "Unknown Error");
			pcs_free(token);
			pcs_free(code_string);
			return PCS_FAIL;
		}
	}
	else {
		if (code_string) pcs_free(code_string);
		code_string = pcs_get_embed_query_token_by_key(html, "&codestring");
		if (!code_string) {
			pcs_set_errmsg(handle, "Can't read the codestring from the response. Response: %s", html);
			pcs_free(token);
			pcs_free(code_string);
			return PCS_FAIL;
		}
		pcs_set_errmsg(handle, "error: %d. Maybe because wrong captcha");
	}
	if (i < 3 && code_string[0]) {
		i++;
		goto try_login;
	}
	if (i == 3) {
		pcs_cat_errmsg(handle, " retry %d times", i);
	}
	else {
		pcs_set_errmsg(handle, "Can't get code_string");
	}
	pcs_free(token);
	pcs_free(code_string);
	pcs_set_errmsg(handle, "Unknown Error");
	return PCS_FAIL;
}

PCS_API PcsRes pcs_logout(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	int http_code;

	pcs_clear_errmsg(handle);
	pcs_http_get(pcs->http, URL_PASSPORT_LOGOUT, PcsFalse);
	http_code = pcs_http_code(pcs->http);
	if (http_code == 302) {
		if (pcs->username) {
			pcs_free(pcs->username);
			pcs->username = NULL;
		}
		if (pcs->password) {
			pcs_free(pcs->password);
			pcs->password = NULL;
		}
		if (pcs->bdstoken) {
			pcs_free(pcs->bdstoken);
			pcs->bdstoken = NULL;
		}
		if (pcs->bduss) {
			pcs_free(pcs->bduss);
			pcs->bduss = NULL;
		}
		if (pcs->sysUID) {
			pcs_free(pcs->sysUID);
			pcs->sysUID = NULL;
		}
		return PCS_OK;
	}
	pcs_set_errmsg(handle, "Can't logout. Http Code: %d", http_code);
	return PCS_FAIL;
}

PCS_API PcsRes pcs_quota(Pcs handle, size_t *quota, size_t *used)
{
	struct pcs *pcs = (struct pcs *)handle;
	cJSON *json, *item;
	char *url, *html;
	int error;

	pcs_clear_errmsg(handle);
	url = pcs_build_pan_api_url(handle, "quota", NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return PCS_BUILD_URL;
	}
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get the response from the server.");
		return PCS_NETWORK_ERROR;
	}

	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json. Response: %s", html);
		return PCS_WRONG_RESPONSE;
	}

	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.errno. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	error = item->valueint;
	if (error != 0) {
		pcs_set_errmsg(handle, "Unknown error. Response: %s", html);
		cJSON_Delete(json);
		return PCS_FAIL;
	}

	item = cJSON_GetObjectItem(json, "total");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.total. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (quota)
		*quota = (size_t)item->valuedouble;

	item = cJSON_GetObjectItem(json, "used");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.used. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (used)
		*used = (size_t)item->valuedouble;

	cJSON_Delete(json);

	return PCS_OK;
}

PCS_API PcsRes pcs_mkdir(Pcs handle, const char *path)
{
	struct pcs *pcs = (struct pcs *)handle;
	int error;
	char *html, *postdata, *url;

	pcs_clear_errmsg(handle);
	url = pcs_build_pan_api_url(handle, "create", "a", "commit", NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return PCS_BUILD_URL;
	}
	postdata = pcs_http_build_post_data(pcs->http, 
		"path", path,
		"isdir", "1",
		"size", "",
		"block_list", "[]",
		"method", "post",
		NULL);
	if (!postdata) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(url);
		return PCS_BUILD_POST_DATA;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(url);
	pcs_free(postdata);
	if (!html) {
		pcs_set_errmsg(handle, "Can't get response from the server.");
		return PCS_NETWORK_ERROR;
	}
	error = pcs_get_errno_from_api_res(handle, html);
	if (error) {
		return PCS_FAIL;
	}
	return PCS_OK;
}

PCS_API PcsFileInfoList *pcs_list(Pcs handle, const char *dir, int pageindex, int pagesize, const char *order, PcsBool desc)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *page, *pagenum, *tt;
	PcsFileInfoList *filist = NULL;
	pcs_clear_errmsg(handle);
	page = pcs_utils_sprintf("%d", pageindex);
	pagenum = pcs_utils_sprintf("%d", pagesize);
	tt = pcs_utils_sprintf("%d", (int)time(0));
	filist = pcs_pan_api_1(handle, "list", 
		"_", tt,
		"dir", dir,
		"page", page,
		"num", pagenum,
		"order", order,
		desc ? "desc" : "", desc ? "1" : "0",
		NULL);
	pcs_free(page);
	pcs_free(pagenum);
	pcs_free(tt);
	return filist;
}

PCS_API PcsFileInfoList *pcs_search(Pcs handle, const char *dir, const char *key, PcsBool recursion)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsFileInfoList *filist = NULL;
	pcs_clear_errmsg(handle);
	filist = pcs_pan_api_1(handle, "search", 
		"dir", dir,
		"key", key,
		recursion ? "recursion" : "", NULL,
		NULL);
	return filist;
}

PCS_API PcsFileInfo *pcs_meta(Pcs handle, const char *path)
{
	PcsFileInfoList *filist;
	PcsFileInfo *meta = NULL;
	char *dir, *key;
	PcsFileInfoListIterater iterater;
	
	pcs_clear_errmsg(handle);
	dir = pcs_utils_basedir(path);
	key = pcs_utils_filename(path);
	filist = pcs_search(handle, dir, key, PcsFalse);
	pcs_free(dir);
	pcs_free(key);
	if (!filist)
		return NULL;
	pcs_filist_iterater_init(filist, &iterater, PcsFalse);
	while(pcs_filist_iterater_next(&iterater)) {
		if (pcs_utils_strcmpi(path, iterater.current->path) == 0) {
			meta = iterater.current;
			break;
		}
	}
	if (meta) {
		meta = pcs_fileinfo_clone(meta);
		if (!meta) {
			pcs_set_errmsg(handle, "Can't clone the meta data.");
		}
	}
	else {
		pcs_set_errmsg(handle, "Can't find the file.");
	}
	pcs_filist_destroy(filist);
	return meta;
}

PCS_API PcsPanApiRes *pcs_delete(Pcs handle, PcsSList *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_1(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "delete", filelist, file_count);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_rename(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_2(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "rename", filelist, file_count);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_move(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_3(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "move", filelist, file_count);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_copy(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_3(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "copy", filelist, file_count);
	pcs_free(filelist);
	return res;
}

static inline void readToAesHead(char *buf, struct PcsAesHead *head)
{
	head->magic = readInt(buf);
	head->bits = readInt(&buf[4]);
	head->polish = readInt(&buf[8]);
}

//static inline void fillAesHead(const struct PcsAesHead *head, char *buf)
//{
//	int2Buffer(head->magic, buf);
//	int2Buffer(head->bits, &buf[4]);
//	int2Buffer(head->polish, &buf[8]);
//}

static struct PcsAesState *createPcsAesState(Pcs handle, int bits, int mod, int polish)
{
	struct pcs *pcs = (struct pcs *)handle;
	struct PcsAesState *state = NULL;
	const char *key;
	int rc;
	if (!pcs->secure_key) {
		pcs_set_errmsg(handle, "The key is not specify.");
		return NULL;
	}
	if (bits != 128 && bits != 192 && bits != 256) {
		pcs_set_errmsg(handle, "bits should be 128, 192 or 256.");
		return NULL;
	}
	state = (struct PcsAesState *)pcs_malloc(sizeof(struct PcsAesState));
	if (!state) {
		pcs_set_errmsg(handle, "Can't create PcsAesState.");
		return NULL;
	}
	memset(state, 0, sizeof(struct PcsAesState));
	state->head.magic = PCS_AES_MAGIC;
	state->head.bits = bits;
	state->head.polish = polish;
	state->mod = mod;
	key = md5_string_raw(pcs->secure_key);
	memcpy(state->key, key, AES_BLOCK_SIZE);
	switch (mod)
	{
	case AES_ENCRYPT:
		rc = AES_set_encrypt_key(key, bits, &state->aes);
		break;
	case AES_DECRYPT:
		rc = AES_set_decrypt_key(key, bits, &state->aes);
		break;
	default:
		pcs_set_errmsg(handle, "Unknow AES Mod.");
		pcs_free(state);
		return NULL;
		//break;
	}
	if (rc < 0) {
		pcs_set_errmsg(handle, "Can't set encryption|decryption key in AES.");
		pcs_free(state);
		return NULL;
	}
	return state;
}

static void destroyPcsAesState(struct PcsAesState *state)
{
	if (state) pcs_free(state);
}

static int pcs_download_aes_process_buffer(struct PcsDownloadState *state)
{
	struct pcs *pcs = (struct pcs *)state->handle;
	struct PcsAesState *aes = (struct PcsAesState *)state->userdata;
	int sz;

	aes->buffer_size = (state->buffer_size / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
	if (aes->buffer_size == state->buffer_size) {
		aes->buffer_size -= PCS_MD5_SIZE;
		aes->buffer_size = (aes->buffer_size / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
	}
	// decrypt AES_ENCRYPT
	AES_cbc_encrypt(state->buffer, aes->buffer,
		aes->buffer_size, &aes->aes, aes->iv, AES_DECRYPT);
	state->buffer_size -= aes->buffer_size;
	memcpy(state->buffer, &state->buffer[aes->buffer_size], state->buffer_size);
	if (aes->buffer_size >= AES_BLOCK_SIZE) {
		/*如果缓存中字节数超过last_block需要的字节数的话，
		则把last_block的字节送出去。
		然后把缓存中最后的字节填充到last_block中。
		然后把缓存中剩余的字节送出去。*/
		if (aes->last_block_size > 0){
			MD5_Update(&aes->md5, aes->last_block, aes->last_block_size); /*用送出去的值更新MD5*/
			sz = (*state->write)(aes->last_block,
				aes->last_block_size, state->contentlength, state->write_state);
			if (sz != aes->last_block_size) return -1;
			aes->last_block_size = 0;
		}
		memcpy(aes->last_block,
			&aes->buffer[aes->buffer_size - AES_BLOCK_SIZE],
			AES_BLOCK_SIZE);
		aes->last_block_size = AES_BLOCK_SIZE;
		aes->buffer_size -= aes->last_block_size;
		if (aes->buffer_size > 0) {
			MD5_Update(&aes->md5, aes->buffer, aes->buffer_size); /*用送出去的值更新MD5*/
			sz = (*state->write)(aes->buffer, aes->buffer_size,
				state->contentlength, state->write_state);
			if (sz != aes->buffer_size) return -1;
		}
	}
	else {
		int wsz = aes->buffer_size + aes->last_block_size - AES_BLOCK_SIZE; /*获取补齐last_block后还多余的字节数*/
		if (wsz > 0) { /*如果有多余的数据的话，则送出去*/
			MD5_Update(&aes->md5, aes->last_block, wsz); /*用送出去的值更新MD5*/
			sz = (*state->write)(aes->last_block, wsz, state->contentlength, state->write_state);
			if (sz != wsz) return -1;
			memcpy(aes->last_block, &aes->last_block[wsz], aes->last_block_size - wsz);
			aes->last_block_size -= wsz;
			memcpy(&aes->last_block[aes->last_block_size], aes->buffer, aes->buffer_size);
			aes->last_block_size += aes->buffer_size;
		}
		else {
			/*如果不够last_block需要的字节数，则直接复制到last_block中*/
			memcpy(&aes->last_block[aes->last_block_size], aes->buffer, aes->buffer_size);
			aes->last_block_size += aes->buffer_size;
		}
	}
	return 0;
}

static PcsBool pcs_download_aes_finish(void *userdata)
{
	struct PcsDownloadState *state = (struct PcsDownloadState *)userdata;
	struct pcs *pcs = (struct pcs *)state->handle;
	struct PcsAesState *aes = (struct PcsAesState *)state->userdata;
	int sz, i;
	unsigned char md[PCS_MD5_SIZE], *p;
	if (state->buffer_size != PCS_MD5_SIZE) {
		pcs_set_errmsg(state->handle, "Broken file.");
		return PcsFalse;
	}
	if (aes->last_block) {
		MD5_Update(&aes->md5, aes->last_block, aes->last_block_size - aes->head.polish); /*用送出去的值更新MD5*/
		sz = (*state->write)(aes->last_block, aes->last_block_size - aes->head.polish, state->contentlength, state->write_state);
		if (sz != aes->last_block_size - aes->head.polish) {
			pcs_set_errmsg(state->handle, "Write wrong size data.");
			return PcsFalse;
		}
		aes->last_block_size = 0;
	}
	MD5_Final(md, &aes->md5);
	p = (unsigned char *)state->buffer;
	for (i = 0; i < PCS_MD5_SIZE; i++) {
		if (md[i] != p[i]) {
			pcs_set_errmsg(state->handle, "Wrong secure key or broken file.");
			return PcsFalse;
		}
	}
	return PcsTrue;
}

static void pcs_download_aes_destroy(void *userdata)
{
	struct PcsDownloadState *state = (struct PcsDownloadState *)userdata;
	struct PcsAesState *aes = (struct PcsAesState *)state->userdata;
	destroyPcsAesState(aes);
}

static PcsBool pcs_download_finish(struct PcsDownloadState *state)
{
	struct pcs *pcs = (struct pcs *)state->handle;
	PcsBool rc = PcsTrue;
	if (state->finish) {
		rc = (*state->finish)(state->finish_state);
	}
	else if (state->buffer_size > 0) {
		int l = (*state->write)(state->buffer, state->buffer_size, state->contentlength, state->write_state);
		if (l != state->buffer_size) {
			rc = PcsFalse;
		}
		else
			state->buffer_size = 0;
	}
	return rc;
}

static void pcs_download_destroy(struct PcsDownloadState *state)
{
	if (state->destroy) {
		(*state->destroy)(state->destroy_state);
	}
}

static size_t pcs_download_write_func(char *ptr, size_t size, size_t contentlength, void *userdata)
{
	struct PcsDownloadState *state = (struct PcsDownloadState *)userdata;
	struct pcs *pcs = (struct pcs *)state->handle;
	struct PcsAesState *aes = (struct PcsAesState *)state->userdata;
	char *p = ptr;
	size_t sz = size, l;

	state->contentlength = contentlength;
	while (sz > 0) {
		l = (PCS_BUFFER_SIZE) - state->buffer_size;
		if (l > sz) l = sz;
		memcpy(&state->buffer[state->buffer_size], p, l);
		state->buffer_size += l;
		sz -= l;
		p += l;
		if (!state->secure && state->buffer_size >= AES_BLOCK_SIZE) {
			struct PcsAesHead head = { 0 };
			readToAesHead(state->buffer, &head);
			if (head.magic == PCS_AES_MAGIC
				&& (head.bits == 128
				|| head.bits == 192
				|| head.bits == 256)
				&& head.polish >= 0
				&& head.polish < AES_BLOCK_SIZE) { /*检测到文件被AES加密*/
				aes = createPcsAesState(state->handle, head.bits, AES_DECRYPT, head.polish);
				if (!aes) return 0;
				state->userdata = aes;
				state->finish_state = state;
				state->finish = &pcs_download_aes_finish;
				state->destroy_state = state;
				state->destroy = &pcs_download_aes_destroy;
				switch (head.bits)
				{
				case 128:
					state->secure = PCS_SECURE_AES_CBC_128;
					break;
				case 192:
					state->secure = PCS_SECURE_AES_CBC_192;
					break;
				case 256:
					state->secure = PCS_SECURE_AES_CBC_256;
					break;
				}
				state->buffer_size -= AES_BLOCK_SIZE;
				memcpy(state->buffer, &state->buffer[AES_BLOCK_SIZE], state->buffer_size);
				MD5_Init(&aes->md5);
			}
			else {
				state->secure = PCS_SECURE_PLAINTEXT;
			}
		}
		if (state->secure == PCS_SECURE_AES_CBC_128
			|| state->secure == PCS_SECURE_AES_CBC_192
			|| state->secure == PCS_SECURE_AES_CBC_256) {
			if (pcs_download_aes_process_buffer(state))
				return 0;
		}
		else if (state->secure == PCS_SECURE_PLAINTEXT) {
			l = (*state->write)(state->buffer, state->buffer_size, contentlength, state->write_state);
			if (l != state->buffer_size) return 0;
			state->buffer_size = 0;
		}
		else {
			return 0;
		}
	}
	return size;
}

static PcsRes pcs_download_secure(Pcs handle, const char *path, PcsHttpWriteFunction write, void *write_state)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url;
	struct PcsDownloadState state = { 0 };

	pcs_clear_errmsg(handle);
	if (!write) {
		pcs_set_errmsg(handle, "Please specify the write function.");
		return PCS_FAIL;
	}
	state.handle = handle;
	state.contentlength = 0;
	state.userdata = NULL;
	state.write = write;
	state.write_state = write_state;
	pcs_http_setopts(pcs->http,
		PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, &pcs_download_write_func,
		PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE, &state,
		PCS_HTTP_OPTION_END);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "download",
		"app_id", "250528",
		"path", path,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return PCS_BUILD_URL;
	}
	if (pcs_http_get_download(pcs->http, url, PcsTrue)) {
		pcs_free(url);
		if (!pcs_download_finish(&state)) {
			pcs_download_destroy(&state);
			return PCS_FAIL;
		}
		pcs_download_destroy(&state);
		return PCS_OK;
	}
	pcs_free(url);
	pcs_set_errmsg(handle, "Can't download the file: %s", pcs_http_strerror(pcs->http));
	pcs_download_destroy(&state);
	return PCS_FAIL;
}

static PcsRes pcs_download_normal(Pcs handle, const char *path, PcsHttpWriteFunction write, void *write_state)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url;

	pcs_clear_errmsg(handle);
	if (!write) {
		pcs_set_errmsg(handle, "Please specify the write function.");
		return PCS_FAIL;
	}
	pcs_http_setopts(pcs->http,
		PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, write,
		PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE, write_state,
		PCS_HTTP_OPTION_END);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "download",
		"app_id", "250528",
		"path", path,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return PCS_BUILD_URL;
	}
	if (pcs_http_get_download(pcs->http, url, PcsTrue)) {
		pcs_free(url);
		return PCS_OK;
	}
	pcs_free(url);
	pcs_set_errmsg(handle, "Can't download the file: %s", pcs_http_strerror(pcs->http));
	return PCS_FAIL;
}

PCS_API PcsRes pcs_download(Pcs handle, const char *path)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->secure_enable)
		return pcs_download_secure(handle, path, pcs->download_func, pcs->download_data);
	else
		return pcs_download_normal(handle, path, pcs->download_func, pcs->download_data);
}

static size_t pcs_cat_write_func(char *ptr, size_t size, size_t contentlength, void *userdata)
{
	struct pcs *pcs = (struct pcs *)userdata;
	size_t sz = pcs->buffer_size + size;
	char *buf = (char *)pcs_malloc(sz + 1);
	if (!buf) return 0;
	if (pcs->buffer) {
		memcpy(buf, pcs->buffer, pcs->buffer_size);
		memcpy(&buf[pcs->buffer_size], ptr, size);
		buf[sz] = '\0';
		pcs_free(pcs->buffer);
		pcs->buffer = buf;
		pcs->buffer_size = sz;
	}
	else {
		memcpy(buf, ptr, size);
		buf[sz] = '\0';
		pcs->buffer = buf;
		pcs->buffer_size = sz;
	}
	return size;
}

PCS_API const char *pcs_cat(Pcs handle, const char *path, size_t *dstsz)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes rc;

	if (pcs->buffer) pcs_free(pcs->buffer);
	pcs->buffer = NULL;
	pcs->buffer_size = 0;
	if (pcs->secure_enable) {
		rc = pcs_download_secure(handle, path, &pcs_cat_write_func, pcs);
	}
	else {
		rc = pcs_download_normal(handle, path, &pcs_cat_write_func, pcs);
	}
	if (rc != PCS_OK) {
		return NULL;
	}
	if (dstsz) *dstsz = pcs->buffer_size;
	return pcs->buffer;
}

PCS_API PcsFileInfo *pcs_upload_buffer(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *buffer, size_t buffer_size)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;
	size_t sz = buffer_size;
	char *buf = (char *)buffer;

	pcs_clear_errmsg(handle);
	filename = pcs_utils_filename(path);
	if (pcs->secure_enable
		&& (pcs->secure_method == PCS_SECURE_AES_CBC_128 || pcs->secure_method == PCS_SECURE_AES_CBC_192 || pcs->secure_method == PCS_SECURE_AES_CBC_256)) {
		struct PcsAesState *aes = NULL;
		MD5_CTX md5;
		unsigned char md5_value[PCS_MD5_SIZE];
		MD5_Init(&md5);
		MD5_Update(&md5, buffer, buffer_size);
		MD5_Final(md5_value, &md5);
		// set the encryption length
		sz = 0;
		if (buffer_size % AES_BLOCK_SIZE == 0) {
			sz = buffer_size;
		}
		else {
			sz = (buffer_size / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
		}
		buf = (char *)pcs_malloc(sz + AES_BLOCK_SIZE + PCS_MD5_SIZE);
		if (!buf) {
			pcs_set_errmsg(handle, "Can't alloc buffer for post data.");
			pcs_free(filename);
			return NULL;
		}
		memset(buf, 0, sz + AES_BLOCK_SIZE);
		int2Buffer(PCS_AES_MAGIC, buf);
		int2Buffer(pcs->secure_method, &buf[4]);
		int2Buffer((int)(sz - buffer_size), &buf[8]);
		aes = createPcsAesState(handle, pcs->secure_method, AES_ENCRYPT, (unsigned char)(sz - buffer_size));
		if (!aes) {
			pcs_set_errmsg(handle, "Can't create AES object.");
			pcs_free(filename);
			pcs_free(buf);
			return NULL;
		}
		// encrypt (iv will change)
		AES_cbc_encrypt(buffer, &buf[AES_BLOCK_SIZE], buffer_size, &aes->aes, aes->iv, AES_ENCRYPT);
		sz += AES_BLOCK_SIZE;
		destroyPcsAesState(aes);

		/*复制md5值到最后*/
		memcpy(&buf[sz], md5_value, PCS_MD5_SIZE);
		sz += PCS_MD5_SIZE;
	}
	if (pcs_http_form_addbuffer(pcs->http, &form, "file", buf, (long)sz, filename) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(filename);
		if (buf != buffer) pcs_free(buf);
		return NULL;
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, overwrite, form);
	pcs_http_form_destroy(pcs->http, form);
	if (buf != buffer) pcs_free(buf);
	return meta;
}

size_t pcs_upload_read_func(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	struct PcsUploadState *state = (struct PcsUploadState *) userdata;
	struct PcsAesState *aes = state->aes;
	FILE *file = state->file;
	char *buf = state->buffer;
	size_t sz, buf_size = state->buffer_size;

	if (buf_size == 0) {
		sz = fread(aes->buffer, 1, PCS_BUFFER_SIZE, file);
		if (ferror(file)) {
			return CURL_READFUNC_ABORT;
		}
		if (sz > 0) {
			MD5_Update(&aes->md5, aes->buffer, sz);
			if ((sz % AES_BLOCK_SIZE) != 0) {
				buf_size = (sz / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
				memset(&aes->buffer[sz], 0, buf_size - sz);
			}
			else {
				buf_size = sz;
			}
			// encrypt (iv will change)
			AES_cbc_encrypt(aes->buffer, buf, buf_size, &aes->aes, aes->iv, AES_ENCRYPT);
			state->buffer_size = buf_size;
		}
		else if (!state->eof) {
			MD5_Final(buf, &aes->md5);
			buf_size = state->buffer_size = PCS_MD5_SIZE;
			state->eof = 1;
		}
	}

	if (buf_size > 0) {
		sz = size * nmemb;
		if (sz >= buf_size) {
			sz = buf_size;
			memcpy((char *)ptr, buf, sz);
			memset(buf, 0, PCS_BUFFER_SIZE);
			buf_size = 0;
			state->buffer_size = buf_size;
		}
		else {
			memcpy((char *)ptr, buf, sz);
			buf_size -= sz;
			memcpy(buf, &buf[sz], buf_size);
			state->buffer_size = buf_size;
		}
	}
	else {
		sz = 0;
	}
	return sz;
}

PCS_API PcsFileInfo *pcs_upload(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *local_filename)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;
	struct PcsUploadState state = { 0 };
	struct PcsAesState *aes = NULL;

	pcs_clear_errmsg(handle);
	filename = pcs_utils_filename(path);
	if (pcs->secure_enable 
		&& (pcs->secure_method == PCS_SECURE_AES_CBC_128 
		|| pcs->secure_method == PCS_SECURE_AES_CBC_192 
		|| pcs->secure_method == PCS_SECURE_AES_CBC_256)) {
		size_t file_size = 0, sz = 0;
		state.file = fopen(local_filename, "rb");
		if (state.file) {
			fseek(state.file, 0, SEEK_END);
			file_size = ftell(state.file);
			fseek(state.file, 0, SEEK_SET);
		}
		else {
			pcs_set_errmsg(handle, "Can't open the file.");
			pcs_free(filename);
			return NULL;
		}
		if (file_size % AES_BLOCK_SIZE == 0) {
			sz = file_size;
		}
		else {
			sz = (file_size / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
		}
		memset(state.buffer, 0, AES_BLOCK_SIZE);
		int2Buffer(PCS_AES_MAGIC, state.buffer);
		int2Buffer(pcs->secure_method, &state.buffer[4]);
		int2Buffer((int)(sz - file_size), &state.buffer[8]);
		state.buffer_size = AES_BLOCK_SIZE;
		state.handle = handle;
		state.contentlength = sz + AES_BLOCK_SIZE;
		state.secure = pcs->secure_method;
		aes = createPcsAesState(handle, pcs->secure_method, AES_ENCRYPT, (unsigned char)(sz - file_size));
		if (!aes) {
			pcs_set_errmsg(handle, "Can't create AES object.");
			pcs_free(filename);
			if (state.file) fclose(state.file);
			return NULL;
		}
		state.aes = aes;
		MD5_Init(&aes->md5);

		if (pcs_http_form_addbufferfile(pcs->http, &form, "file", filename, &pcs_upload_read_func, &state, sz + AES_BLOCK_SIZE + PCS_MD5_SIZE) != PcsTrue) {
			pcs_set_errmsg(handle, "Can't build the post data.");
			pcs_free(filename);
			destroyPcsAesState(aes);
			if (state.file) fclose(state.file);
			return NULL;
		}

	}
	else {
		if (pcs_http_form_addfile(pcs->http, &form, "file", local_filename, filename) != PcsTrue) {
			pcs_set_errmsg(handle, "Can't build the post data.");
			pcs_free(filename);
			return NULL;
		}
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, overwrite, form);
	pcs_http_form_destroy(pcs->http, form);
	if (aes) destroyPcsAesState(aes);
	if (state.file) fclose(state.file);
	return meta;
}

PCS_API char *pcs_cookie_data(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_http_cookie_data(pcs->http);
}

PCS_API const char *pcs_req_rawdata(Pcs handle, int *size, const char **encode)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_http_rawdata(pcs->http, size, encode);
}
