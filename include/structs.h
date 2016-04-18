#pragma once

#include <string.h>

// std::string sql = "select INT_RAT_CHANNEL_ID, INT_TYPE_ID, VC_TYPE_EXTEND, INT_GROUP_ID from " + m_sTabIOChann;
struct ST_IOChannel
{
public:
	ST_IOChannel(): channel_id(0), type_id(0), group_id(0)
	{ memset(type_ex, 0, sizeof(type_ex)); }
	ST_IOChannel(const ST_IOChannel& st_io): channel_id(st_io.channel_id), type_id(st_io.type_id), group_id(st_io.group_id)
	{ strcpy(this->type_ex, st_io.type_ex); }

	const ST_IOChannel& operator = (const ST_IOChannel& st_io)
	{
		if ( this != &st_io )
		{
			this->channel_id = st_io.channel_id;
			this->type_id    = st_io.type_id;
			this->group_id   = st_io.group_id;

			strcpy(this->type_ex, st_io.type_ex);
		}

		return *this;
	}

public:
	void Init()
	{
		channel_id = 0;
		type_id    = 0;
		group_id   = 0;

		memset(type_ex, 0, sizeof(type_ex));
	}

public:
	int channel_id;
	int type_id;
	char type_ex[32];
	int group_id;
};

// sql = "select INT_RAT_CHANNEL_ID, VC_TABLE, VC_PATH, VC_COMMITPATH, INT_POLICY, INT_DS_CHANNEL_ID1, INT_DS_CHANNEL_ID2, VC_OTHER_CFG from " + m_sTabDebPolicy;
struct ST_DebPolicy
{
public:
	ST_DebPolicy(): channel_id(0), policy(0), chann_id1(0), chann_id2(0)
	{
		memset(table, 0, sizeof(table));
		memset(path, 0, sizeof(path));
		memset(commit_path, 0, sizeof(commit_path));
		memset(other_cfg, 0, sizeof(other_cfg));
	}
	ST_DebPolicy(const ST_DebPolicy& st_deb): channel_id(st_deb.channel_id), policy(st_deb.policy), chann_id1(st_deb.chann_id1), chann_id2(st_deb.chann_id2)
	{
		strcpy(this->table, st_deb.table);
		strcpy(this->path, st_deb.path);
		strcpy(this->commit_path, st_deb.commit_path);
		strcpy(this->other_cfg, st_deb.other_cfg);
	}

	const ST_DebPolicy& operator = (const ST_DebPolicy& st_deb)
	{
		if ( this != &st_deb )
		{
			channel_id = st_deb.channel_id;
			policy = st_deb.policy;
			chann_id1 = st_deb.chann_id1;
			chann_id2 = st_deb.chann_id2;

			strcpy(this->table, st_deb.table);
			strcpy(this->path, st_deb.path);
			strcpy(this->commit_path, st_deb.commit_path);
			strcpy(this->other_cfg, st_deb.other_cfg);
		}

		return *this;
	}

public:
	void Init()
	{
		channel_id = 0;
		policy     = 0;
		chann_id1  = 0;
		chann_id2  = 0;

		memset(table, 0, sizeof(table));
		memset(path, 0, sizeof(path));
		memset(commit_path, 0, sizeof(commit_path));
		memset(other_cfg, 0, sizeof(other_cfg));
	}

public:
	int channel_id;
	char table[64];
	char path[256];
	char commit_path[256];
	int policy;
	int chann_id1;
	int chann_id2;
	char other_cfg[64];
};

struct ST_ONChannel
{
public:
	ST_ONChannel(): type_id(0), old_channel_id(0), new_channel_id(0)
	{ memset(type_ex, 0, sizeof(type_ex)); }
	ST_ONChannel(const ST_ONChannel& onc): type_id(onc.type_id), old_channel_id(onc.old_channel_id), new_channel_id(onc.new_channel_id)
	{ strcpy(this->type_ex, onc.type_ex); }

	const ST_ONChannel& operator = (const ST_ONChannel& onc)
	{
		if ( this != &onc )
		{
			this->type_id        = onc.type_id;
			this->old_channel_id = onc.old_channel_id;
			this->new_channel_id = onc.new_channel_id;

			strcpy(this->type_ex, onc.type_ex);
		}

		return *this;
	}

public:
	void Init()
	{
		type_id        = 0;
		old_channel_id = 0;
		new_channel_id = 0;

		memset(type_ex, 0, sizeof(type_ex));
	}

public:
	int type_id;
	char type_ex[32];
	int old_channel_id;
	int new_channel_id;
};

