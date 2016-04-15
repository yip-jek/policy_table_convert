/*
 * Author: hace <qianshanhai At gmail Dot com>
 * Date:   2010.9.10
 * Update: 2010.11.24
 */

#ifndef __SR_LIBMM_DBI_H
#define __SR_LIBMM_DBI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "dbi_fetch.h"
#include "dbi_define.h"

/** @addtogroup libmm
  @{
  */

/**
 * @example test_dbi.cpp
 * @example test_dbi_execute.cpp
 * @example test_dbi_select.cpp
 * @example test_dbi2_select.cpp 
 * @brief 数据库使用例子
 */

/**
 * @class DBI
 * @brief 通用的数据库操作接口
 * @file dbi.h
 * @brief DBI 通用的数据库操作接口
 */

struct dbi_func {
	int (*New)();
	int (*Delete)();
	int (*init)();
	int (*connect)(const struct db_user *user);
	int (*disconnect)();
	int (*begin)();
	int (*rollback)();
	int (*commit)();
	int (*execute)(const char *sql);
	int (*query)(fetch_t *f, const char *sql);
	int (*fetch)(fetch_t *f);
	int (*fetch_done)(fetch_t *f);
	char *(*error)();
	int (*_errno)(int *err);
	int (*lock_wait)(int secord);
	int (*detach)();
    char* (*version)();

	//add by james:
	int (*release)(dbi_query_t *q);
	int (*prepare)(dbi_query_t *q, const char *sql);
	int (*free_sql)(dbi_query_t *q);
	int (*execute_p)(dbi_query_t *q);
	int (*open_cursor)(dbi_query_t *q);
	int (*fetch_p)(dbi_query_t *q);
	int (*close_cursor)(dbi_query_t *q);
	int (*set_param_c)(dbi_query_t *q, int pos, char *val, int size, bool is_null);
	int (*set_param_i)(dbi_query_t *q, int pos, int &val, bool is_null);
	int (*set_param_l)(dbi_query_t *q, int pos, long &val, bool is_null);
	int (*set_param_f)(dbi_query_t *q, int pos, float &val, bool is_null);
	int (*set_param_d)(dbi_query_t *q, int pos, double &val, bool is_null); 
	int (*bind_param_c)(dbi_query_t *q, int pos, char *&val, int size);  
	int (*bind_param_i)(dbi_query_t *q, int pos, int *&val);   
	int (*bind_param_l)(dbi_query_t *q, int pos, long *&val);  
	int (*bind_param_f)(dbi_query_t *q, int pos, float *&val); 
	int (*bind_param_d)(dbi_query_t *q, int pos, double *&val);
	int (*get_field_c)(dbi_query_t *q, int pos, char *val, int size, dbi_flag_t &iflag);   
	int (*get_field_i)(dbi_query_t *q, int pos, int &val, dbi_flag_t &iflag);    
	int (*get_field_l)(dbi_query_t *q, int pos, long &val, dbi_flag_t &iflag);   
	int (*get_field_f)(dbi_query_t *q, int pos, float &val, dbi_flag_t &iflag);  
	int (*get_field_d)(dbi_query_t *q, int pos, double &val, dbi_flag_t &iflag); 
	int (*bind_field_c)(dbi_query_t *q, int pos, char *&val, int size);  
	int (*bind_field_i)(dbi_query_t *q, int pos, int *&val);   
	int (*bind_field_l)(dbi_query_t *q, int pos, long *&val);  
	int (*bind_field_f)(dbi_query_t *q, int pos, float *&val); 
	int (*bind_field_d)(dbi_query_t *q, int pos, double *&val);
	/* set a param value to null */
	int (*set_null)(dbi_query_t *q, int  pos, dbi_flag_t iflag);
	/* check if a field value is null */
	int (*get_null)(dbi_query_t *q, int  pos, dbi_flag_t &iflag );
	int (*get_incolumns)(dbi_query_t *q);
	int (*get_outcolumns)(dbi_query_t *q);
	int (*get_rows)(dbi_query_t *q);
};

typedef struct dbi_func *(*dbi_module_func_t)(void);

/** @endcond */

class DBI {

	/** @cond */
	private:
		char m_err_msg[DBI_MAX_ERRORMSG_LEN];
		char m_name[64];

		void *m_lib_handle;

		struct dbi_func *m_dbi_func;
		struct db_user m_db_user;

		static dbi_flag_t DEF_FLAG_BIND_VALUE;
		int m_connected;

