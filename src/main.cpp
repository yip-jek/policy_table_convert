#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "helper.h"
#include "log.h"
#include "config.h"
#include "policy_table_convert.h"

int main(int argc, char* argv[])
{
	if ( argc != 4 )
	{
		std::cerr << "[usage] " << argv[0] << " daemon_flag ccm_id cfg_file" << std::endl;
		return -1;
	}

	// Daemon process ?
	if ( Helper::Str2Int(argv[1]) == 1 )
	{
		pid_t fpid = fork();
		if ( fpid < 0 )		// fork error !
		{
			std::cerr << "Fork process error: " << fpid << std::endl;
			return -1;
		}
		else if ( fpid > 0 )	// Parent process end !
		{
			return 0;
		}
	}

	if ( !Log::SetCCMID(Helper::Str2LLong(argv[2])) )
	{
		std::cerr << "[LOG] Set CCM_ID fail!" << std::endl;
		return -1;
	}

	AutoLogger alog;
	Log* pLog = alog.Get();

	try
	{
		Config cfg;
		cfg.SetCfgFile(argv[3]);
		cfg.RegisterItem("SYS", "LOG_PATH");
		cfg.ReadConfig();

		pLog->SetPath(cfg.GetCfgValue("SYS", "LOG_PATH"));
		pLog->Init();

		char version[128] = "";
		snprintf(version, sizeof(version), "%s ver_%.2f. Compiled at %s on %s", argv[0], 1.0f, __TIME__, __DATE__);
		std::cout << version << std::endl;
		pLog->Output(version);

		PolicyTabConv ptc(cfg);
		ptc.Init();
		ptc.Do();
	}
	catch ( Exception& ex )
	{
		std::cerr << "[ERROR] " << ex.What() << ", ERROR_CODE: " << ex.ErrorCode() << std::endl;
		pLog->Output("[ERROR] %s, ERROR_CODE:%d", ex.What().c_str(), ex.ErrorCode());
	}
	catch ( ... )
	{
		std::cerr << "[ERROR] Unknown error!" << std::endl;
		pLog->Output("[ERROR] Unknown error!");
	}

	std::cout << "[MAIN] " << argv[0] << " quit!" << std::endl;
	pLog->Output("[MAIN] %s quit!", argv[0]);
	return 0;
}

