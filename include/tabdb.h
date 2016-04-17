#pragma once

#include <string>
#include <vector>
#include "exception.h"
#include "dbi.h"
#include "structs.h"

class TabDB
{
public:
	TabDB();
	~TabDB();

public:
	void SetIOChannel(const std::string& io_channel);
	void SetDebPolicy(const std::string& deb_policy);
	void SetGroupID(long group_id);
	void SetOtherCfg(const std::string& other_cfg);

public:
	void Connect(const std::string& db_name, const std::string& svr_name) throw(Exception);
	void Disconnect() throw(Exception);
	void PrepareSql() throw(Exception);
	void SqlFree() throw(Exception);
	void SelectIOChannel(std::vector<ST_IOChannel>& v_io) throw(Exception);
	void SelectDebPolicy(std::vector<ST_DebPolicy>& v_deb) throw(Exception);
	void UpdateDebPolicy(std::vector<ST_DebPolicy>& v_deb) throw(Exception);
	void InsertDebPolicy(std::vector<ST_DebPolicy>& v_deb) throw(Exception);

private:
	void Begin() throw(Exception);
	void Commit() throw(Exception);
	void Release();
	void ReleaseQuery(dbi_query_t*& p_query);
	bool InitQuery(dbi_query_t*& p_query);
	bool FreeQuerySql(dbi_query_t*& p_query);

private:
	DBI*			m_pDBI;
	bool			m_bConnected;
	bool			m_bPrepared;

	dbi_query_t*	m_pQuerySel_io;
	dbi_query_t*	m_pQuerySel_deb;
	dbi_query_t*	m_pQueryUpd_deb;
	dbi_query_t*	m_pQueryIns_deb;

	std::string		m_sTabIOChann;
	std::string		m_sTabDebPolicy;
	long			m_nGroupID;
	std::string		m_sOtherCfg;
};

