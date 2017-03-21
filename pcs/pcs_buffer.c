#include "pcs_defs.h"
#include "pcs_buffer.h"

#include <stdlib.h>
#include <string.h>

#include "pcs_mem.h"

int pcs_buf_init(pcs_buf_t *buf)
{
	memset(buf, 0, sizeof(pcs_buf_t));
	return 0;
}

void pcs_buf_free(pcs_buf_t *buf)
{
	if (buf) {
		pcs_free(buf->data);
	}
}

int pcs_buf_enlarge(pcs_buf_t *buf, int want_size)
{
	char *newdata;
	int sz = buf->len + want_size;
	if (sz <= buf->size) return 0;
	sz = buf->len + want_size * 2;
	if (sz != buf->size) {
		newdata = (char *)pcs_malloc(sz);
		if (!newdata)
			return -1;
		if (buf->data) {
			memcpy(newdata, buf->data, buf->len + 1);
			pcs_free(buf->data);
		}
		buf->data = newdata;
		buf->size = sz;
	}
	return 0;
}

int pcs_buf_write(pcs_buf_t *buf, const char *data, int size)
{
	if (!data || size == 0)
		return 0;
	if (pcs_buf_enlarge(buf, size + 1))
		return -1;
	memcpy(buf->data + buf->len, data, size);
	buf->len += size;
	buf->data[buf->len] = '\0';
	return 0;
}

int pcs_buf_insert(pcs_buf_t *buf, int offset, const char *data, int size)
{
	if (!data || size == 0)
		return 0;
	if (buf->len > offset) {
		if (pcs_buf_enlarge(buf, size + 1))
			return -1;
		memmove(buf->data + offset + size, buf->data + offset, buf->len - offset + 1);
		memcpy(buf->data + offset, data, size);
		buf->len += size;
	}
	else {
		if (pcs_buf_enlarge(buf, offset + size - buf->len + 1))
			return -1;
		memcpy(buf->data + offset, data, size);
		buf->len = offset + size;
		buf->data[buf->len] = '\0';
	}
	return 0;
}