	private:
		int dbi_open_library(const char *name);
		int dbi_open_library();
		int dbi_close_library();
		int dbi_get_db_crypt(struct db_user *p);

		/** @endcond */

	public:
		DBI();
		~DBI();

	public:

		/**
		 * @brief 初始化一些环境变量, 如 SR_DBI_PATH, DB_CRYPT.
		 * 在 new DBI() 后, 需要调用 dbi_init().
		 * @param name  所选用的数据库, 如 informix, oracle 等等.
		 * 为了使代码与数据库无关, 通常从配置文件读入这个参数,
		 * 否则, 代码可能无法通过评审.
		 * @param database 所要连接的数据库, 例如 devdb\@cxcs 中的 devdb.
		 * @param host informix:主机名, 如 devdb\@cxcs 中的 cxcs oracle:建议填用户名，
		 * dbcrypt生成密码文件时，也要用户名
		 * @return 返回 0 成功, 返回 -1 失败.
		 */
		int dbi_init(const char *name, const char *database, const char *host);

		/**
		 * @brief 连接数据库.
		 * @return 返回 0 成功, 返回 -1 失败.
		 */
		int dbi_connect();

		/**
		 * @brief 断开数据库.
		 * @return 返回 0 成功, 返回 -1 失败.
		 */
		int dbi_disconnect();

		int dbi_begin();
		int dbi_rollback();
		int dbi_commit();

		/**
		 * @brief 执行一条 SQL 语句, 该 SQL 是不能有返回值的.
		 * @param sql 标准的 SQL
		 * @return 返回 0 成功, 返回 -1 失败.
		 */
		int dbi_execute(const char *sql);

		/**
		 * @brief 执行一条 select SQL 语句
		 *
		 * 与 dbi_fetch() dbi_fetch_done() 一起使用, 当中的 f 需要使用
		 * declare_fetch(f) 声明.
		 * @param f 使用 declare_fetch(f) 声明
		 * @param sql 标准的 SQL 语句
		 * @return 返回 0 成功, 返回 -1 失败.
		 */
		int dbi_query(fetch_t *f, const char *sql);

		/**
		 * @brief 得到一行数据
		 *
		 * 需要使用 for_each_fetch() 或 fetch_value() 得到对应的值,
		 * @param[in] f 与 dbi_query() 对应使用的 f
		 * @return 有数据返回 0, 无数据返回 1, 失败返回 -1
		 */
		int dbi_fetch(fetch_t *f);

		/**
		 * @brief 释放 f 对应的资源
		 * @param f 与 dbi_query() 对应使用的 f
		 * @return 返回 0 成功, 返回 -1 失败.
		 */
		int dbi_fetch_done(fetch_t *f);

		/**
		 * @brief 得到上一个操作的错误信息, 如果有的话
		 * @return 返回错误的描述信息.
		 */
		const char *dbi_error();

		/**
		 * @brief 返回dbi_init传入的数据库name, 如 informix, oracle 等等.
		 * @return 返回数据库name
		 */
		const char *dbi_name();

		/**
		 * @brief 得到上一个操作的 SQLCODE
		 * @param err 得到另一个错误信息. 为sqlca.sqlerrd,
		 *            这么使用: int sqlerrd[6]; dbi_errno(sqlerrd);
		 *            sqlerrd[2] 就是影响的行数
		 * @return 返回错误的描述信息.
		 */
		int dbi_errno(int *err = NULL);

		/**
		 * @brief 设置锁, 目前只对 informix 有效
		 * @param secord 等待的秒数, 默认为 10s. 并且必须是 10 的整数倍.
		 * @return 成功返回 0, 失败返回 -1
		 */
		int dbi_lock_wait(int secord = 10);

		int dbi_detach();

        char * dbi_version();

	public: // add by james 2012.3.19

		/**
			@brief 
				init an dbi_query_t obj. you can think it takes the same effort with "q = new dbi_query_t;"
				but, please always use dbi_init_query interface to do this, it will be more compatibility for the future version.
			@warning
				remember to call dbi_del_query() to release the dbi_query_t obj. 
				otherwides, you will cause the memory leak problem!
			@param q 
				an uninited dbi_query_t point
			@return 
				DBI_SUCCESS -- success
			@return
				DBI_ERROR -- error, call db_error() to get detail error msg
			*/
		int dbi_init_query(dbi_query_t *&q);

