#pragma once

#include "exception.h"

class Log;
class Config;
class DBI;

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
	void Run() throw(Exception);

private:
	void Release();

private:
	Log*		m_pLog;
	Config*		m_pCfg;
	DBI*		m_pDBI;
};

