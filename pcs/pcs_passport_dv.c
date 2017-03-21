#include "pcs_defs.h"
#include "pcs_passport_dv.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>

#include "pcs_defs.h"
#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_buffer.h"
#include "utf8.h"

#define BROWSER_UNKNOW			-1
#define BROWSER_MSIE			1
#define BROWSER_CHROME			2
#define BROWSER_FIREFOX			3
#define BROWSER_360SE			4
#define BROWSER_360EE			5
#define BROWSER_OPERA			6
#define BROWSER_SOGOU			7
#define BROWSER_QIHU_THEWORLD	8 /*世界之窗 */
#define BROWSER_TENCENT_TRAVELER 9
#define BROWSER_QQ_BROWSER		10
#define BROWSER_SAFARI			11
#define BROWSER_MAXTHON			12

#define OS_UNKNOW	0
#define OS_WIN		1
#define OS_LINUX	2
#define OS_MAC		3
#define OS_IPHONE	4
#define OS_IPOD		5
#define OS_IPAD		6

#define DV_MAGIC_TOOLBAR	0x102
#define DV_MAGIC_SLOWSCRIPT	0x202 /* 记录慢脚本 */
#define DV_MAGIC_BROWSER	0x402
#define DV_MAGIC_INIT		0x502
#define DV_MAGIC_SCREENINFO	0x602
#define DV_MAGIC_TESTUTILS	0x702 /* 是否使用了一些自动测试工具，例如：phantom, webdriver 等*/
#define DV_MAGIC_MOUSEMOVE	0x802 /* 鼠标事件 (包含经过的毫秒数) */
#define DV_MAGIC_MOUSEDOWN	0x902 /* 鼠标事件 (包含经过的毫秒数) */
#define DV_MAGIC_TOUCHSTART	0xa02 /* touchstart 事件 */
#define DV_MAGIC_KEYDOWN	0xc02
#define DV_MAGIC_FCEVENT	0xd02 /* 焦点事件 (包含经过的毫秒数) */
#define DV_MAGIC_PCMAC		0x1002
#define DV_MAGIC_LOCATION	0x1302
#define DV_MAGIC_TIME		0x1502 /* 此 js 启动时间 */
#define DV_MAGIC_PAGETOKEN	0x1602
#define DV_MAGIC_VERIFY		0x1702 /*作用未知，可能用于验证*/

#define MAX_TESTUTILS		3
#define MAX_MOUSEMOVE		5
#define MAX_MOUSEDOWN		5
#define MAX_KEYDOWN			5
#define MAX_FOCUS			5

#define DV_BROWSER			BROWSER_CHROME
#define DV_BROWSER_VERSION	"57"			/* chrome version 57 */
#define DV_OS				OS_WIN

#define DV_SCREEN_TOP			0
#define DV_SCREEN_LEFT			0
#define DV_CLIENT_WIDTH			1349
#define DV_CLIENT_HEIGHT		716
#define DV_SCREEN_WIDTH			1366
#define DV_SCREEN_HEIGHT		768
#define DV_SCREEN_AVAIL_WIDTH	1366
#define DV_SCREEN_AVAIL_HEIGHT	738
#define DV_OUTER_WIDTH			1366
#define DV_OUTER_HEIGHT			738

#define DV_KEY_SHIFT		16
#define DV_KEY_2			50
#define DV_KEY_EQUALSIGN	187
#define DV_KEY_DASH			189
#define DV_KEY_PERIOD		190
#define DV_KEY_PLACEHOLDER	229

#define DV_CKEY_NONE	0
#define DV_CKEY_CTRL	1
#define DV_CKEY_ALT		2
#define DV_CKEY_SHIFT	4

#define PCS_OK		0
#define PCS_EINVAL	-1
#define PCS_ENOMEM	-2

#define pcs_error(e)	(e)

typedef struct pkg_t {
	pcs_buf_t a;
	int pc;
	int bol;

	int keydown;
	int mousemove;
	int mousedown;
	int focus;
} pkg_t;

typedef struct jskey_t {
	int keycode;
	int flags;
} jskey_t;

static char *passport_base64(const char *input, int length)
{
	static const char *s_cm = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
	int len, b;
	char *buf, *dst;
	const char *p;

	if (length == -1) length = strlen(input);

	len = length / 3;
	len *= 4;
	b = length % 3;
	if (b > 1)
		len += 3;
	else if (b > 0)
		len += 2;

	len += 4; /* for first 'MDEw' */

	buf = (char *)pcs_malloc(len + 1);
	if (!buf) {
		return NULL;
	}

	dst = buf;
	strcpy(dst, "MDEw");
	dst += 4;

	p = input;

	while (length >= 3) {
		b = ((*(p++)) & 0xff); b <<= 8;
		b |= ((*(p++)) & 0xff); b <<= 8;
		b |= ((*(p++)) & 0xff);
		*(dst++) = s_cm[(b & 0xfc0000) >> 18];
		*(dst++) = s_cm[(b & 0x3f000) >> 12];
		*(dst++) = s_cm[(b & 0xfc0) >> 6];
		*(dst++) = s_cm[(b & 0x3f)];
		length -= 3;
	}

	if (length > 0) {
		b = ((*(p++)) & 0xff); b <<= 8;
		if (length > 1)
			b |= ((*(p++)) & 0xff);
		b <<= 8;

		*(dst++) = s_cm[(b & 0xfc0000) >> 18];
		*(dst++) = s_cm[(b & 0x3f000) >> 12];
		if (length > 1)
			*(dst++) = s_cm[(b & 0xfc0) >> 6];
	}
	*(dst++) = '\0';
	return buf;
}

static char *passport_debase64(char **pbuf, int *plen, const char *input, int length)
{
	static const char s_rcm[256] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,                52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,
		0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,            15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,63,
		0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};

	int len, b;
	char *buf, *dst;
	const char *p;

	if (length == -1) length = strlen(input);

	len = length / 4;
	len *= 3;
	b = length % 4;
	if (b > 2)
		len += 2;
	else if (b > 0)
		len++;

	buf = (char *)pcs_malloc(len + 1);
	if (!buf) {
		return NULL;
	}

	buf[len] = '\0';

	if (pbuf) *pbuf = buf;
	if (plen) *plen = len;

	dst = buf;
	p = input;

	while (length >= 4) {
		b = s_rcm[(*(p++)) & 0xff]; b <<= 6;
		b |= s_rcm[(*(p++)) & 0xff]; b <<= 6;
		b |= s_rcm[(*(p++)) & 0xff]; b <<= 6;
		b |= s_rcm[(*(p++)) & 0xff];
		*(dst++) = (b >> 16) & 0xff;
		*(dst++) = (b >> 8) & 0xff;
		*(dst++) = b & 0xff;
		length -= 4;
	}

	if (length > 0) {
		b = s_rcm[(*(p++)) & 0xff]; b <<= 6;
		if (length > 1)
			b |= s_rcm[(*(p++)) & 0xff];
		b <<= 6;
		if (length > 2)
			b |= s_rcm[(*(p++)) & 0xff];
		b <<= 6;

		*(dst++) = (b >> 16) & 0xff;
		*(dst++) = (b >> 8) & 0xff;
		if (length > 2)
			*(dst++) = b & 0xff;
	}
	*(dst++) = '\0';
	return buf;
}