		/**
			@brief 
				prepare a statement from an standard sql string
			@warning 
				remenber to call dbi_del_query() to release the resource 
				alloced for the prepared statement after, 
				otherwides, you will cause the memory leak problem!
			@param q
				an dbi_query_t obj point that inited by dbi_init_query()
			@param sql
				an standard sql string
			@return 
				DBI_SUCCESS -- prepare success
			@return 
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE
			*/
		int dbi_prepare_sql(dbi_query_t *q, const char *sql);

		/**
			@brief 
				close a statement that had already prepared.
			@param q
				an dbi_query_t obj point that prepared by dbi_prepare_sql().
			@return 
				DBI_SUCCESS -- success;
			@return 
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_free_sql(dbi_query_t *&q);
		
		/**
			@brief  
				release an dbi_query_t obj, 
					and release the resource with a statement that had already prepared 
			@param q
				an dbi_query_t obj point that inited by dbi_init_query()
			@return 
				DBI_SUCCESS -- success;
			@return 
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_del_query(dbi_query_t *&q);
		
		/**
			@brief 
				execute a statement that had already prepared 
			@param q
				an dbi_query_t obj point that prepared by 
					dbi_prepare_sql(dbi_query_t *q, const char *sql) 
			@return  
				DBI_SUCCESS -- success;
			@return 
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_execute(dbi_query_t *q);

		/**
			@brief  
				execute a statement that had already prepared,
					and open an select cursor ready for fetch data from the record set.
			@param q
				an dbi_query_t obj point that prepared by dbi_prepare_sql
			@return  
				DBI_SUCCESS -- success;
			@return 
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_open_cursor(dbi_query_t *q);

		/**
			@brief  
				fetch data  from a recordset use the opened cursor
			@return	
				DBI_SUCCESS  -- success;
			@return 
				DBI_FETCH_EOF -- no data found or have already reach the end of the record set;
			@return 
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_fetch(dbi_query_t *q);

		/**
			@brief
				close an opened cursor, so that you can reopen it again whitout reprepare it.
					when you call dbi_free_sql() it will close the cursor automaticly.
			@return  
				DBI_SUCCESS -- success;
			@return 
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;

			*/
		int dbi_close_cursor(dbi_query_t *q);

