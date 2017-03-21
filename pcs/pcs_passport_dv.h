#ifndef PCS_PASSPORT_DV_H_
#define PCS_PASSPORT_DV_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 产生 dv
 *   pdv       - 用于接收产生的 dv，需要 free(*pdv)
 *   starttime - 登录页面打开的时间
 */
int passport_build_dv(char **pdv, int64_t starttime, const char *username);

int passport_print_dv(const char *dv);

#ifdef __cplusplus
}
#endif

#endif /* PCS_PASSPORT_DV_H_ */