static void buf_encode_xor(char *buf, int len)
{
	int c = 0xcb;
	unsigned char *p = (unsigned char*)buf, *end = p + len;
	while (p < end) {
		*p = (*p ^ c) & 0xFF;
		c = *p++;
	}
}

static void buf_decode_xor(char *buf, int len)
{
	unsigned char *p = (unsigned char*)buf + len - 1;
	while (p > (unsigned char*)buf) {
		*p = (*p ^ *(p - 1)) & 0xFF;
		p--;
	}
	*p = (*p ^ 0xcb) & 0xFF;
}

static char *buf_writei8(char *s, int64_t v)
{
	int i;
	for (i = 7; i >= 0; i--, v >>= 8) {
		s[i] = v & 0xFF;
	}
	return s + 8;
}

static char *buf_writei4(char *s, int v)
{
	int i;
	for (i = 3; i >= 0; i--, v >>= 8) {
		s[i] = v & 0xFF;
	}
	return s + 4;
}

static char *buf_writei2(char *s, short v)
{
	int i;
	for (i = 1; i >= 0; i--, v >>= 8) {
		s[i] = v & 0xFF;
	}
	return s + 2;
}

static char *buf_writech(char *s, char v)
{
	*s = v;
	return s + 1;
}

static char *buf_writebs(char *s, const char *bytes, int n)
{
	memcpy(s, bytes, n);
	return s + n;
}

static const char *buf_readi8(const char *s, int64_t *pv)
{
	int64_t v = 0;
	int i;
	for (i = 0; i < 8; i++) {
		v <<= 8;
		v |= (s[i] & 0xff);
	}
	if (pv) *pv = v;
	return s + 8;
}

static const char *buf_readi4(const char *s, int *pv)
{
	int v = 0;
	int i;
	for (i = 0; i < 4; i++) {
		v <<= 8;
		v |= (s[i] & 0xff);
	}
	if (pv) *pv = v;
	return s + 4;
}

static const char *buf_readi2(const char *s, int *pv)
{
	int v = 0;
	int i;
	for (i = 0; i < 2; i++) {
		v <<= 8;
		v |= (s[i] & 0xff);
	}
	if (pv) *pv = v;
	return s + 2;
}

static const char *buf_readch(const char *s, int *pv)
{
	if (pv) *pv = (*s) & 0xff;
	return s + 1;
}

static const char *buf_readbs(const char *s, char *buf, int n)
{
	if (buf && n > 0)
		memcpy(buf, s, n);
	return s + n;
}

static const char *buf_readcs(const char *s, char **pstr, int n)
{
	char *str;
	if (pstr) {
		str = (char *)pcs_malloc(n + 1);
		if (str) {
			memcpy(str, s, n);
			str[n] = '\0';
		}
	}
	else
		str = NULL;
	*pstr = str;
	return s + n;
}

static int calc_cs(const char *d, int len)
{
	int i, a = 0x21;
	for (i = 0; i < len; i++) {
		a += (d[i] & 0xFF);
		a &= 0xFF;
	}
	return a;
}

static void pkg_cs(pkg_t *pkg)
{
	int cs = calc_cs(pkg->a.data + 16, pkg->a.len - 16);
	buf_writei2(pkg->a.data + 2, (short)cs);
}

static void pkg_av(pkg_t *pkg)
{
	buf_writei2(pkg->a.data + 4, 0xa);
}

static void pkg_bl(pkg_t *pkg, int len)
{
	pkg->bol += len;
	buf_writei2(pkg->a.data + 6, (short)pkg->bol);
}

static void pkg_p(pkg_t *pkg)
{
	pkg->pc++;
	buf_writei2(pkg->a.data + 8, (short)pkg->pc);
}

/* 用于发送数据到服务器时，打包数据 */
static int pkg_ppk(pkg_t *pkg, const char *a, int alen, int seq, int blen)
{
	int error;

	if (error = pcs_buf_write(&pkg->a, a, alen))
		return error;

	buf_writei2(pkg->a.data + pkg->a.len - alen + 2, (short)(alen - 4));

	pkg_p(pkg);
	pkg_bl(pkg, alen);

	pkg->a.data[10] = 0;
	buf_writei2(pkg->a.data + 11, (short)seq);
	buf_writei2(pkg->a.data + 2, (short)blen);

	return PCS_OK;
}

static void pkg_ldm(pkg_t *pkg, short a)
{
	buf_writei2(pkg->a.data + 13, a);
}

static int pkg_gap(pkg_t *pkg, const char *d, int len)
{
	int error;
	if ((rand() % 100) < 50) {
		if (error = pcs_buf_write(&pkg->a, d, len))
			return error;
		buf_writei2(pkg->a.data + pkg->a.len - len + 2, (short)(len - 4));
	}
	else {
		if (error = pcs_buf_insert(&pkg->a, 16, d, len))
			return error;
		buf_writei2(pkg->a.data + 16 + 2, (short)(len - 4));
	}
	pkg_av(pkg);
	pkg_p(pkg);
	pkg_bl(pkg, len);
	pkg_cs(pkg);
	return PCS_OK;
}

static int pkg_setdv(pkg_t *pkg, const char *d, int len)
{
	char *c;
	int error, i;

	c = (char *)pcs_malloc(len + 3);
	if (!c) return pcs_error(PCS_ENOMEM);

	memcpy(c + 2, d, len);
	for (i = 0; i < 2; i++)
		c[i] = d[i];
	buf_writei2(c + 2, (short)(len - 2));
	c[len + 2] = '\0';

	buf_encode_xor(c + 4, len - 2);

	error = pkg_gap(pkg, c, len + 2);
	pcs_free(c);

	return error;
}

static int pkg_init(pkg_t *pkg)
{
	int error;
	char a[16];

	memset(pkg, 0, sizeof(pkg_t));

	memset(a, 0, sizeof(a));

	error = pcs_buf_write(&pkg->a, a, sizeof(a));
	if (error)
		return error;

	buf_writei2(pkg->a.data, 0xa);

	pkg_ldm(pkg, 0x5d34);

	return PCS_OK;
}

static void pkg_free(pkg_t *pkg)
{
	if (pkg) {
		pcs_buf_free(&pkg->a);
	}
}

static int passport_initdv(pkg_t *pkg)
{
	char data[6], *p = data;
	p = buf_writei2(p, DV_MAGIC_INIT);
	p = buf_writei4(p, 0xa);
	return pkg_setdv(pkg, data, p - data);
}

static int passport_dv_toolbar(pkg_t *pkg)
{
	char data[8], *p = data;
	p = buf_writei2(p, DV_MAGIC_TOOLBAR);
	p = buf_writei2(p, 0x1);
	/* bK 的值为 0xe97b2,可在 Chrome Developer Tools 中导出. 导出方法参考 passport_dv_verify() 中的注释。*/
	p = buf_writei4(p, 0xe97b2);
	return pkg_setdv(pkg, data, p - data);
}

/* 登录开始时间 */
static int passport_dv_starttime(pkg_t *pkg, int64_t starttime)
{
	char data[10], *p = data;
	p = buf_writei2(p, DV_MAGIC_TIME);
	p = buf_writei8(p, starttime);
	return pkg_setdv(pkg, data, p - data);
}

