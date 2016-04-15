#include "policy_table_convert.h"
#include "config.h"
#include "log.h"
#include "def.h"
#include "dbi.h"

PolicyTabConv::PolicyTabConv(Config& cfg)
:m_pLog(Log::Instance())
,m_pCfg(&cfg)
,m_pDBI(NULL)
{
}

PolicyTabConv::~PolicyTabConv()
{
	Release();
}

void PolicyTabConv::Init() throw(Exception)
{
	std::string db_name;
	std::string svr_name;

	Release();

	m_pDBI = new DBI;

	if ( m_pDBI->dbi_init("oracle", db_name.c_str(), svr_name.c_str()) != DBI_SUCCESS )
	{
		throw Exception(PTC_INIT_FAIL, "DBI init failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}
	if ( m_pDBI->dbi_connect() != DBI_SUCCESS )
	{
		throw Exception(PTC_INIT_FAIL, "DBI connect failed: %s [DB_NAME=%s, SVR_NAME=%s] [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), db_name.c_str(), svr_name.c_str(), __FILE__, __LINE__);
	}

	m_pLog->Output("Connect database OK.");
}

void PolicyTabConv::Run() throw(Exception)
{
}

void PolicyTabConv::Release()
{
	if ( m_pDBI != NULL )
	{
		delete m_pDBI;
		m_pDBI = NULL;
	}
}

