﻿#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef WIN32
# include <malloc.h>
# include "openssl_md5.h"
#else
# include <alloca.h>
# include <openssl/md5.h>
#endif

#include "pcs_defs.h"
#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_http.h"
#include "cJSON.h"
#include "pcs.h"

#ifdef WIN32
# define lseek _lseek
# define fileno _fileno
# define fseeko _fseeki64
# define ftello _ftelli64
#endif

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
#define URL_GET_PUBLIC_KEY	"https://passport.baidu.com/v2/getpublickey?"
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
	char		*public_key;
	char		*key;

	PcsGetCaptchaFunction		captcha_func;
	void						*captcha_data;

	PcsHttp		http;

	PcsHttpWriteFunction	download_func;
	void					*download_data;

	char		*buffer;
	size_t		buffer_size;
};

#define PCS_BUFFER_SIZE			(16 * 1024)
#define PCS_ACTION_NONE			0
#define PCS_ACTION_DOWNLOAD		1
#define PCS_ACTION_UPLOAD		2

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

    memset(captcha, 0, captchaSize);
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
		fi->fs_id = (uint64_t)val->valuedouble;

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
		fi->size = (int64_t)val->valuedouble;

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
			fi->block_list = (char **) pcs_malloc((cnt + 1) * sizeof(char *));
			if (!fi->block_list) return fi;
			memset(fi->block_list, 0, (cnt + 1) * sizeof(char *));
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
	const char *errmsg;
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
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
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
	const char *errmsg;
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
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
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

static PcsFileInfo *pcs_upload_form(Pcs handle, const char *path, PcsHttpForm form, const char *ondup)
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
		"ondup", ondup,
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
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
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

static PcsFileInfo *pcs_upload_slice_form(Pcs handle, PcsHttpForm form)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *html;
	cJSON *json, *item;
	PcsFileInfo *meta;

	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "upload",
		"type", "tmpfile",
		"app_id", "250528",
		"BDUSS", pcs->bduss,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}
	html = pcs_post_httpform(pcs->http, url, form, PcsTrue);
	pcs_free(url);
	if (!html) {
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	/*{
		"path": "\/temp\/putty-0.63-installer.exe",
		"size": 1869122,
		"ctime": 1420193926,
		"mtime": 1420193926,
		"md5": "18bd0948d254894441dd6f818d9b3811",
		"fs_id": 919365701714983,
		"isdir": 0,
		"request_id": 2401283243
	}*/
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

/*移除字符串中的换行符*/
//static char * remove_enter(char *str)
//{
//	char *p1, *p2;
//	p1 = p2 = str;
//	for (p1 = p2 = str; *p1; p1++) {
//		if (*p1 != '\n') {
//			*p2++ = *p1;
//		}
//	}
//	*p2 = '\0';
//	return str;
//}

/**
* 登录时，转换字节串为其base64编码
* Use EVP to Base64 encode the input byte array to readable text
*/
//static char* base64(const char *inputBuffer, int inputLen)
//{
//	EVP_ENCODE_CTX  ctx;
//	int base64Len = (((inputLen + 2) / 3) * 4) + 1; // Base64 text length  
//	int pemLen = base64Len + base64Len / 64; // PEM adds a newline every 64 bytes  
//	char* base64 = (char *)pcs_malloc(pemLen + 1);
//	int result;
//	EVP_EncodeInit(&ctx);
//	EVP_EncodeUpdate(&ctx, (unsigned char *)base64, &result, (unsigned char *)inputBuffer, inputLen);
//	EVP_EncodeFinal(&ctx, (unsigned char *)&base64[result], &result);
//	return remove_enter(base64);
//}

/*登录时编码密码密文中的符号*/
//static char * escape_symbol(const char *buf)
//{
//	static char tb[] = "0123456789ABCDEF";
//	const char *p = buf;
//	char *tmp, *np;
//	int newLen = 0;
//#define IS_SYMBOL(p) (*p == '#' || *p == '%' || *p == '&' || *p == '+' || *p == '=' || *p == '/' || *p == '\\' || *p == ' ' || *p == '\f' || *p == '\r' || *p == '\n' || *p == '\t')
//	while (*p) {
//		if (IS_SYMBOL(p))
//			newLen += 3;
//		else
//			newLen++;
//		p++;
//	}
//	p = buf;
//	tmp = (char *)pcs_malloc(newLen + 1);
//	np = tmp;
//	while (*p) {
//		if (IS_SYMBOL(p)) {
//			np[0] = '%';
//			np[1] = tb[(((int)(*p)) >> 4) & 0xF];
//			np[2] = tb[(((int)(*p))) & 0xF];
//			np += 3;
//		}
//		else {
//			*np = *p;
//			np++;
//		}
//		p++;
//	}
//	*np = '\0';
//	return tmp;
//}

/*登录时加密用户密码*/
//static char *rsa_encrypt(const char *str, const char *pub_key){
//	char *p_en, *b64;
//	int rsa_len;
//	BIO *bufio;
//	RSA *rsa = NULL;
//
//	bufio = BIO_new_mem_buf((void *)(char *)pub_key, -1);
//	PEM_read_bio_RSA_PUBKEY(bufio, &rsa, NULL, NULL);
//	if (rsa == NULL){
//		ERR_print_errors_fp(stdout);
//		return NULL;
//	}
//	rsa_len = RSA_size(rsa);
//	p_en = (unsigned char *)pcs_malloc(rsa_len + 1);
//	memset(p_en, 0, rsa_len + 1);
//	if (RSA_public_encrypt(rsa_len, (unsigned char *)str, (unsigned char*)p_en, rsa, RSA_NO_PADDING)<0){
//		return NULL;
//	}
//	RSA_free(rsa);
//
//	b64 = base64(p_en, rsa_len);
//	pcs_free(p_en);
//	return b64;
//}

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
	if (pcs->public_key)
		pcs_free(pcs->public_key);
	if (pcs->key)
		pcs_free(pcs->key);
	if (pcs->buffer)
		pcs_free(pcs->buffer);
	pcs_free(pcs);
}