/* 浏览器信息 */
static int passport_dv_browser(pkg_t *pkg)
{
	char data[8], *p = data;
	p = buf_writei2(p, DV_MAGIC_BROWSER);
	p = buf_writech(p, DV_BROWSER);
	p = buf_writei2(p, 0);
	p = buf_writech(p, DV_OS);
	p = buf_writebs(p, DV_BROWSER_VERSION, sizeof(DV_BROWSER_VERSION) - 1);
	return pkg_setdv(pkg, data, p - data);
}

static char *passport_get_pagetoken(int64_t starttime)
{
	static char token[64];
	int i, len;
	strcpy(token, "tk0.");
	len = 10 + (rand() % 16);
	for (i = 4; i < len; i++) {
		token[i] = '0' + (rand() % 10);
	}
	while (token[i] == '0')
		token[i--] = '\0';
	len = i + 1;
	memset(token + len, 0, sizeof(token) - len);
	snprintf(token + len, sizeof(token) - len - 1,
		"%" PRId64, starttime - (rand() % 50));
	return token;
}

/* PageToken */
static int passport_dv_token(pkg_t *pkg, int64_t starttime)
{
	char data[128], *p = data, *token;
	int tklen;
	token = passport_get_pagetoken(starttime);
	tklen = strlen(token);
	p = buf_writei2(p, DV_MAGIC_PAGETOKEN);
	p = buf_writech(p, tklen);
	p = buf_writebs(p, token, tklen);
	return pkg_setdv(pkg, data, p - data);
}

/* 未知 */
static int passport_dv_verify(pkg_t *pkg)
{
	static const char *s_functions[] = {
		"function (a){if(y[_0xc3a2('0x46')]){var b=window[_0xc3a2('0x3c')][_0xc3a2"
		"('0x3b')](_0xc3a2('0x3d'));if(b&&b[_0xc3a2('0x45')]){eval(y[_0xc3a2('0x46"
		"')]+'\\x3d'+'\\x22'+b[_0xc3a2('0x45')]+'\\x22'+'\\x3b')}else{var c=ch(a);"
		"var d=bb(c[_0xc3a2('0x20')](0x4));d=bz[_0xc3a2('0x4d')](c[_0xc3a2('0x20')"
		"](0x0,0x4)+d);eval(y[_0xc3a2('0x46')]+'\\x3d'+'\\x22'+bf(d)+'\\x22'+'\\x3"
		"b')}}}",

		"function (a){try{var b='';var c=a||window[_0xc3a2('0x80')];var d=c[_0xc3a"
		"2('0x81')]||c[_0xc3a2('0x82')];if(d&&d['\\x69\\x64']){b=encodeURIComponen"
		"t(d['\\x69\\x64'])}Q++;if(y[_0xc3a2('0xb')]!=0x0&&P>=y[_0xc3a2('0xb')]){r"
		"eturn}if(!(y[_0xc3a2('0x83')]>0x0&&Q==y[_0xc3a2('0x83')])){return}var e;v"
		"ar f;if(c[_0xc3a2('0x84')]){e=c[_0xc3a2('0x84')];f=c[_0xc3a2('0x85')]}els"
		"e{e=c[_0xc3a2('0x86')]+document[_0xc3a2('0x6c')][_0xc3a2('0x87')]-documen"
		"t[_0xc3a2('0x6c')][_0xc3a2('0x88')];f=c[_0xc3a2('0x89')]+document[_0xc3a2"
		"('0x6c')][_0xc3a2('0x8a')]-document[_0xc3a2('0x6c')][_0xc3a2('0x8b')]}var"
		" b='';var d=c[_0xc3a2('0x81')]||c[_0xc3a2('0x82')];if(d&&d['\\x69\\x64'])"
		"{b=encodeURIComponent(d['\\x69\\x64'])}var g=new Date()[_0xc3a2('0x8')]()"
		"-cl;var h=new co();var i=[0x8,0x2,b[_0xc3a2('0x14')],h[_0xc3a2('0x1e')](e"
		"),h[_0xc3a2('0x1e')](f),h[_0xc3a2('0x2d')](g),h[_0xc3a2('0x1d')](b)];i=bB"
		"(i);bS(bQ(bG(''+arguments[_0xc3a2('0x2c')])),''+arguments[_0xc3a2('0x2c')"
		"]);B(i);P++;Q=0x0}catch(_0x47fe90){}}",

		"function (){var a=[0x0,cl+'\\x7c'+Math[_0xc3a2('0x7')](),0x0];var b=bg(a)"
		";a[0x0]=b;return a}",

		"function (a){try{if(y[_0xc3a2('0xa')]>0x0&&W>=y[_0xc3a2('0xa')]){return}v"
		"ar b=a||window[_0xc3a2('0x80')];var c=b[_0xc3a2('0x81')]||b[_0xc3a2('0x82"
		"')];var d=b[_0xc3a2('0x9f')];var e=0x0;if(b[_0xc3a2('0xa0')]&&d!=0x11){e+"
		"=0x1}if(b[_0xc3a2('0xa1')]&&d!=0x12){e+=0x2}if(b[_0xc3a2('0xa2')]&&d!=0x1"
		"0){e+=0x4}var f=_0xc3a2('0xa3');if(c){f=c['\\x69\\x64']?encodeURIComponen"
		"t(c['\\x69\\x64']):c[_0xc3a2('0x42')]?encodeURIComponent(c[_0xc3a2('0x42'"
		")]):f}for(var g=0x0;g<y[_0xc3a2('0x3')][_0xc3a2('0x14')];g++){if(y[_0xc3a"
		"2('0x3')][g]==f){return}}var h=new Date()[_0xc3a2('0x8')]()-cl;var i=new "
		"co();var j=[0xc,0x2,f[_0xc3a2('0x14')],d,e,i[_0xc3a2('0x2d')](h),i[_0xc3a"
		"2('0x1d')](f)];if(c&&c[_0xc3a2('0x40')]==_0xc3a2('0x4')){j=[0xc,0x2,f[_0x"
		"c3a2('0x14')],d,e,i[_0xc3a2('0x2d')](h),i[_0xc3a2('0x1d')](f),i[_0xc3a2('"
		"0x1d')](_0xc3a2('0xa4'))]}j=bB(j);bS(bQ(bG(''+arguments[_0xc3a2('0x2c')])"
		"),''+arguments[_0xc3a2('0x2c')]);B(j);if(W<=0x2){O()}W++}catch(_0x15212d)"
		"{}}",
	};
	int findex, postion, length, flen, error;
	const char *func;
	char *d, *p;

	findex = rand() % 4;

	func = s_functions[findex];
	flen = strlen(func);

	do {
		postion = rand() % flen;
		length = rand() % 0x14;
		if (length < 2) length += 2;
	} while (postion + length > flen);

	p = d = (char *)pcs_malloc(7 + length);
	if (!d) return pcs_error(PCS_ENOMEM);

	p = buf_writei2(p, DV_MAGIC_VERIFY);
	p = buf_writech(p, findex);
	p = buf_writei2(p, postion);
	p = buf_writei2(p, length);
	p = buf_writebs(p, func + postion, length);

	error = pkg_setdv(pkg, d, p - d);

	pcs_free(d);

	return error;
}

/* maybe MAC address? */
static int passport_dv_pcmac(pkg_t *pkg)
{
	char data[3], *p = data;
	p = buf_writei2(p, DV_MAGIC_PCMAC);
	p = buf_writech(p, 0);
	return pkg_setdv(pkg, data, p - data);
}

