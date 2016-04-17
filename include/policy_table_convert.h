#pragma once

#include <string>
#include <map>
#include "exception.h"

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

private:
	Log*		m_pLog;
	Config*		m_pCfg;
	TabDB*		m_pTabDB;

private:
	MAP_STR2		m_mTable;
	MAP_STR2		m_mPath;
	MAP_STR2		m_mCommitPath;
};

