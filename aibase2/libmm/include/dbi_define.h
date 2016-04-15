/*
 * Author: James <chenyuebin.tech At gmail Dot com>
 * Date:   2010.11.24
 * Update: 2010.11.24
 */

#ifndef __SR_LIBMM_DBI_DEFINE_H
#define __SR_LIBMM_DBI_DEFINE_H


/** @addtogroup libmm
@{
*/

/**
 * @file dbi_define.h
 * @brief 数据库相关参量定义
 */

/** @brief 用于prepare相关方法的句柄*/
typedef struct DBI_QUERY_T
{
	void *p_query;

	public:
		DBI_QUERY_T() { p_query = NULL;}
} dbi_query_t;

/** @brief 数据库连接账户信息结构体*/
struct db_user 
{
	char user[128];
	char pass[128];
	char host[128];
	char database[128];
};

/** @brief 定义dbi接口中flag值的含义*/
typedef enum DBI_DB_FLAG_TYPE
{
	DBI_DB_FLAG_UNDEF = -2,
	DBI_DB_FLAG_NULL = -1,            /* 标识一个值为null 值*/
	DBI_DB_FLAG_NOTNULL = 0,          /* 标识一个数据为非null值 */
	DBI_DB_FLAG_TRUNCATE = 1          /* 表示数据被数据库截断*/
} dbi_flag_t;

const int DBI_ENV_NOFOUND_DB_CRYPT=-2; // 找不到DB_CRYPT环境变量，需要指定数据库加密文件的路径
const int DBI_ERROR=-1;    // 操作错误，可以调用 dbi->error方法获取错误信息
const int DBI_SUCCESS=0;  // 操作成功
const int DBI_FETCH_EOF=1; // 游标到达结果集尾部，遍历完毕

/**  @brief  sql语句的最大长度*/
const int DBI_MAX_SQLSTR_LEN = 4096;
/**  @brief  错误字符串长度 */
const int DBI_MAX_ERRORMSG_LEN = 4096;

/** @} */
#endif

