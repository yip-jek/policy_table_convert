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
 * @brief ���ݿ���ز�������
 */

/** @brief ����prepare��ط����ľ��*/
typedef struct DBI_QUERY_T
{
	void *p_query;

	public:
		DBI_QUERY_T() { p_query = NULL;}
} dbi_query_t;

/** @brief ���ݿ������˻���Ϣ�ṹ��*/
struct db_user 
{
	char user[128];
	char pass[128];
	char host[128];
	char database[128];
};

/** @brief ����dbi�ӿ���flagֵ�ĺ���*/
typedef enum DBI_DB_FLAG_TYPE
{
	DBI_DB_FLAG_UNDEF = -2,
	DBI_DB_FLAG_NULL = -1,            /* ��ʶһ��ֵΪnull ֵ*/
	DBI_DB_FLAG_NOTNULL = 0,          /* ��ʶһ������Ϊ��nullֵ */
	DBI_DB_FLAG_TRUNCATE = 1          /* ��ʾ���ݱ����ݿ�ض�*/
} dbi_flag_t;

const int DBI_ENV_NOFOUND_DB_CRYPT=-2; // �Ҳ���DB_CRYPT������������Ҫָ�����ݿ�����ļ���·��
const int DBI_ERROR=-1;    // �������󣬿��Ե��� dbi->error������ȡ������Ϣ
const int DBI_SUCCESS=0;  // �����ɹ�
const int DBI_FETCH_EOF=1; // �α굽������β�����������

/**  @brief  sql������󳤶�*/
const int DBI_MAX_SQLSTR_LEN = 4096;
/**  @brief  �����ַ������� */
const int DBI_MAX_ERRORMSG_LEN = 4096;

/** @} */
#endif