		/**
			@brief 
				set an char* /int/long/float or double  c/c++ value to a db input column
			@param q
				an prepared dbi_query_t obj
			@param pos
				the column id you want to set param, the valid value is [0, maxid -1]
			@param val
				an char* /int/long/float/double variable reference which contain the value you want to set.
			@param size
				if the val is char* type, you need to set the maxsize with the column, 
				or an str len bigger than strlen(val). 
			@param is_null
				when this value is true, set the param to null(default value is false--not null ).
			@return
				DBI_SUCCESS -- success;
			@return
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_set_param(dbi_query_t *q, int pos, char *val, int size, bool is_null = false);
		int dbi_set_param(dbi_query_t *q, int pos, int &val, bool is_null = false);
		int dbi_set_param(dbi_query_t *q, int pos, long &val, bool is_null = false);
		int dbi_set_param(dbi_query_t *q, int pos, float &val, bool is_null = false);
		int dbi_set_param(dbi_query_t *q, int pos, double &val, bool is_null = false);

		/**
			@brief 	
				bind an char* /int* /long* /float* or double*  variable to a db input column,
				when you change the value of bind variable 
				and the db input column value will be change too.
			@param q
				an prepared dbi_query_t obj
			@param pos
				the column id you want to set param, the valid value is [0, maxid -1]
			@param val
				an char* /int/long/float/double variable reference which contain the value you want to set.
			@param size
				if the val is char* type, you need to set size to strlen(val), 
				so, don't use bind interface for the "where" sub statement, when your strlen(val) is uncurrently.
				instead of bind, you need to use set_param() function for "where" sub statement.
			@param is_null
				when this value is true, set the param to null(default value is false--not null ).
			@return
				DBI_SUCCESS -- success;
			@return
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_bind_param(dbi_query_t *q, int pos, char *&val, int size=0);
		int dbi_bind_param(dbi_query_t *q, int pos, int *&val);
		int dbi_bind_param(dbi_query_t *q, int pos, long *&val);
		int dbi_bind_param(dbi_query_t *q, int pos, float *&val);
		int dbi_bind_param(dbi_query_t *q, int pos, double *&val);

		/**
			@brief 	
				get an value into char* /int* /long* /float* or double*  variable from a db output column,
				you can use char* to read clob
			@warning
				before you use dbi_get_field() functions to get data from an cursor, make sure you have called 
				dbi_open_cursor() before.
				now the maximum size of clob type can be read is 5242880
			@param q
				an prepared dbi_query_t obj
			@param pos
				the column id you want to set param, the valid value is [0, maxid -1]
			@param val
				an char* /int/long/float/double variable reference return from the call
					and which contains the value you want to get.
			@param size
				if the val is char* type, you need to set the buffer size
			@warning
				if the size for the char* type is not bigger enough, the return string will be tranceted,
				you can check whether the iflag is DBI_DB_FLAG_TRUNCATE to know this.
			@param iflag
				return the column value state, it would be :
					DBI_DB_FLAG_NOTNULL -- normal state, not null, and the val is validable; 
					DBI_DB_FLAG_NULL -- no value was set to val , is null state; 
					DBI_DB_FLAG_TRUNCATE -- the buffer size is too small, the return data is uncompleted.
			@return
				DBI_SUCCESS -- success;
			@return
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_get_field(dbi_query_t *q, int pos, char *val, int size, dbi_flag_t &iflag = DEF_FLAG_BIND_VALUE);
		int dbi_get_field(dbi_query_t *q, int pos, int &val, dbi_flag_t &iflag = DEF_FLAG_BIND_VALUE);
		int dbi_get_field(dbi_query_t *q, int pos, long &val, dbi_flag_t &iflag = DEF_FLAG_BIND_VALUE);
		int dbi_get_field(dbi_query_t *q, int pos, float &val, dbi_flag_t &iflag = DEF_FLAG_BIND_VALUE);
		int dbi_get_field(dbi_query_t *q, int pos, double &val, dbi_flag_t &iflag = DEF_FLAG_BIND_VALUE);

		/**
			@brief 	
				bind an  char* /int* /long* /float* or double*  variable to a db output column,
				when the db column value change, the reference variable change too.
				if you need to check the column null state, use dbi_get_null().
				you can use char* to read clob type
			@warning
				now the maximum size can be read of clob type is 5242880
			@param q
				an prepared dbi_query_t obj
			@param pos
				the column id you want to set param, the valid value is [0, maxid -1]
			@param val
				an char* /int/long/float/double variable reference return from the call
					and which contains the value you want to get.
			@param size
				if the val is char* type, you need to set the buffer size
			@return
				DBI_SUCCESS -- success;
			@return
				DBI_ERROR -- error, call db_error() to get detail error msg, or call dbi_errno() to get SQLCODE;
			*/
		int dbi_bind_field(dbi_query_t *q, int pos, char *&val, int size=0);
		int dbi_bind_field(dbi_query_t *q, int pos, int *&val);
		int dbi_bind_field(dbi_query_t *q, int pos, long *&val);
		int dbi_bind_field(dbi_query_t *q, int pos, float *&val);
		int dbi_bind_field(dbi_query_t *q, int pos, double *&val);

		/** 
			@brief 
				set a param value to null before execute or open_cursor
			@param q an dbi_query_t point that have prepared by dbi_prepare_sql function;
			@param pos the column id, the valid value is [0, maxid -1]
			@param iflag an dbi_flag_t val to set the column value to null/notnull state, it should be
				DBI_DB_FLAG_NOTNULL -- the value is not null; 
					or
				DBI_DB_FLAG_NULL -- the value is null; 
			@return DBI_SUCCESS --success
			@return DBI_ERROR --error, call db_error() to get detail error msg 
			*/
		int dbi_set_null(dbi_query_t *q, int  pos, dbi_flag_t iflag);

		/** @brief get return value state after dbi_fetch()
			@param q
				an dbi_query_t point that have prepared by dbi_prepare_sql function;
			@param pos
				the column id, the valid value is [0, maxid -1]
			@param iflag
				an dbi_flag_t val return the column value state, it could be:
				DBI_DB_FLAG_NOTNULL -- the value is not null; 
				DBI_DB_FLAG_NULL -- the value is null; 
				DBI_DB_FLAG_TRUNCATE -- the value was trancated;
			@return DBI_SUCCESS --success
			@return DBI_ERROR --error, call db_error() to get detail error msg 
			*/
		int dbi_get_null(dbi_query_t *q, int  pos, dbi_flag_t &iflag );

		/** @brief get the number of output columns
		 	@param none
			@return the number of output columns
			*/
		int dbi_get_outcolumns(dbi_query_t *q);

		/** @brief get the number of input parameters
		 	@param none
			@return the number of input parameters
			*/
		int dbi_get_incolumns(dbi_query_t *q);

		/** @brief get the affected rows
		 	@param none
			@return the affected rows of the sql has been prepared
			*/
		int dbi_get_rows(dbi_query_t *q);
};


/** 
  @}
  
  */
#endif