PCS_API void pcs_clone_userinfo(Pcs dst, Pcs src)
{
	struct pcs *pcs_dst = (struct pcs *)dst;
	struct pcs *pcs_src = (struct pcs *)src;
	if (pcs_dst->username) {
		pcs_free(pcs_dst->username);
		pcs_dst->username = NULL;
	}
	if (pcs_src->username)
		pcs_dst->username = pcs_utils_strdup(pcs_src->username);

	if (pcs_dst->password) {
		pcs_free(pcs_dst->password);
		pcs_dst->password = NULL;
	}
	if (pcs_src->password)
		pcs_dst->password = pcs_utils_strdup(pcs_src->password);

	if (pcs_dst->bdstoken) {
		pcs_free(pcs_dst->bdstoken);
		pcs_dst->bdstoken = NULL;
	}
	if (pcs_src->bdstoken)
		pcs_dst->bdstoken = pcs_utils_strdup(pcs_src->bdstoken);

	if (pcs_dst->bduss) {
		pcs_free(pcs_dst->bduss);
		pcs_dst->bduss = NULL;
	}
	if (pcs_src->bduss)
		pcs_dst->bduss = pcs_utils_strdup(pcs_src->bduss);

	if (pcs_dst->sysUID) {
		pcs_free(pcs_dst->sysUID);
	}
	if (pcs_src->sysUID)
		pcs_dst->sysUID = pcs_utils_strdup(pcs_src->sysUID);

	//if (pcs_dst->errmsg) {
	//	pcs_free(pcs_dst->errmsg);
	//	pcs_dst->errmsg = NULL;
	//}
	//if (pcs_src->errmsg)
	//	pcs_dst->errmsg = pcs_utils_strdup(pcs_src->errmsg);

	if (pcs_dst->public_key) {
		pcs_free(pcs_dst->public_key);
		pcs_dst->public_key = NULL;
	}
	if (pcs_src->public_key)
		pcs_dst->public_key = pcs_utils_strdup(pcs_src->public_key);

	if (pcs_dst->key) {
		pcs_free(pcs_dst->key);
		pcs_dst->key = NULL;
	}
	if (pcs_src->key)
		pcs_dst->key = pcs_utils_strdup(pcs_src->key);
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
	case PCS_OPTION_USAGE:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_USAGE, value);
		break;
	default:
		pcs_set_errmsg(handle, "Unknown option");
		res = PCS_UNKNOWN_OPT;
		break;
	case PCS_OPTION_TIMEOUT:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_TIMEOUT, value);
		break;
	case PCS_OPTION_CONNECTTIMEOUT:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_CONNECTTIMEOUT, value);
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
	const char *html, *errmsg;
	int http_code;

	pcs_clear_errmsg(handle);
	html = pcs_http_get(pcs->http, URL_DISK_HOME, PcsFalse);
	http_code = pcs_http_code(pcs->http);
	if (http_code != 200) {
		if (http_code != 302) {
			errmsg = pcs_http_strerror(pcs->http);
			if (errmsg)
				pcs_set_errmsg(handle, errmsg);
			else
				pcs_set_errmsg(handle, "The server response wrong http code.");
			return PCS_NETWORK_ERROR;
		}
		return PCS_NOT_LOGIN;
	}
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
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
	char *p, *html, *url, *token, *code_string, captch[8], *post_data, *tt, *codetype, *rsa_pwd = NULL/*, *ptmp*/;
	cJSON *json, *root, *item;
	int error = -1, i;
	const char *errmsg;

	pcs_clear_errmsg(handle);

	html = pcs_http_get(pcs->http, URL_HOME, PcsTrue);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}

	//html = pcs_http_get(pcs->http, URL_PASSPORT_API "login", PcsTrue);
	//if (!html) {
	//	errmsg = pcs_http_strerror(pcs->http);
	//	if (errmsg)
	//		pcs_set_errmsg(handle, errmsg);
	//	else
	//		pcs_set_errmsg(handle, "Can't get response from the remote server.");
	//	return PCS_NETWORK_ERROR;
	//}

	url = pcs_utils_sprintf(URL_PASSPORT_API "getapi" "&tpl=netdisk" "&apiver=v3" "&tt=%d" "&class=login" "&logintype=basicLogin" "&callback=bd__cbs__pwxtn7",
		(int)time(0));
	html = pcs_http_get(pcs->http, url, PcsTrue); pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}
	json = cJSON_Parse(extract_json_from_callback(html));
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

	url = pcs_utils_sprintf(URL_PASSPORT_API "logincheck" "&token=%s" "&tpl=netdisk" "&apiver=v3" "&tt=%d" "&username=%s" "&isphone=false" "&callback=bd__cbs__q4ztud",
		token, (int)time(0), pcs->username);
	html = pcs_http_get(pcs->http, url, PcsTrue); pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		pcs_free(token);
		pcs_free(code_string);
		return PCS_NETWORK_ERROR;
	}
	json = cJSON_Parse(extract_json_from_callback(html));
	if (!json){
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		pcs_free(token);
		pcs_free(code_string);
		return PCS_WRONG_RESPONSE;
	}
	root = cJSON_GetObjectItem(json, "data");
	if (!root) {
		pcs_set_errmsg(handle, "Can't read res.data. Response: %s", html);
		cJSON_Delete(json);
		pcs_free(token);
		pcs_free(code_string);
		return PCS_WRONG_RESPONSE;
	}
	item = cJSON_GetObjectItem(root, "codeString");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.codeString. Response: %s", html);
		pcs_free(token);
		pcs_free(code_string);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	pcs_free(code_string);
	code_string = pcs_utils_strdup(item->valuestring);

	item = cJSON_GetObjectItem(root, "vcodetype");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.vcodetype. Response: %s", html);
		pcs_free(token);
		pcs_free(code_string);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	codetype = pcs_utils_strdup(item->valuestring);
	cJSON_Delete(json);

	url = pcs_utils_sprintf(URL_GET_PUBLIC_KEY  "&token=%s" "&tpl=netdisk" "&apiver=v3" "&tt=%d" "&callback=bd__cbs__wl95ks",
		token, (int)time(0));
	html = pcs_http_get(pcs->http, url, PcsTrue); pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(codetype);
		return PCS_NETWORK_ERROR;
	}
	json = cJSON_Parse(extract_json_from_callback(html));
	if (!json){
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(codetype);
		return PCS_WRONG_RESPONSE;
	}
	item = cJSON_GetObjectItem(json, "pubkey");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.pubkey. Response: %s", html);
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(codetype);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (pcs->public_key) pcs_free(pcs->public_key);
	pcs->public_key = pcs_utils_strdup(item->valuestring);

	item = cJSON_GetObjectItem(json, "key");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.key. Response: %s", html);
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(codetype);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (pcs->key) pcs_free(pcs->key);
	pcs->key = pcs_utils_strdup(item->valuestring);
	cJSON_Delete(json);

	i = 0;
	/*使用RSA加密密码后提交数据，测试一直是密码错误，以后再搞吧...*/
	//ptmp = rsa_encrypt(pcs->password, pcs->public_key);
	//rsa_pwd = escape_symbol(ptmp);
	//pcs_free(ptmp);
