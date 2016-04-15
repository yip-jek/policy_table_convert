#include "policy_table_convert.h"
#include "config.h"
#include "log.h"
#include "def.h"
#include "helper.h"
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
	m_pCfg->RegisterItem("DATABASE", "DB_NAME");
	m_pCfg->RegisterItem("DATABASE", "SVR_NAME");
	m_pCfg->RegisterItem("TABLES", "IO_CHANNEL");
	m_pCfg->RegisterItem("TABLES", "DEB_POLICY");
	m_pCfg->RegisterItem("CONVERTION", "VC_OTHER_CFG");
	m_pCfg->RegisterItem("CONVERTION", "VC_TABLE");
	m_pCfg->RegisterItem("CONVERTION", "VC_PATH");
	m_pCfg->RegisterItem("CONVERTION", "VC_COMMITPATH");

	m_pCfg->ReadConfig();

	std::string db_name = m_pCfg->GetCfgValue("DATABASE", "DB_NAME");
	std::string svr_name = m_pCfg->GetCfgValue("DATABASE", "SVR_NAME");

	m_sTabIOChann = m_pCfg->GetCfgValue("TABLES", "IO_CHANNEL");
	m_sTabDebPolicy = m_pCfg->GetCfgValue("TABLES", "DEB_POLICY");
	m_sOtherCfg = m_pCfg->GetCfgValue("CONVERTION", "VC_OTHER_CFG");

	InitMapConvertor("VC_TABLE", m_mTable);
	InitMapConvertor("VC_PATH", m_mPath);
	InitMapConvertor("VC_COMMITPATH", m_mCommitPath);

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

void PolicyTabConv::Do() throw(Exception)
{
}

void PolicyTabConv::Release()
{
	if ( m_pDBI != NULL )
	{
		m_pDBI->dbi_disconnect();
		m_pLog->Output("Disconnect database.");

		delete m_pDBI;
		m_pDBI = NULL;
	}
}

void PolicyTabConv::InitMapConvertor(const std::string& name, MAP_STR2& m) throw(Exception)
{
	m.clear();

	std::string str;
	try
	{
		str = m_pCfg->GetCfgValue("CONVERTION", name);
	}
	catch ( Exception& ex )
	{
		return;
	}

	bool up = ("VC_TABLE" == name);

	std::list<std::string> list_str;
	Helper::SplitStr(str, ",", list_str, true);

	std::string left;
	std::string right;
	for ( std::list<std::string>::iterator it = list_str.begin(); it != list_str.end(); ++it )
	{
		if ( it->empty() )
		{
			throw Exception(PTC_INIT_FAIL, "Split [CONVERTION->%s] fail: %s [FILE:%s, LINE:%d]", name.c_str(), str.c_str(), __FILE__, __LINE__);
		}

		if ( !SeparateStr(*it, left, right, up) )
		{
			throw Exception(PTC_INIT_FAIL, "Separate [CONVERTION->%s] fail: %s [FILE:%s, LINE:%d]", name.c_str(), it->c_str(), __FILE__, __LINE__);
		}

		if ( m.find(left) != m.end() )
		{
			throw Exception(PTC_INIT_FAIL, "[CONVERTION->%s] item duplication: %s [FILE:%s, LINE:%d]", name.c_str(), left.c_str(), __FILE__, __LINE__);
		}

		m[left] = right;
	}
}

bool PolicyTabConv::SeparateStr(const std::string& src, std::string& left, std::string& right, bool upper)
{
	size_t pos = src.find(':');
	if ( std::string::npos == pos )
	{
		return false;
	}

	left = src.substr(0, pos);
	Helper::Trim(left);
	if ( left.find(':') != std::string::npos )
	{
		return false;
	}

	right = src.substr(pos+1);
	Helper::Trim(right);
	if ( right.find(':') != std::string::npos )
	{
		return false;
	}

	if ( upper )
	{
		Helper::Upper(left);
		Helper::Upper(right);
	}
	return true;
}