/* 位置信息 */
static int passport_dv_location(pkg_t *pkg)
{
	char data[64], *p = data;
	const char *location = "http://pan.baidu.com/";
	const char *referer = "";
	int llen, rlen;
	llen = strlen(location);
	rlen = strlen(referer);
	p = buf_writei2(p, DV_MAGIC_LOCATION);
	p = buf_writei2(p, llen);
	p = buf_writei2(p, rlen);
	p = buf_writebs(p, location, llen);
	p = buf_writebs(p, referer, rlen);
	return pkg_setdv(pkg, data, p - data);
}

/* 屏幕信息 */
static int passport_dv_screeninfo(pkg_t *pkg)
{
	char data[44], *p = data;
	p = buf_writei2(p, DV_MAGIC_SCREENINFO);
	p = buf_writei4(p, DV_SCREEN_TOP);
	p = buf_writei4(p, DV_SCREEN_LEFT);
	p = buf_writei4(p, DV_CLIENT_WIDTH);
	p = buf_writei4(p, DV_CLIENT_HEIGHT);
	p = buf_writei4(p, DV_SCREEN_WIDTH);
	p = buf_writei4(p, DV_SCREEN_HEIGHT);
	p = buf_writei4(p, DV_SCREEN_AVAIL_WIDTH);
	p = buf_writei4(p, DV_SCREEN_AVAIL_HEIGHT);
	p = buf_writei4(p, DV_OUTER_WIDTH);
	p = buf_writei4(p, DV_OUTER_HEIGHT);
	return pkg_setdv(pkg, data, p - data);
}

static int passport_dv_testutils(pkg_t *pkg)
{
	/*
	通过以下脚本产生：
	(function() {
	var a = [
	(window['phantom'] ? 1 : 0).toString(),
	(window['_phantom'] ? 1 : 0).toString(),
	(window['callPhantom'] ? 1 : 0).toString(),
	(window['__fxdriver_unwrapped'] ? 1 : 0).toString(),
	(window['fxdriver_id'] ? 1 : 0).toString(),
	(document.getElementsByTagName('html')[0].getAttribute('webdriver') == null ? 0 : 1).toString(),
	(document['$cdc_asdjflasutopfhvcZLmcfl_'] ? 1 : 0).toString(),
	(document['__webdriver_script_fn'] ? 1 : 0).toString(),
	(window['webdriver'] ? 1 : 0).toString(),
	(window['ClientUtils'] ? 1 : 0).toString()
	];

	a = a.join('');
	a = parseInt('0000000000000000000000' + a, 2);

	var Serialization = function() {
	this.m = function(a, b, c) { return a >> b & Math.pow(2, (typeof c == "undefined" ? 1 : c) * 8) - 1};
	this.b2i2 = function(a) { return [this.m(a, 8), this.m(a, 0)] };
	this.b2i4 = function(a) { return this.b2i2(this.m(a, 16, 2)).concat(this.b2i2(this.m(a, 0, 2))) };
	};

	var hex = function(a) {
	a = a.toString(16);
	if(a.length == 1)
	return '\\x0' + a;
	else
	return '\\x' + a;
	};

	var k = new Serialization();
	var data = '\\x07\\x02';
	var arr = k.b2i4(a);
	for (var j = 0; j < arr.length; j++) {
	data += hex(arr[j]);
	}
	return data;
	})()
	*/
	const char *data = "\x07\x02\x00\x00\x00\x00";
	return pkg_setdv(pkg, data, 6);
}

static int passport_dv_mousemove(pkg_t *pkg, const char *target, int x, int y, int time)
{
	char *d, *p;
	int target_len, error;

	if (pkg->mousemove >= MAX_MOUSEMOVE)
		return PCS_OK;// pcs_error(PCS_ELIMIT);

	if (target)
		target_len = strlen(target);
	else
		target_len = 0;

	p = d = (char *)pcs_malloc(12 + target_len);
	if (!d) return pcs_error(PCS_ENOMEM);

	p = buf_writei2(p, DV_MAGIC_MOUSEMOVE);
	p = buf_writech(p, target_len);
	p = buf_writei2(p, x);
	p = buf_writei2(p, y);
	p = buf_writei4(p, time);
	if (target_len > 0)
		p = buf_writebs(p, target, target_len);
	error = pkg_setdv(pkg, d, p - d);
	pcs_free(d);
	return error;
}

static int passport_dv_mousedown(pkg_t *pkg, const char *target, int x, int y,
	int which, const char *eltattrs, int time, int rect,
	int left, int top, int width, int height)
{
	char *d, *p;
	int target_len, eltattr_len, error;

	if (pkg->mousedown >= MAX_MOUSEDOWN)
		return PCS_OK;// pcs_error(PCS_ELIMIT);

	if (target)
		target_len = strlen(target);
	else
		target_len = 0;

	if (eltattrs)
		eltattr_len = strlen(eltattrs);
	else
		eltattr_len = 0;

	p = d = (char *)pcs_malloc(24 + target_len + eltattr_len);
	if (!d) return pcs_error(PCS_ENOMEM);

	p = buf_writei2(p, DV_MAGIC_MOUSEDOWN);
	p = buf_writech(p, target_len);
	p = buf_writei2(p, x);
	p = buf_writei2(p, y);
	p = buf_writech(p, which);
	p = buf_writech(p, eltattr_len);
	p = buf_writei4(p, time);
	p = buf_writech(p, rect ? 1 : 0);
	if (rect) {
		p = buf_writei2(p, left);
		p = buf_writei2(p, top);
		p = buf_writei2(p, width);
		p = buf_writei2(p, height);
	}
	if (target_len > 0)
		p = buf_writebs(p, target, target_len);
	if (eltattr_len > 0)
		p = buf_writebs(p, eltattrs, eltattr_len);
	error = pkg_setdv(pkg, d, p - d);
	pcs_free(d);

	if (error) return error;

	if (pkg->mousedown < MAX_TESTUTILS) {
		error = passport_dv_testutils(pkg);
		if (error) return error;
	}

	pkg->mousedown++;

	return error;
}

static int passport_dv_focus(pkg_t *pkg, const char *target, int time)
{
	char *d, *p;
	int target_len, error;

	if (pkg->focus >= MAX_FOCUS)
		return PCS_OK;// pcs_error(PCS_ELIMIT);

	if (target)
		target_len = strlen(target);
	else
		target_len = 0;

	p = d = (char *)pcs_malloc(8 + target_len);
	if (!d) return pcs_error(PCS_ENOMEM);

	p = buf_writei2(p, DV_MAGIC_FCEVENT);
	p = buf_writei4(p, time);
	p = buf_writech(p, target_len);
	if (target_len > 0)
		p = buf_writebs(p, target, target_len);
	error = pkg_setdv(pkg, d, p - d);
	pcs_free(d);

	if (error) return error;

	pkg->focus++;

	return error;
}

