#include "policy_table_convert.h"
#include "config.h"
#include "log.h"
#include "def.h"
#include "helper.h"
#include "tabdb.h"

PolicyTabConv::PolicyTabConv(Config& cfg)
:m_pLog(Log::Instance())
,m_pCfg(&cfg)
,m_pTabDB(NULL)
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
	m_pCfg->RegisterItem("CONVERTION", "INT_GROUP_ID");
	m_pCfg->RegisterItem("CONVERTION", "VC_OTHER_CFG");
	m_pCfg->RegisterItem("CONVERTION", "VC_TABLE");
	m_pCfg->RegisterItem("CONVERTION", "VC_PATH");
	m_pCfg->RegisterItem("CONVERTION", "VC_COMMITPATH");

	m_pCfg->ReadConfig();

	std::string db_name = m_pCfg->GetCfgValue("DATABASE", "DB_NAME");
	std::string svr_name = m_pCfg->GetCfgValue("DATABASE", "SVR_NAME");
	std::string io_channel = m_pCfg->GetCfgValue("TABLES", "IO_CHANNEL");
	std::string deb_policy = m_pCfg->GetCfgValue("TABLES", "DEB_POLICY");
	long group_id = (long)m_pCfg->GetCfgLongVal("CONVERTION", "INT_GROUP_ID");
	std::string other_cfg = m_pCfg->GetCfgValue("CONVERTION", "VC_OTHER_CFG");

	InitMapConvertor("VC_TABLE", m_mTable);
	InitMapConvertor("VC_PATH", m_mPath);
	InitMapConvertor("VC_COMMITPATH", m_mCommitPath);

	Release();

	m_pTabDB = new TabDB;

	m_pTabDB->SetIOChannel(io_channel);
	m_pTabDB->SetDebPolicy(deb_policy);
	m_pTabDB->SetGroupID(group_id);
	m_pTabDB->SetOtherCfg(other_cfg);

	m_pTabDB->Connect(db_name, svr_name);
	m_pLog->Output("Connect database OK.");
}

void PolicyTabConv::Do() throw(Exception)
{
	m_pTabDB->PrepareSql();

	std::vector<ST_IOChannel> vc_io;
	m_pTabDB->SelectIOChannel(vc_io);

	int size = vc_io.size();
	m_pLog->Output("IO_CHANNEL: size = %d", size);
	for ( int i = 0; i < size; ++i )
	{
		ST_IOChannel& si = vc_io[i];
		m_pLog->Output("%d) channel_id=%d, type_id=%d, extend=%s, group_id=%d", i+1, si.channel_id, si.type_id, si.type_ex, si.group_id);
	}

	std::vector<ST_DebPolicy> vc_deb;
	m_pTabDB->SelectDebPolicy(vc_deb);

	/*
	int channel_id;
	char table[64];
	char path[256];
	char commit_path[256];
	int policy;
	int chann_id1;
	int chann_id2;
	char other_cfg[64];
	*/
	size = vc_deb.size();
	m_pLog->Output("DEB_POLICY: size = %d", size);
	for ( int i = 0; i < size; ++i )
	{
		ST_DebPolicy& sd = vc_deb[i];
		m_pLog->Output("%d) channel_id=%d, table=%s, path=%s, commit_path=%s, policy=%d, channID1=%d, channID2=%d, other_cfg=%s", i+1, sd.channel_id, sd.table, sd.path, sd.commit_path, sd.policy, sd.chann_id1, sd.chann_id2, sd.other_cfg);
	}

	m_pTabDB->SqlFree();
}

void PolicyTabConv::Release()
{
	if ( m_pTabDB != NULL )
	{
		delete m_pTabDB;
		m_pTabDB = NULL;
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

