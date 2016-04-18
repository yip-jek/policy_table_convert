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

	m_nGroupID = (int)m_pCfg->GetCfgLongVal("CONVERTION", "INT_GROUP_ID");
	m_sOtherCfg = m_pCfg->GetCfgValue("CONVERTION", "VC_OTHER_CFG");

	InitMapConvertor("VC_TABLE", m_mTable);
	InitMapConvertor("VC_PATH", m_mPath);
	InitMapConvertor("VC_COMMITPATH", m_mCommitPath);

	Release();

	m_pTabDB = new TabDB;

	m_pTabDB->SetIOChannel(io_channel);
	m_pTabDB->SetDebPolicy(deb_policy);

	m_pTabDB->Connect(db_name, svr_name);
	m_pLog->Output("Connect database OK.");
}

void PolicyTabConv::Do() throw(Exception)
{
	m_pTabDB->PrepareSql();

	std::vector<ST_IOChannel> vc_io;
	m_pTabDB->SelectIOChannel(vc_io);

	std::vector<ST_DebPolicy> vc_deb;
	m_pTabDB->SelectDebPolicy(vc_deb);

	std::map<int, ST_IOChannel> m_io;
	int vec_size = vc_io.size();
	for ( int i = 0; i < vec_size; ++i )
	{
		ST_IOChannel& st_io = vc_io[i];
		m_io[st_io.channel_id] = st_io;
	}

	std::map<int, ST_DebPolicy> m_deb_val;
	vec_size = vc_deb.size();
	for ( int i = 0; i < vec_size; ++i )
	{
		ST_DebPolicy& st_deb = vc_deb[i];

		if ( m_io.find(st_deb.channel_id) != m_io.end() )
		{
			m_deb_val[st_deb.channel_id] = st_deb;
		}
	}

	std::map<int, ST_ONChannel> m_onc;
	GetONChannel(vc_io, m_onc);

	std::map<int, ST_DebPolicy> m_new_deb_upd;
	std::map<int, ST_DebPolicy> m_new_deb_ins;
	ST_DebPolicy new_st_deb;
	std::map<int, ST_DebPolicy>::iterator it = m_deb_val.begin();
	std::map<int, ST_ONChannel>::iterator on_it;
	MS2_IT mit;
	for ( ; it != m_deb_val.end(); ++it )
	{
		on_it = m_onc.find(it->first);
		if ( on_it != m_onc.end() )
		{
			ST_DebPolicy& st_deb = it->second;
			new_st_deb = st_deb;
			new_st_deb.channel_id = on_it->second.new_channel_id;
			strcpy(new_st_deb.other_cfg, m_sOtherCfg.c_str());

			mit = m_mTable.find(new_st_deb.table);
			if ( mit != m_mTable.end() )
			{
				strcpy(new_st_deb.table, mit->second.c_str());
			}
			mit = m_mPath.find(new_st_deb.path);
			if ( mit != m_mPath.end() )
			{
				strcpy(new_st_deb.path, mit->second.c_str());
			}
			mit = m_mCommitPath.find(new_st_deb.commit_path);
			if ( mit != m_mCommitPath.end() )
			{
				strcpy(new_st_deb.commit_path, mit->second.c_str());
			}

			if ( m_deb_val.find(new_st_deb.channel_id) != m_deb_val.end() )
			{
				m_new_deb_upd[new_st_deb.channel_id] = new_st_deb;
			}
			else
			{
				m_new_deb_ins[new_st_deb.channel_id] = new_st_deb;
			}
		}
	}
	m_pLog->Output("new update deb: size = %d", m_new_deb_upd.size());
	m_pLog->Output("new insert deb: size = %d", m_new_deb_ins.size());

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

void PolicyTabConv::GetONChannel(std::vector<ST_IOChannel>& vc_io, std::map<int, ST_ONChannel>& m_onc)
{
	m_onc.clear();

	ST_ONChannel onc;
	const int V_SIZE = vc_io.size();
	for ( int i = 0; i < V_SIZE; ++i )
	{
		ST_IOChannel& io = vc_io[i];

		if ( io.group_id != m_nGroupID )
		{
			onc.Init();

			onc.type_id        = io.type_id;
			onc.old_channel_id = io.channel_id;
			strcpy(onc.type_ex, io.type_ex);

			m_onc[onc.old_channel_id] = onc;
		}
	}

	std::map<int, ST_ONChannel>::iterator it;
	std::string l_ex;
	std::string r_ex;
	for ( int i = 0; i < V_SIZE; ++i )
	{
		ST_IOChannel& io = vc_io[i];

		if ( io.group_id == m_nGroupID )
		{
			l_ex = io.type_ex;
			for ( it = m_onc.begin(); it != m_onc.end(); ++it )
			{
				ST_ONChannel& onc = it->second;
				r_ex = onc.type_ex;

				if ( io.type_id == onc.type_id && l_ex == r_ex )
				{
					onc.new_channel_id = io.channel_id;
					break;
				}
			}
		}
	}
}

