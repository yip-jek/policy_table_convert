#include "tabdb.h"
#include "def.h"

TabDB::TabDB()
:m_pDBI(NULL)
,m_bConnected(false)
,m_bPrepared(false)
,m_pQuerySel_io(NULL)
,m_pQuerySel_deb(NULL)
,m_pQueryUpd_deb(NULL)
,m_pQueryIns_deb(NULL)
,m_nGroupID(0)
{
}

TabDB::~TabDB()
{
	Release();
}

void TabDB::SetIOChannel(const std::string& io_channel)
{
	m_sTabIOChann = io_channel;
}

void TabDB::SetDebPolicy(const std::string& deb_policy)
{
	m_sTabDebPolicy = deb_policy;
}

void TabDB::SetGroupID(long group_id)
{
	m_nGroupID = group_id;
}

void TabDB::SetOtherCfg(const std::string& other_cfg)
{
	m_sOtherCfg = other_cfg;
}

void TabDB::Connect(const std::string& db_name, const std::string& svr_name) throw(Exception)
{
	Release();

	m_pDBI = new DBI;

	if ( m_pDBI->dbi_init("oracle", db_name.c_str(), svr_name.c_str()) != DBI_SUCCESS )
	{
		throw Exception(DB_INIT_FAIL, "DBI init failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}
	if ( m_pDBI->dbi_connect() != DBI_SUCCESS )
	{
		throw Exception(DB_CONNECT_FAIL, "DBI connect failed: %s [DB_NAME=%s, SVR_NAME=%s] [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), db_name.c_str(), svr_name.c_str(), __FILE__, __LINE__);
	}
	
	m_bConnected = true;
}

void TabDB::Disconnect() throw(Exception)
{
	if ( m_bConnected )
	{
		if ( m_pDBI->dbi_disconnect() != DBI_SUCCESS )
		{
			throw Exception(DB_DISCONNECT_FAIL, "DBI disconnect failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		m_bConnected = false;
	}
}

void TabDB::PrepareSql() throw(Exception)
{
	if ( !m_bPrepared )
	{
		if ( !InitQuery(m_pQuerySel_io) )
		{
			throw Exception(DB_QUERY_INIT_FAIL, "DBI init query [m_pQuerySel_io] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		if ( !InitQuery(m_pQuerySel_deb) )
		{
			throw Exception(DB_QUERY_INIT_FAIL, "DBI init query [m_pQuerySel_deb] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}
		if ( !InitQuery(m_pQueryUpd_deb) )
		{
			throw Exception(DB_QUERY_INIT_FAIL, "DBI init query [m_pQueryUpd_deb] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}
		if ( !InitQuery(m_pQueryIns_deb) )
		{
			throw Exception(DB_QUERY_INIT_FAIL, "DBI init query [m_pQueryIns_deb] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		std::string sql = "select INT_RAT_CHANNEL_ID, INT_TYPE_ID, VC_TYPE_EXTEND, INT_GROUP_ID from " + m_sTabIOChann;
		if ( m_pDBI->dbi_prepare_sql(m_pQuerySel_io, sql.c_str()) != 0 )
		{
			throw Exception(DB_PREPARE_FAIL, "DBI prepare sql [%s] failed: %s [FILE:%s, LINE:%d]", sql.c_str(), m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		sql = "select INT_RAT_CHANNEL_ID, VC_TABLE, VC_PATH, VC_COMMITPATH, INT_POLICY, INT_DS_CHANNEL_ID1, INT_DS_CHANNEL_ID2, VC_OTHER_CFG from " + m_sTabDebPolicy;
		if ( m_pDBI->dbi_prepare_sql(m_pQuerySel_deb, sql.c_str()) != 0 )
		{
			throw Exception(DB_PREPARE_FAIL, "DBI prepare sql [%s] failed: %s [FILE:%s, LINE:%d]", sql.c_str(), m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		sql = "update " + m_sTabDebPolicy + " set VC_TABLE = :v0, VC_PATH = :v1, VC_COMMITPATH = :v2, INT_POLICY = :v3, INT_DS_CHANNEL_ID1 = :v4, INT_DS_CHANNEL_ID2 = :v5, VC_OTHER_CFG = :v6 where INT_RAT_CHANNEL_ID = :v7";
		if ( m_pDBI->dbi_prepare_sql(m_pQueryUpd_deb, sql.c_str()) != 0 )
		{
			throw Exception(DB_PREPARE_FAIL, "DBI prepare sql [%s] failed: %s [FILE:%s, LINE:%d]", sql.c_str(), m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		sql = "insert into " + m_sTabDebPolicy + " (INT_RAT_CHANNEL_ID, VC_TABLE, VC_PATH, VC_COMMITPATH, INT_POLICY, INT_DS_CHANNEL_ID1, INT_DS_CHANNEL_ID2, VC_OTHER_CFG) values(:v0, :v1, :v2, :v3, :v4, :v5, :v6, :v7)";
		if ( m_pDBI->dbi_prepare_sql(m_pQueryIns_deb, sql.c_str()) != 0 )
		{
			throw Exception(DB_PREPARE_FAIL, "DBI prepare sql [%s] failed: %s [FILE:%s, LINE:%d]", sql.c_str(), m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		m_bPrepared = true;
	}
}

void TabDB::SqlFree() throw(Exception)
{
	if ( m_bPrepared )
	{
		if ( !FreeQuerySql(m_pQuerySel_io) )
		{
			throw Exception(DB_FREE_SQL_FAIL, "DBI free sql [m_pQuerySel_io] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		if ( !FreeQuerySql(m_pQuerySel_deb) )
		{
			throw Exception(DB_FREE_SQL_FAIL, "DBI free sql [m_pQuerySel_deb] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		if ( !FreeQuerySql(m_pQueryUpd_deb) )
		{
			throw Exception(DB_FREE_SQL_FAIL, "DBI free sql [m_pQueryUpd_deb] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		if ( !FreeQuerySql(m_pQueryIns_deb) )
		{
			throw Exception(DB_FREE_SQL_FAIL, "DBI free sql [m_pQueryIns_deb] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}

		m_bPrepared = false;
	}
}

void TabDB::SelectIOChannel(std::vector<ST_IOChannel>& v_io) throw(Exception)
{
	std::vector<ST_IOChannel>().swap(v_io);

	if ( m_pDBI->dbi_open_cursor(m_pQuerySel_io) != 0 )
	{
		throw Exception(DB_CURSOR_FAIL, "DBI open cursor [m_pQuerySel_io] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}

	ST_IOChannel st_io;
	while ( true )
	{
		int fetch_result = m_pDBI->dbi_fetch(m_pQuerySel_io);

		if ( DBI_SUCCESS == fetch_result )
		{
			st_io.Init();

			int col = 0;
			m_pDBI->dbi_get_field(m_pQuerySel_io, col++, st_io.channel_id);
			m_pDBI->dbi_get_field(m_pQuerySel_io, col++, st_io.type_id);
			m_pDBI->dbi_get_field(m_pQuerySel_io, col++, st_io.type_ex, sizeof(st_io.type_ex));
			m_pDBI->dbi_get_field(m_pQuerySel_io, col++, st_io.group_id);

			v_io.push_back(st_io);
		}
		else if ( DBI_FETCH_EOF == fetch_result )	// no data
		{
			break;
		}
		else if ( DBI_ERROR == fetch_result )
		{
			throw Exception(DB_FETCH_FAIL, "DBI fetch [m_pQuerySel_io] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}
		else
		{
			throw Exception(DB_FETCH_FAIL, "DBI fetch unknown result: %d [FILE:%s, LINE:%d]", fetch_result, __FILE__, __LINE__);
		}
	}

	if ( m_pDBI->dbi_close_cursor(m_pQuerySel_io) != 0 )
	{
		throw Exception(DB_CURSOR_FAIL, "DBI close cursor [m_pQuerySel_io] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}
}

void TabDB::SelectDebPolicy(std::vector<ST_DebPolicy>& v_deb) throw(Exception)
{
	std::vector<ST_DebPolicy>().swap(v_deb);

	if ( m_pDBI->dbi_open_cursor(m_pQuerySel_deb) != 0 )
	{
		throw Exception(DB_CURSOR_FAIL, "DBI open cursor [m_pQuerySel_deb] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}

	ST_DebPolicy st_deb;
	while ( true )
	{
		int fetch_result = m_pDBI->dbi_fetch(m_pQuerySel_deb);

		if ( DBI_SUCCESS == fetch_result )
		{
			st_deb.Init();

			int col = 0;
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.channel_id);
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.table, sizeof(st_deb.table));
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.path, sizeof(st_deb.path));
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.commit_path, sizeof(st_deb.commit_path));
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.policy);
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.chann_id1);
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.chann_id2);
			m_pDBI->dbi_get_field(m_pQuerySel_deb, col++, st_deb.other_cfg, sizeof(st_deb.other_cfg));

			v_deb.push_back(st_deb);
		}
		else if ( DBI_FETCH_EOF == fetch_result )	// no data
		{
			break;
		}
		else if ( DBI_ERROR == fetch_result )
		{
			throw Exception(DB_FETCH_FAIL, "DBI fetch [m_pQuerySel_io] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
		}
		else
		{
			throw Exception(DB_FETCH_FAIL, "DBI fetch unknown result: %d [FILE:%s, LINE:%d]", fetch_result, __FILE__, __LINE__);
		}
	}

	if ( m_pDBI->dbi_close_cursor(m_pQuerySel_io) != 0 )
	{
		throw Exception(DB_CURSOR_FAIL, "DBI close cursor [m_pQuerySel_io] failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}

}

void TabDB::UpdateDebPolicy(std::vector<ST_DebPolicy>& v_deb) throw(Exception)
{
	std::vector<ST_DebPolicy>().swap(v_deb);
}

void TabDB::InsertDebPolicy(std::vector<ST_DebPolicy>& v_deb) throw(Exception)
{
	std::vector<ST_DebPolicy>().swap(v_deb);
}

void TabDB::Begin() throw(Exception)
{
	if ( m_pDBI->dbi_begin() != 0 )
	{
		throw Exception(DB_BEGIN_FAIL, "DBI begin failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}
}

void TabDB::Commit() throw(Exception)
{
	if ( m_pDBI->dbi_commit() != 0 )
	{
		throw Exception(DB_COMMIT_FAIL, "DBI commit failed: %s [FILE:%s, LINE:%d]", m_pDBI->dbi_error(), __FILE__, __LINE__);
	}
}

void TabDB::Release()
{
	if ( m_pDBI != NULL )
	{
		if ( m_bConnected )
		{
			m_pDBI->dbi_disconnect();

			m_bConnected = false;
		}

		delete m_pDBI;
		m_pDBI = NULL;
	}

	SqlFree();

	ReleaseQuery(m_pQuerySel_io);
	ReleaseQuery(m_pQuerySel_deb);
	ReleaseQuery(m_pQueryUpd_deb);
	ReleaseQuery(m_pQueryIns_deb);
}

void TabDB::ReleaseQuery(dbi_query_t*& p_query)
{
	if ( p_query != NULL )
	{
		m_pDBI->dbi_del_query(p_query);
		p_query = NULL;
	}
}

bool TabDB::InitQuery(dbi_query_t*& p_query)
{
	if ( NULL == p_query )
	{
		if ( m_pDBI->dbi_init_query(p_query) != 0 )
		{
			return false;
		}
	}

	return true;
}

bool TabDB::FreeQuerySql(dbi_query_t*& p_query)
{
	if ( p_query != NULL )
	{
		if ( m_pDBI->dbi_free_sql(p_query) != 0 )
		{
			return false;
		}
	}

	return true;
}