static int passport_dv_keydown(pkg_t *pkg, const char *target, 
	int keycode, int crtlcode, int time, int pswd)
{
	char *d, *p;
	int target_len, error;

	if (pkg->keydown >= MAX_KEYDOWN)
		return PCS_OK;// pcs_error(PCS_ELIMIT);

	if (target)
		target_len = strlen(target);
	else
		target_len = 0;

	p = d = (char *)pcs_malloc(16 + target_len);
	if (!d) return pcs_error(PCS_ENOMEM);

	p = buf_writei2(p, DV_MAGIC_KEYDOWN);
	p = buf_writech(p, target_len);
	p = buf_writech(p, keycode);
	p = buf_writech(p, crtlcode);
	p = buf_writei4(p, time);
	if (target_len > 0)
		p = buf_writebs(p, target, target_len);
	if (pswd)
		p = buf_writebs(p, "pswd", 4);
	error = pkg_setdv(pkg, d, p - d);
	pcs_free(d);

	if (error) return error;

	if (pkg->keydown < MAX_TESTUTILS) {
		error = passport_dv_testutils(pkg);
		if (error) return error;
	}

	pkg->keydown++;

	return error;
}

static int passport_randi(int i)
{
	int v;
	if ((rand() % 100) < 50)
		v = (i + (rand() % 10));
	else
		v = (i - (rand() % 10));
	if (v < 0) v = 0;
	return v;
}