try_login:
	if (code_string[0]) {
		res = pcs_get_captcha(pcs, code_string, captch, sizeof(captch));
		if (res != PCS_OK) {
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(codetype);
			if (rsa_pwd) pcs_free(rsa_pwd);
			return res;
		}
	}
	tt = pcs_utils_sprintf("%d", (int)time(0));
	post_data = pcs_http_build_post_data(pcs->http,
		"staticpage", "http://pan.baidu.com/res/static/thirdparty/pass_v3_jump.html",
		"charset", "utf-8",
		"token", token,
		"tpl", "netdisk",
		"subpro", "",
		"apiver", "v3",
		"tt", tt,
		"codestring", code_string,
		"safeflg", "0",
		"u", "http://pan.baidu.com/",
		"isPhone", "",
		"quick_user", "0",
		"logintype", "basicLogin",
		"logLoginType", "pc_loginBasic",
		"idc", "",
		"loginmerge", "true",
		"username", pcs->username,
		"password", rsa_pwd ? rsa_pwd : pcs->password,
		"verifycode", captch,
		"mem_pass", "on",
		"rsakey", rsa_pwd ? pcs->key : "", 
		"crypttype", rsa_pwd ? "12" : "",
		"ppui_logintime", "2602",
		"callback", "parent.bd__pcbs__msdlhs",
		NULL);
	pcs_free(tt);
	if (!post_data) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(codetype);
		if (rsa_pwd) pcs_free(rsa_pwd);
		return PCS_BUILD_POST_DATA;
	}
	html = pcs_http_post(pcs->http, URL_PASSPORT_API "login", post_data, PcsTrue);
	pcs_free(post_data);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(codetype);
		if (rsa_pwd) pcs_free(rsa_pwd);
		return PCS_NETWORK_ERROR;
	}
	else {
		char *errorStr = pcs_get_embed_query_int_value_by_key(html, "&error");
		if (!errorStr) errorStr = pcs_get_embed_query_int_value_by_key(html, "err_no");
		if (!errorStr) {
			pcs_set_errmsg(handle, "Can't read the error from the response. Response: %s", html);
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(codetype);
			if (rsa_pwd) pcs_free(rsa_pwd);
			return PCS_NETWORK_ERROR;
		}
		error = atoi(errorStr);
		pcs_free(errorStr);
	}

	if (error == 0) {
		if (pcs_islogin(pcs) == PCS_LOGIN) {
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(codetype);
			if (rsa_pwd) pcs_free(rsa_pwd);
			return PCS_OK;
		}
		else {
			pcs_set_errmsg(handle, "Unknown Error");
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(codetype);
			if (rsa_pwd) pcs_free(rsa_pwd);
			return PCS_FAIL;
		}
	}
	else if (error == 4) {
		pcs_set_errmsg(handle, "Wrong password");
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(codetype);
		if (rsa_pwd) pcs_free(rsa_pwd);
		return PCS_FAIL;
	}
	else {
		if (code_string) pcs_free(code_string);
		code_string = pcs_get_embed_query_token_by_key(html, "&codestring");
		if (!code_string)
			code_string = pcs_get_embed_query_token_by_key(html, "&codeString");
		if (!code_string) {
			pcs_set_errmsg(handle, "Can't read the codestring from the response. Response: %s", html);
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(codetype);
			if (rsa_pwd) pcs_free(rsa_pwd);
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
	pcs_free(codetype);
	if (rsa_pwd) pcs_free(rsa_pwd);
	pcs_set_errmsg(handle, "Unknown Error");
	return PCS_FAIL;
}

PCS_API PcsRes pcs_logout(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	int http_code;
	const char *errmsg;

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
	errmsg = pcs_http_strerror(pcs->http);
	if (errmsg)
		pcs_set_errmsg(handle, errmsg);
	else
		pcs_set_errmsg(handle, "Can't logout. Http Code: %d", http_code);
	return PCS_FAIL;
}

PCS_API PcsRes pcs_quota(Pcs handle, int64_t *quota, int64_t *used)
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
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
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
		*quota = (int64_t)item->valuedouble;

	item = cJSON_GetObjectItem(json, "used");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.used. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (used)
		*used = (int64_t)item->valuedouble;

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
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
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

static PcsRes pcs_download_normal(Pcs handle, const char *path, PcsHttpWriteFunction write, void *write_state, curl_off_t max_speed, curl_off_t resume_from)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url;
	const char *errmsg;

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
	if (pcs_http_get_download(pcs->http, url, PcsTrue, max_speed, resume_from)) {
		pcs_free(url);
		return PCS_OK;
	}
	pcs_free(url);
	errmsg = pcs_http_strerror(pcs->http);
	if (errmsg)
		pcs_set_errmsg(handle, errmsg);
	else
		pcs_set_errmsg(handle, "Can't download the file: %s", pcs_http_strerror(pcs->http));
	return PCS_FAIL;
}

