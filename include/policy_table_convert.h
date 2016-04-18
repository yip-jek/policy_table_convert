#pragma once

#include <string>
#include <vector>
#include <map>
#include "exception.h"
#include "structs.h"

class Log;
class Config;
class TabDB;

typedef std::map<std::string, std::string> MAP_STR2;
typedef MAP_STR2::iterator MS2_IT;

class PolicyTabConv
{
private:	// noncopyable
	PolicyTabConv(const PolicyTabConv& );
	const PolicyTabConv& operator = (const PolicyTabConv& );

public:
	explicit PolicyTabConv(Config& cfg);
	~PolicyTabConv();

public:
	void Init() throw(Exception);
	void Do() throw(Exception);

private:
	void Release();
	void InitMapConvertor(const std::string& name, MAP_STR2& m) throw(Exception);
	bool SeparateStr(const std::string& src, std::string& left, std::string& right, bool upper);
	void GetONChannel(std::vector<ST_IOChannel>& vc_io, std::map<int, ST_ONChannel>& m_onc);

private:
	Log*		m_pLog;
	Config*		m_pCfg;
	TabDB*		m_pTabDB;

private:
	int				m_nGroupID;
	std::string		m_sOtherCfg;
	MAP_STR2		m_mTable;
	MAP_STR2		m_mPath;
	MAP_STR2		m_mCommitPath;
};

