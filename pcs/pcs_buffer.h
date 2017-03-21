#ifndef PCS_BUFFER_H_
#define PCS_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pcs_buf_t pcs_buf_t;

struct pcs_buf_t {
	char *data;
	int len;
	int size;
};

#define PCS_BUF_INIT() { 0 }

int pcs_buf_init(pcs_buf_t *buf);

void pcs_buf_free(pcs_buf_t *buf);

/* 扩容，使得 buf 有足够的空间来装入 want_size 大小的数据 */
int pcs_buf_enlarge(pcs_buf_t *buf, int want_size);

int pcs_buf_write(pcs_buf_t *buf, const char *data, int size);

int pcs_buf_insert(pcs_buf_t *buf, int offset, const char *data, int size);

#define pcs_buf_clear(buf) ((buf)->len = 0)

#ifdef __cplusplus
}
#endif

#endif /* PCS_BUFFER_H_ */