PCS_API PcsRes pcs_download(Pcs handle, const char *path, curl_off_t max_speed, curl_off_t resume_from)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_download_normal(handle, path, pcs->download_func, pcs->download_data, max_speed, resume_from);
}

PCS_API int64_t pcs_get_download_filesize(Pcs handle, const char *path)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url;
	const char *errmsg;
	int64_t size;

	pcs_clear_errmsg(handle);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "download",
		"app_id", "250528",
		"path", path,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return 0;
	}
	size = pcs_http_get_download_filesize(pcs->http, url, PcsTrue);
	pcs_free(url);
	errmsg = pcs_http_strerror(pcs->http);
	if (errmsg)
		pcs_set_errmsg(handle, errmsg);
	return size;
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
	rc = pcs_download_normal(handle, path, &pcs_cat_write_func, pcs, 0, 0);
	if (rc != PCS_OK) {
		return NULL;
	}
	if (dstsz) *dstsz = pcs->buffer_size;
	return pcs->buffer;
}

PCS_API PcsFileInfo *pcs_upload_buffer(Pcs handle, const char *path, PcsBool overwrite, const char *buffer, size_t buffer_size)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;
	size_t sz = buffer_size;
	char *buf = (char *)buffer;

	pcs_clear_errmsg(handle);
	filename = pcs_utils_filename(path);
	if (pcs_http_form_addbuffer(pcs->http, &form, "file", buf, (long)sz, filename) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(filename);
		if (buf != buffer) pcs_free(buf);
		return NULL;
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, form, overwrite ? "overwrite" : "newcopy");
	pcs_http_form_destroy(pcs->http, form);
	if (buf != buffer) pcs_free(buf);
	return meta;
}

