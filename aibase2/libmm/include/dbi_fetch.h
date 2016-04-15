/*
 * Author: hace <qianshanhai At gmail Dot com>
 * Date:   2010.11.24
 * Update: 2010.11.24
 */

#ifndef __SR_LIBMM_DBI_FETCH_H
#define __SR_LIBMM_DBI_FETCH_H

#include <stdio.h>
#include <string.h>

/** @addtogroup libmm
@{
*/

/**
 * @file dbi_fetch.h
 * @brief 数据库数据结构
 */

#define FETCH_LENGTH 1024

/** @cond */
typedef struct {
	int T;		/* V 的个数 */
	char **V; 	/* 指针数组, 存放一列的值 */
	void *D;
	char *buf;
	int F; 		/* 标志 */
	char N[64];
	char C[64];
	int I, J, K; /* 临时变量 */
} fetch_t;
/** @endcond */

/**
 * @brief
 * @param f 声明 fetch_t 类型 f
 */
#define declare_fetch(f) fetch_t f[1]; fetch_clear(f)

#define fetch_used(f) (f->F)
#define fetch_name(f) (f->N)
#define fetch_cursor(f) (f->C)
#define fetch_out_data(f) (f->D)

inline void fetch_clear(fetch_t *f)
{
	memset(f, 0, sizeof(*f));
}

/**
 * @brief 
 * @param f 经过 dbi_fetch() 后的数据变量
 * @param pos  第几个值, 从 0 开始
 * @return 返回值都是 char * , 因此, 如果是整数请使用 atoi(), q_atoll() 进行转换.
 */
inline char *fetch_value(fetch_t *f, int pos)
{
	return pos >= 0 && pos < f->T ? f->V[pos] : NULL;
}

/**
 * @brief
 * @param f 经过 dbi_query() 后的数据变量
 * @return 返回一行中的元素个数
 */
inline int fetch_total(fetch_t *f)
{
	return f->T;
}

/**
 * @brief 把数据绑定到指定的变量上, 如:
 *
 * <pre>
 * char *name, *age; 
 * fetch_bind_value(f, &name, &age); 
 *
 * while (dbi->dbi_fetch(f) == 0) { 
 *     LOG("name = %s, age = %d\n", name, atoi(age)); 
 * }
 * </pre>
 *
 * @param f 经过 dbi_query() 后的数据变量
 * @param ... 其它的参数, 类型为 char ** 如:
 * @return 无返回值
 */
inline void fetch_bind_value(fetch_t *f, ...)
{
	va_list ap;

	va_start(ap, f);
	
	for (int i = 0; i < f->T; i++) {
		char **p = va_arg(ap, char **);
		if (p)
			*p = fetch_value(f, i);
	}

	va_end(ap);
}

inline int fetch_new(fetch_t *f, int total, void *D, char *buf)
{
	f->F = 1;
	f->D = D;
	f->buf = buf;
	f->T = total;
	f->V = (char **)calloc(f->T, sizeof(char *));

	for (int i = 0; i < f->T; i++) {
		f->V[i] = (char *)malloc(FETCH_LENGTH);
		if (f->V[i] == NULL)
			return -1;
		memset(f->V[i], 0, FETCH_LENGTH);
	}

	return 0;
}

inline void fetch_destory(fetch_t *f)
{
	if (!fetch_used(f))
		return;

	if (f->V) {
		for (int i = 0; i < f->T; i++) {
			if (f->V[i])
				free(f->V[i]);
		}
		free(f->V);
	}
	
	if (f->buf)
		free(f->buf);

	if (f->D)
		free(f->D);

	fetch_clear(f);
}

inline void fetch_getuniq(fetch_t *f, unsigned int *x, long y)
{
	(*x)++;
	snprintf(f->N, sizeof(f->N), "N%u%ld", *x, y);
	snprintf(f->C, sizeof(f->C), "C%u%ld", *x, y);
}

/**
 * @brief 遍历 f 的各个元素, 一般这么使用:
 *
 * <pre>
 * for_each_fetch(f, buf) {
 *     LOG("%s\n", buf);
 * }
 * </pre>
 * @param f   经过 dbi_fetch() 后的数据变量
 * @param buf 当时的那个元素
 */
#define for_each_fetch(f, buf) \
	f->I = 0; \
	for (char *buf = NULL; f->I < fetch_total(f) \
				&& (buf = fetch_value(f, f->I)); f->I++)

/** @} */

#endif