static int passport_keycodes(jskey_t *keys, const char *username, int n)
{
	int i, len, wlen, ch;
	int shift;
	wchar_t *wchs;
	jskey_t *key;

	if (!username) return pcs_error(PCS_EINVAL);

	len = strlen(username);
	wlen = u8_wc_size(username, len);
	if (wlen < 1) return pcs_error(PCS_EINVAL);

	wchs = (wchar_t*)pcs_malloc((wlen + 1) * sizeof(wchar_t));
	if (!wchs) return pcs_error(PCS_ENOMEM);

	wlen = u8_toucs(wchs, wlen + 1, username, len);

	for (i = 0, shift = 0, key = keys; (i < n) && (i < wlen); i++, key++) {
		ch = (unsigned)wchs[i];
		if ((ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '@' || ch == '+') { /* 需要按 Shift */
			if (!shift) {
				key->keycode = DV_KEY_SHIFT;
				key->flags = DV_CKEY_NONE;
				shift = 1;
				key++;
				i++;
			}
			key->flags = DV_CKEY_SHIFT;
			if (ch >= 'A' && ch <= 'Z')
				key->keycode = ch;
			else if (ch == '_')
				key->keycode = DV_KEY_DASH;
			else if (ch == '@')
				key->keycode = DV_KEY_2;
			else if (ch == '+')
				key->keycode = DV_KEY_EQUALSIGN;
			else
				assert("unreachable");
		}
		else {
			key->flags = DV_CKEY_NONE;
			shift = 0;
			if (ch >= 'a' && ch <= 'z')
				key->keycode = toupper(ch);
			else if (ch >= '0' && ch <= '9')
				key->keycode = ch;
			else if (ch == '-')
				key->keycode = DV_KEY_DASH;
			else if (ch == '.')
				key->keycode = DV_KEY_PERIOD;
			else if (ch > 127)
				key->keycode = DV_KEY_PLACEHOLDER;
			else {
				pcs_free(wchs);
				return pcs_error(PCS_EINVAL); /* 非预期的用户字符串 */
			}
		}
	}

	pcs_free(wchs);

	return PCS_OK;
}

/* 模拟输入用户名 */
static int passport_dv_imitate_input_username(pkg_t *pkg, int *ptime, const char *username)
{
	int i, time, error = PCS_OK;
	jskey_t keys[MAX_KEYDOWN] = { 0 };

	if (!username) return pcs_error(PCS_EINVAL);

	if ((error = passport_keycodes(keys, username, MAX_KEYDOWN)))
		return error;

	time = *ptime;
	for (i = 0; (!error) && (i < MAX_KEYDOWN); i++) {
		if (keys[i].keycode == 0)
			break;
		error = passport_dv_keydown(pkg, "TANGRAM__PSP_4__userName",
			keys[i].keycode, keys[i].flags, time, 0);
		time += 200 + (rand() % 200);
	}
	*ptime = time;
	return error;
}

/* 模拟用户行为 */
static int passport_dv_imitate(pkg_t *pkg, const char *username)
{
	int time, error;

	/* 移动鼠标到到 “账号密码登录” 的链接上 */
	time = passport_randi(1466);
	if ((error = passport_dv_mousemove(pkg, "",
		passport_randi(1110), passport_randi(125), time)))
		return error;

	/* 点击 “账号密码登录” */
	time = passport_randi(1803);
	if ((error = passport_dv_mousedown(pkg, "",
		passport_randi(1134), passport_randi(130), 0, "", time,
		1, 1103, 120, 65, 21)))
		return error;

	/* “账号密码登录”获取焦点 */
	time += (rand() % 10);
	if ((error = passport_dv_focus(pkg, "", time)))
		return error;

	/* 移动鼠标到用户名输入框 */
	time = passport_randi(3082);
	if ((error = passport_dv_mousemove(pkg, "TANGRAM__PSP_4__userName",
		passport_randi(1075), passport_randi(220), time)))
		return error;

	/* 点击用户名输入框 */
	time += (rand() % 5);
	if ((error = passport_dv_mousedown(pkg, "TANGRAM__PSP_4__userName",
		passport_randi(1065), passport_randi(219), 0, "", time,
		0, 0, 0, 0, 0)))
		return error;

	/* “账号密码登录”失去焦点 */
	time += (rand() % 5);
	if ((error = passport_dv_focus(pkg, "", time)))
		return error;

	/* “用户名输入框”获取焦点 */
	time += (rand() % 5);
	if ((error = passport_dv_focus(pkg, "TANGRAM__PSP_4__userName", time)))
		return error;

	/* 再次点击“用户名输入框” */
	time = passport_randi(3706);
	if ((error = passport_dv_mousedown(pkg, "TANGRAM__PSP_4__userName",
		passport_randi(1085), passport_randi(203), 0, "", time,
		0, 0, 0, 0, 0)))
		return error;

	/* 移开鼠标 */
	time = passport_randi(3945);
	if ((error = passport_dv_mousemove(pkg, "TANGRAM__PSP_4__userName",
		passport_randi(917), passport_randi(213), time)))
		return error;

	/* 模拟输入用户名 */
	time = passport_randi(5264);
	if ((error = passport_dv_imitate_input_username(pkg, &time, username)))
		return error;

	/* 按 TAB 键 */
	if ((error = passport_dv_keydown(pkg, "TANGRAM__PSP_4__userName",
		9, 0, time, 0)))
		return error;

	/* “用户名输入框”失去焦点 */
	time += (rand() % 500);
	if ((error = passport_dv_focus(pkg, "TANGRAM__PSP_4__userName", time)))
		return error;

	/* “密码输入框”获得焦点 */
	time += (rand() % 500);
	if ((error = passport_dv_focus(pkg, "TANGRAM__PSP_4__password", time)))
		return error;

	/* 移开鼠标 */
	time = passport_randi(16000);
	if ((error = passport_dv_mousemove(pkg, "",
		passport_randi(1257), passport_randi(32), time)))
		return error;

	/* 移动到登录按钮上 */
	time = passport_randi(16400);
	if ((error = passport_dv_mousemove(pkg, "TANGRAM__PSP_4__memberPassWrapper",
		passport_randi(1054), passport_randi(289), time)))
		return error;

	/* 点击登录按钮 */
	time = passport_randi(16900);
	if ((error = passport_dv_mousedown(pkg, "TANGRAM__PSP_4__submit",
		passport_randi(1050), passport_randi(355), 0, "", time,
		0, 0, 0, 0, 0)))
		return error;

	return PCS_OK;
}

/* 产生 dv
 *   pdv       - 用于接收产生的 dv，需要 free(*pdv)
 *   starttime - 登录页面打开的时间
 */
int passport_build_dv(char **pdv, int64_t starttime, const char *username)
{
	int error;
	char *dv = NULL;
	pkg_t pkg;

	if ((error = pkg_init(&pkg))) {
		*pdv = NULL;
		return error;
	}

	if ((error = passport_initdv(&pkg)) ||
		(error = passport_dv_toolbar(&pkg)) ||
		(error = passport_dv_starttime(&pkg, starttime)) ||
		(error = passport_dv_browser(&pkg)) ||
		(error = passport_dv_token(&pkg, starttime)) ||
		(error = passport_dv_verify(&pkg)) ||
		(error = passport_dv_pcmac(&pkg)) ||
		(error = passport_dv_location(&pkg)) ||
		(error = passport_dv_screeninfo(&pkg)) ||
		(error = passport_dv_testutils(&pkg))) {
		pkg_free(&pkg);
		*pdv = NULL;
		return error;
	}

	error = passport_dv_imitate(&pkg, username);

	if (error) {
		pkg_free(&pkg);
		*pdv = NULL;
		return error;
	}

	dv = passport_base64(pkg.a.data, pkg.a.len);

	*pdv = dv;

	pkg_free(&pkg);
	
	if (!dv) return pcs_error(PCS_ENOMEM);

	return PCS_OK;
}


static const char *passport_dv_browser_name(int browser)
{
	switch (browser) {
	case BROWSER_MSIE:
		return "IE";
	case BROWSER_CHROME:
		return "Chrome";
	case BROWSER_FIREFOX:
		return "Firefox";
	case BROWSER_360SE:
		return "360 SE";
	case BROWSER_360EE:
		return "360 EE";
	case BROWSER_OPERA:
		return "Opera";
	case BROWSER_SOGOU:
		return "Sogou";
	case BROWSER_QIHU_THEWORLD:
		return "Qihu Theworld";
	case BROWSER_TENCENT_TRAVELER:
		return "Tencent Traveler";
	case BROWSER_QQ_BROWSER:
		return "QQ Browser";
	case BROWSER_SAFARI:
		return "Safari";
	case BROWSER_MAXTHON:
		return "Maxthon";
	case BROWSER_UNKNOW:
	default:
		return "Unknow";
	}
}

static const char *passport_dv_os_name(int os)
{
	switch (os) {
	case OS_WIN:
		return "Windows";
	case OS_LINUX:
		return "Linux";
	case OS_MAC:
		return "Mac";
	case OS_IPHONE:
		return "iPhone";
	case OS_IPOD:
		return "iPod";
	case OS_IPAD:
		return "iPad";
	case OS_UNKNOW:
	default:
		return "Unknow";
	}
}

static const char *passport_dv_name(int magic)
{
	switch (magic) {
	case DV_MAGIC_TOOLBAR:
		return "Tool bar info";
	case DV_MAGIC_SLOWSCRIPT:
		return "Slow javascript";
	case DV_MAGIC_BROWSER:
		return "Browser info";
	case DV_MAGIC_INIT:
		return "Init";
	case DV_MAGIC_SCREENINFO:
		return "Screen info";
	case DV_MAGIC_TESTUTILS:
		return "Test utils";
	case DV_MAGIC_MOUSEMOVE:
		return "Mouse move";
	case DV_MAGIC_MOUSEDOWN:
		return "Mouse down";
	case DV_MAGIC_TOUCHSTART:
		return "Touch start";
	case DV_MAGIC_KEYDOWN:
		return "Key down";
	case DV_MAGIC_FCEVENT:
		return "Focus event";
	case DV_MAGIC_PCMAC:
		return "PC MAC address";
	case DV_MAGIC_LOCATION:
		return "Location and referer";
	case DV_MAGIC_TIME:
		return "Time";
	case DV_MAGIC_PAGETOKEN:
		return "Page token";
	case DV_MAGIC_VERIFY:
		return "Verify";
	default:
		return "Unknow";
	}
}

#define left_length(s,p,l) ((s) + (l) - (p))

static int passport_print_dvitem_default(int magic, const char *d, int len)
{
	int i;
	for (i = 0; i < len; i++, d++) {
		if (i % 8 == 0) printf("\n  ");
		printf("0x%02x ", (*d) & 0xff);
	}
	printf("\n");

	return PCS_OK;
}

static int passport_print_dvitem_toolbar(int magic, const char *d, int len)
{
	int flags, mg;

	if (len < 6) return pcs_error(PCS_EINVAL);

	d = buf_readch(d, NULL);
	d = buf_readch(d, &flags);
	d = buf_readi4(d, &mg);

	/* 下边的名字是猜的 */
	switch (flags) {
	case 0x2:
		printf("  Firebug\n");
		break;
	case 0x4:
		printf("  360\n");
		break;
	case 0x8:
		printf("  Resolution big than 200*200\n");
		break;
	case 0x1:
		printf("  No tool bar\n");
		break;
	case 0x0:
		printf("  [Error]\n");
		break;
	default:
		printf("  Unknow\n");
		break;
	}

	return PCS_OK;
}

static int passport_print_dvitem_browser(int magic, const char *d, int len)
{
	int browser, os;
	char *version;
	const char *p = d;

	if (len < 4) return pcs_error(PCS_EINVAL);

	p = buf_readch(p, &browser);
	p = buf_readi2(p, NULL); /* Property count */
	p = buf_readch(p, &os);
	p = buf_readcs(p, &version, left_length(d, p, len));
	if (!version) return pcs_error(PCS_ENOMEM);

	printf("  %s (%s) %s\n", passport_dv_browser_name(browser),
		passport_dv_os_name(os), version);

	pcs_free(version);

	return PCS_OK;
}

static int passport_print_dvitem_init(int magic, const char *d, int len)
{
	int flags;

	if (len < 4) return pcs_error(PCS_EINVAL);

	d = buf_readi4(d, &flags);

	printf("  flags=0x%x\n", flags);

	return PCS_OK;
}

static int passport_print_dvitem_screeninfo(int magic, const char *d, int len)
{
	int screen_top, screen_left, client_width, client_height, screen_width, screen_height,
		screen_avail_width, screen_avail_height, outer_width, outer_height;

	if (len < 40) return pcs_error(PCS_EINVAL);

	d = buf_readi4(d, &screen_top);
	d = buf_readi4(d, &screen_left);
	d = buf_readi4(d, &client_width);
	d = buf_readi4(d, &client_height);
	d = buf_readi4(d, &screen_width);
	d = buf_readi4(d, &screen_height);
	d = buf_readi4(d, &screen_avail_width);
	d = buf_readi4(d, &screen_avail_height);
	d = buf_readi4(d, &outer_width);
	d = buf_readi4(d, &outer_height);

	printf("  Screen Top=%d\n", screen_top);
	printf("  Screen Left=%d\n", screen_left);
	printf("  Client Width=%d\n", client_width);
	printf("  Client Height=%d\n", client_height);
	printf("  Screen Width=%d\n", screen_width);
	printf("  Screen Height=%d\n", screen_height);
	printf("  Screen Avail Width=%d\n", screen_avail_width);
	printf("  Screen Avail Height=%d\n", screen_avail_height);
	printf("  Outer Width=%d\n", outer_width);
	printf("  Outer Height=%d\n", outer_height);

	return PCS_OK;
}

static int passport_print_dvitem_utils(int magic, const char *d, int len)
{
	int flags;

	if (len < 4) return pcs_error(PCS_EINVAL);

	d = buf_readi4(d, &flags);

	if (flags & (1 << 9)) {
		printf("  phantom\n");
	}
	if (flags & (1 << 8)) {
		printf("  _phantom\n");
	}
	if (flags & (1 << 7)) {
		printf("  callPhantom\n");
	}
	if (flags & (1 << 6)) {
		printf("  __fxdriver_unwrapped\n");
	}
	if (flags & (1 << 5)) {
		printf("  fxdriver_id\n");
	}
	if (flags & (1 << 4)) {
		printf("  webdriver\n");
	}
	if (flags & (1 << 3)) {
		printf("  $cdc_asdjflasutopfhvcZLmcfl_\n");
	}
	if (flags & (1 << 2)) {
		printf("  __webdriver_script_fn\n");
	}
	if (flags & (1 << 1)) {
		printf("  webdriver\n");
	}
	if (flags & (1 << 0)) {
		printf("  ClientUtils\n");
	}

	return PCS_OK;
}

static int passport_print_dvitem_mousemove_events(int magic, const char *d, int len)
{
	const char *p = d;
	char *id;
	int idlen, x, y;
	int time; /*耗时*/

	if (len < 9) return pcs_error(PCS_EINVAL);

	p = buf_readch(p, &idlen);
	p = buf_readi2(p, &x);
	p = buf_readi2(p, &y);
	p = buf_readi4(p, &time);

	if (left_length(d, p, len) < idlen) return pcs_error(PCS_EINVAL);

	p = buf_readcs(p, &id, idlen);
	if (!id) return pcs_error(PCS_ENOMEM);

	printf("  x=%d, y=%d, target=\"%s\"", x, y, id);
	printf("  after %02d:%02d.%03d", (int)(time / 60000), (int)((time % 60000) / 1000), (time % 60000) % 1000);

	printf("\n");

	pcs_free(id);

	return PCS_OK;
}

static int passport_print_dvitem_mousedown_events(int magic, const char *d, int len)
{
	const char *p = d;
	char *id, *elt_attr;
	int idlen, x, y, button, elt_attr_len, have_rect;
	int left, top, offset_width, offset_height;
	int time; /*耗时*/

	if (len < 12) return pcs_error(PCS_EINVAL);

	p = buf_readch(p, &idlen);
	p = buf_readi2(p, &x);
	p = buf_readi2(p, &y);
	p = buf_readch(p, &button);
	p = buf_readch(p, &elt_attr_len);
	p = buf_readi4(p, &time);
	p = buf_readch(p, &have_rect);

	if (have_rect) {
		if (left_length(d, p, len) < 8) return pcs_error(PCS_EINVAL);
		p = buf_readi2(p, &left);
		p = buf_readi2(p, &top);
		p = buf_readi2(p, &offset_width);
		p = buf_readi2(p, &offset_height);
	}

	if (left_length(d, p, len) < (idlen + elt_attr_len))
		return pcs_error(PCS_EINVAL);

	p = buf_readcs(p, &id, idlen);
	if (!id) return pcs_error(PCS_ENOMEM);

	p = buf_readcs(p, &elt_attr, elt_attr_len);
	if (!elt_attr) {
		pcs_free(id);
		return pcs_error(PCS_ENOMEM);
	}

	printf("  x=%d, y=%d, which=%d, target=\"%s\"", x, y, button, id);
	printf("  after %02d:%02d.%03d\n", (int)(time / 60000), (int)((time % 60000) / 1000), (time % 60000) % 1000);
	if (have_rect) {
		printf("  rect: left=%d, top=%d, width=%d, height=%d\n", left, top, offset_width, offset_height);
	}
	printf("  elt attrs=\"%s\"", elt_attr);

	printf("\n");

	pcs_free(id);
	pcs_free(elt_attr);

	return PCS_OK;
}

static int passport_print_dvitem_keydown(int magic, const char *d, int len)
{
	const char *p = d;
	char *id, *type;
	int idlen, keycode, ctl, count;
	int time; /*耗时*/

	if (len < 7) return pcs_error(PCS_EINVAL);

	p = buf_readch(p, &idlen);
	p = buf_readch(p, &keycode);
	p = buf_readch(p, &ctl);
	p = buf_readi4(p, &time);

	if (left_length(d, p, len) < idlen)
		return pcs_error(PCS_EINVAL);

	p = buf_readcs(p, &id, idlen);
	if (!id)
		return pcs_error(PCS_ENOMEM);

	p = buf_readcs(p, &type, left_length(d, p, len));
	if (!type) {
		pcs_free(id);
		return pcs_error(PCS_ENOMEM);
	}

	printf("  ");
	count = 0;
	if (ctl & 0x1) {
		if (count) printf(" + ");
		printf("CTRL");
		count++;
	}
	if (ctl & 0x2) {
		if (count) printf(" + ");
		printf("ALT");
		count++;
	}

	if (ctl & 0x4) {
		if (count) printf(" + ");
		printf("SHIFT");
		count++;
	}

	if (count) printf(" + ");

	if (keycode == 32) {
		printf("keycode=0x20 (Space)");
	}
	else if (keycode < 32 || keycode >= 127) {
		printf("keycode=0x%x", keycode);
	}
	else {
		printf("keycode=0x%x (%c)", keycode, (char)toupper(keycode));
	}

	printf("  target=%s", id);
	if (strlen(type) > 0) {
		printf(" (%s)", type);
	}
	printf("  ");
	printf("after %02d:%02d.%03d", (int)(time / 60000), (int)((time % 60000) / 1000), (time % 60000) % 1000);

	printf("\n");

	pcs_free(id);
	pcs_free(type);

	return PCS_OK;
}

static int passport_print_dvitem_focus_events(int magic, const char *d, int len)
{
	const char *p = d;
	char *id;
	int idlen;
	int time; /*耗时*/

	if (len < 5) return pcs_error(PCS_EINVAL);

	p = buf_readi4(p, &time);
	p = buf_readch(p, &idlen);

	if (left_length(d, p, len) < idlen)
		return pcs_error(PCS_EINVAL);

	p = buf_readcs(p, &id, idlen);
	if (!id)
		return pcs_error(PCS_ENOMEM);

	printf("  target=\"%s\"", id);
	printf("  after %02d:%02d.%03d", (int)(time / 60000), (int)((time % 60000) / 1000), (time % 60000) % 1000);

	printf("\n");

	pcs_free(id);

	return PCS_OK;
}

static int passport_print_dvitem_pcmac(int magic, const char *d, int len)
{
	const char *p = d;
	int pc;

	if (len < 1) return pcs_error(PCS_EINVAL);

	p = buf_readch(p, &pc);

	printf("  property count=0x%x\n", pc);
	if (left_length(d, p, len) > 0) {
		char *s;
		p = buf_readcs(p, &s, left_length(d, p, len));
		if (!s)
			return pcs_error(PCS_ENOMEM);
		printf("  %s\n", s);
		pcs_free(s);
	}

	return PCS_OK;
}

static int passport_print_dvitem_location(int magic, const char *d, int len)
{
	const char *p = d;
	int location_len, referer_len;
	char *location, *referer;

	if (len < 4) return pcs_error(PCS_EINVAL);

	p = buf_readi2(p, &location_len);
	p = buf_readi2(p, &referer_len);

	if (left_length(d, p, len) < (location_len + referer_len))
		return pcs_error(PCS_EINVAL);

	p = buf_readcs(p, &location, location_len);
	if (!location)
		return pcs_error(PCS_ENOMEM);

	p = buf_readcs(p, &referer, referer_len);
	if (!referer) {
		pcs_free(location);
		return pcs_error(PCS_ENOMEM);
	}

	printf("  location=\"%s\"\n", location);
	printf("  referer=\"%s\"\n", referer);

	pcs_free(location);
	pcs_free(referer);

	return PCS_OK;
}

static int passport_print_dvitem_time(int magic, const char *d, int len)
{
	int64_t time;

	if (len < 8) return pcs_error(PCS_EINVAL);

	d = buf_readi8(d, &time);

	printf("  %s\n", pcs_time2str((time_t)(time / 1000)));

	return PCS_OK;
}

static int passport_print_dvitem_pagetoken(int magic, const char *d, int len)
{
	const char *p = d;
	int tlen;
	char *token;

	if (len < 1) return pcs_error(PCS_EINVAL);

	p = buf_readch(p, &tlen);

	if (left_length(d, p, len) < tlen)
		return pcs_error(PCS_EINVAL);

	p = buf_readcs(p, &token, tlen);
	if (!token)
		return pcs_error(PCS_ENOMEM);

	printf("  %s\n", token);

	pcs_free(token);

	return PCS_OK;
}

static int passport_print_dvitem_verify(int magic, const char *d, int len)
{
	const char *p = d;
	int fid, offset, slen;
	char *s;

	if (len < 5) return pcs_error(PCS_EINVAL);

	p = buf_readch(p, &fid);
	p = buf_readi2(p, &offset);
	p = buf_readi2(p, &slen);

	if (left_length(d, p, len) < slen)
		return pcs_error(PCS_EINVAL);

	p = buf_readcs(p, &s, slen);
	if (!s)
		return pcs_error(PCS_ENOMEM);

	printf("  function=0x%x, index=0x%d, length=0x%d  \"%s\"\n", fid, offset, slen, s);

	pcs_free(s);

	return PCS_OK;
}

static int passport_print_dvitem(int magic, const char *d, int len)
{
	printf("\n");
	printf("%s (magic=0x%04x, len=0x%04x)\n", passport_dv_name(magic), magic, len);

	switch (magic) {
	case DV_MAGIC_TOOLBAR:
		return passport_print_dvitem_toolbar(magic, d, len);
	case DV_MAGIC_SLOWSCRIPT:
		return passport_print_dvitem_default(magic, d, len);
	case DV_MAGIC_BROWSER:
		return passport_print_dvitem_browser(magic, d, len);
	case DV_MAGIC_INIT:
		return passport_print_dvitem_init(magic, d, len);
	case DV_MAGIC_SCREENINFO:
		return passport_print_dvitem_screeninfo(magic, d, len);
	case DV_MAGIC_TESTUTILS:
		return passport_print_dvitem_utils(magic, d, len);
	case DV_MAGIC_MOUSEMOVE:
		return passport_print_dvitem_mousemove_events(magic, d, len);
	case DV_MAGIC_MOUSEDOWN:
		return passport_print_dvitem_mousedown_events(magic, d, len);
	case DV_MAGIC_TOUCHSTART:
		return passport_print_dvitem_default(magic, d, len);
	case DV_MAGIC_KEYDOWN:
		return passport_print_dvitem_keydown(magic, d, len);
	case DV_MAGIC_FCEVENT:
		return passport_print_dvitem_focus_events(magic, d, len);
	case DV_MAGIC_PCMAC:
		return passport_print_dvitem_pcmac(magic, d, len);
	case DV_MAGIC_LOCATION:
		return passport_print_dvitem_location(magic, d, len);
	case DV_MAGIC_TIME:
		return passport_print_dvitem_time(magic, d, len);
	case DV_MAGIC_PAGETOKEN:
		return passport_print_dvitem_pagetoken(magic, d, len);
	case DV_MAGIC_VERIFY:
		return passport_print_dvitem_verify(magic, d, len);
	default:
		return passport_print_dvitem_default(magic, d, len);
	}
}

static int passport_print_dvhead(const char *d, int len, int *ppc)
{
	const char *p = d;
	int mgc, cs, av, bol, pc, ldm;

	if (len < 19)
		return pcs_error(PCS_EINVAL);

	if (strncmp("010", p, 3))
		return pcs_error(PCS_EINVAL);

	p = buf_readbs(p, NULL, 3); /* skip '010' */
	p = buf_readi2(p, &mgc); /* always 0xa */
	p = buf_readi2(p, &cs);  /* like crc32, maybe? */
	p = buf_readi2(p, &av);  /* always 0xa */
	p = buf_readi2(p, &bol); /* payload length */
	p = buf_readi2(p, &pc);  /* item count */
	p = buf_readbs(p, NULL, 3); /* unknow */
	p = buf_readi2(p, &ldm); /* always 0x5d34 */
	p = buf_readch(p, NULL); /* unknow */

	if (mgc != 0xa || av != 0xa || ldm != 0x5d34)
		return pcs_error(PCS_EINVAL);

	if (left_length(d, p, len) < bol)
		return pcs_error(PCS_EINVAL);

	if (cs != calc_cs(p, bol))
		return pcs_error(PCS_EINVAL);

	printf("magic=0x%04x cs=0x%04x av=0x%04x bol=0x%04x pc=0x%04x ldm=0x%04x\n",
		mgc, cs, av, bol, pc, ldm);

	if (ppc) (*ppc) = pc;

	return PCS_OK;
}

static int passport_print_dvitems(char *d, int len, int pc)
{
	int magic, payload_len, count = 0;
	int error;

	while (len > 0) {
		if (len < 4) return pcs_error(PCS_EINVAL);

		d = (char *)buf_readi2(d, &magic);
		d = (char *)buf_readi2(d, &payload_len);
		len -= 4;

		if (len < payload_len) return pcs_error(PCS_EINVAL);

		buf_decode_xor(d, payload_len);

		error = passport_print_dvitem(magic, d, payload_len);
		if (error) return pcs_error(PCS_EINVAL);

		d += payload_len;
		len -= payload_len;

		count++;
	}

	if (pc != count)
		return pcs_error(PCS_EINVAL);

	return PCS_OK;
}

int passport_print_dv(const char *dv)
{
	char *d;
	int dlen, pc, error;

	d = passport_debase64(&d, &dlen, dv, -1);

	if (!d) return pcs_error(PCS_EINVAL);

	if ((error = passport_print_dvhead(d, dlen, &pc))) {
		pcs_free(d);
		return error;
	}

	d += 19;
	dlen -= 19;

	if ((error = passport_print_dvitems(d, dlen, pc))) {
		pcs_free(d);
		return error;
	}

	return PCS_OK;
}