PCS_API PcsFileInfo *pcs_upload_slice(Pcs handle, const char *buffer, size_t buffer_size)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;
	size_t sz = buffer_size;
	char *buf = (char *)buffer;

	pcs_clear_errmsg(handle);
	if (pcs_http_form_addbuffer(pcs->http, &form, "file", buf, (long)sz, NULL) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		if (buf != buffer) pcs_free(buf);
		return NULL;
	}
	meta = pcs_upload_slice_form(handle, form);
	pcs_http_form_destroy(pcs->http, form);
	if (buf != buffer) pcs_free(buf);
	return meta;
}

PCS_API PcsFileInfo *pcs_upload_slicefile(Pcs handle, 
	size_t(*read_func)(void *ptr, size_t size, size_t nmemb, void *userdata),
	void *userdata,
	size_t content_size)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;

	pcs_clear_errmsg(handle);
	if (pcs_http_form_addbufferfile(pcs->http, &form, "file", NULL, read_func, userdata, content_size) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		return NULL;
	}
	meta = pcs_upload_slice_form(handle, form);
	pcs_http_form_destroy(pcs->http, form);
	return meta;
}

PCS_API PcsFileInfo *pcs_create_superfile(Pcs handle, const char *path, PcsBool overwrite, PcsSList *block_list)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *postdata, *block_list_data, *html;
	const char *errmsg;
	cJSON *json, *item;
	PcsFileInfo *meta;

	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "createsuperfile",
		"app_id", "250528",
		"path", path,
		"ondup", overwrite ? "overwrite" : "newcopy",
		"BDUSS", pcs->bduss,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}
	block_list_data = pcs_build_filelist_1(handle, block_list, NULL);
	if (!block_list_data) {
		pcs_set_errmsg(handle, "Can't build the block list.");
		pcs_free(url);
		return NULL;
	}
	postdata = (char *)pcs_malloc(strlen(block_list_data) + 32);
	strcpy(postdata, "{\"block_list\":");
	strcat(postdata, block_list_data);
	strcat(postdata, "}");
	pcs_free(block_list_data);
	block_list_data = postdata;

	postdata = pcs_http_build_post_data(pcs->http, "param", block_list_data, NULL);
	pcs_free(block_list_data);
	if (!postdata) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(url);
		return NULL;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(postdata);
	pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	/*{
		"path": "\/temp\/88.zip",
		"size": "1982432",
		"ctime": 1420209233,
		"mtime": 1420209233,
		"md5": "3058b1325c93ba9f24cb7a7ebbc03181",
		"fs_id": 214609865781387,
		"isdir": 0,
		"block_list": ["3058b1325c93ba9f24cb7a7ebbc03181"],
		"s3_handle": "3058b1325c93ba9f24cb7a7ebbc03181",
		"request_id": 1835246947
	}*/
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

PCS_API PcsFileInfo *pcs_upload(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *local_filename)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;

	pcs_clear_errmsg(handle);
	filename = pcs_utils_filename(path);
	if (pcs_http_form_addfile(pcs->http, &form, "file", local_filename, filename) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(filename);
		return NULL;
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, form, overwrite ? "overwrite" : "newcopy");
	pcs_http_form_destroy(pcs->http, form);
	return meta;
}

PCS_API int64_t pcs_local_filesize(Pcs handle, const char *path)
{
	FILE *pf;
	int64_t sz = 0;
	pcs_clear_errmsg(handle);
	pf = fopen(path, "rb");
	if (!pf) {
		pcs_set_errmsg(handle, "Can't open the file. %s", path);
		return -1;
	}
	if (fseeko(pf, 0, SEEK_END)) {
		pcs_set_errmsg(handle, "fseeko() error. %s", path);
		fclose(pf);
		return -1;
	}
	sz = ftello(pf);
	fclose(pf);
	if (sz < 0) {
		pcs_set_errmsg(handle, "ftello() error. %s", path);
		return -1;
	}
	return sz;
}

PCS_API PcsBool pcs_md5_file(Pcs handle, const char *path, char *md5_buf)
{
	MD5_CTX md5;
	FILE *pf;
	size_t sz;
	unsigned char buf[PCS_BUFFER_SIZE];
	unsigned char md5_value[16];
	int i;

	pcs_clear_errmsg(handle);
	pf = fopen(path, "rb");
	if (!pf) {
		pcs_set_errmsg(handle, "Can't open the file. %s", path);
		return PcsFalse;
	}

	MD5_Init(&md5);
	while ((sz = fread(buf, 1, PCS_BUFFER_SIZE, pf)) > 0) {
		MD5_Update(&md5, buf, sz);
	}
	MD5_Final(md5_value, &md5);
	fclose(pf);
	for (i = 0; i < 16; i++) {
		sprintf(&md5_buf[i * 2], "%02x", md5_value[i]);
	}
	return PcsTrue;
}

PCS_API PcsBool pcs_md5_file_slice(Pcs handle, const char *path, int64_t offset, int64_t length, char *md5_buf)
{
	MD5_CTX md5;
	FILE *pf;
	size_t sz;
	int64_t hashed_sz = 0;
	unsigned char buf[PCS_BUFFER_SIZE];
	unsigned char md5_value[16];
	int i;

	pcs_clear_errmsg(handle);
	pf = fopen(path, "rb");
	if (!pf) {
		pcs_set_errmsg(handle, "Can't open the file. %s", path);
		return PcsFalse;
	}

	if (fseeko(pf, offset, SEEK_SET)) {
		pcs_set_errmsg(handle, "fseeko() error. %s", path);
		fclose(pf);
		return PcsFalse;
	}

	MD5_Init(&md5);
	while ((hashed_sz < length) && ((sz = fread(buf, 1, PCS_BUFFER_SIZE, pf)) > 0)) {
		if (hashed_sz + sz > length) {
			MD5_Update(&md5, buf, (size_t)(length - hashed_sz));
			hashed_sz += (length - hashed_sz);
		}
		else {
			MD5_Update(&md5, buf, sz);
			hashed_sz += sz;
		}
	}
	MD5_Final(md5_value, &md5);
	fclose(pf);
	for (i = 0; i < 16; i++) {
		sprintf(&md5_buf[i * 2], "%02x", md5_value[i]);
	}
	return PcsTrue;
}

PCS_API PcsFileInfo *pcs_rapid_upload(Pcs handle, const char *path, PcsBool overwrite, const char *local_filename, char *out_content_md5, char *out_slice_md5)
{
	struct pcs *pcs = (struct pcs *)handle;
	int64_t content_length;
	char content_md5[33] = { 0 }, slice_md5[33] = { 0 }, *content_length_str, *dir, *filename;
	char *url, *html;
	int http_code;
	const char *errmsg;
	cJSON *json, *item;
	PcsFileInfo *meta;

	pcs_clear_errmsg(handle);
	content_length = pcs_local_filesize(handle, local_filename);
	if (content_length < 0) {
		return NULL;
	}
	if (content_length <= PCS_RAPIDUPLOAD_THRESHOLD) {
		pcs_set_errmsg(handle, "The file size is not satisfied, the file must be great than %d.", PCS_RAPIDUPLOAD_THRESHOLD);
		return NULL;
	}

	if (!pcs_md5_file(handle, local_filename, content_md5)) {
		return NULL;
	}
	if (out_content_md5) strcpy(out_content_md5, content_md5);

	if (!pcs_md5_file_slice(handle, local_filename, 0, PCS_RAPIDUPLOAD_THRESHOLD, slice_md5)) {
		return NULL;
	}
	if (out_slice_md5) strcpy(out_slice_md5, slice_md5);

	dir = pcs_utils_basedir(path);
	filename = pcs_utils_filename(path);
	content_length_str = pcs_utils_sprintf("%"PRId64, content_length);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "rapidupload",
		"app_id", "250528",
		"ondup", overwrite ? "overwrite" : "newcopy",
		"dir", dir,
		"filename", filename,
		"content-length", content_length_str,
		"content-md5", content_md5,
		"slice-md5", slice_md5,
		"path", path,
		"BDUSS", pcs->bduss,
		"bdstoken", pcs->bdstoken,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		pcs_free(content_length_str);
		pcs_free(dir);
		pcs_free(filename);
		return NULL;
	}
	pcs_free(content_length_str);
	pcs_free(dir);
	pcs_free(filename);

	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	http_code = pcs_http_code(pcs->http);
	if (http_code == 404)
		return NULL;
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_errmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	/*{
		"path": "\/temp\/putty-0.63-installer.exe",
		"size": 1869122,
		"ctime": 1420193926,
		"mtime": 1420193926,
		"md5": "18bd0948d254894441dd6f818d9b3811",
		"fs_id": 919365701714983,
		"isdir": 0,
		"request_id": 2401283243
	}*/
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

PCS_API double pcs_speed_download(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_http_speed_download(pcs->http);
}
