#pragma once

#include <string>
#include <vector>
#include <kgConfig.h>
#include <kgEnv.h>
#include <kgmod.h>
#include <kgError.h>
#include <kgArgs.h>
#include <signal.h>
#include <kgTempfile.h>
using namespace kglib;

namespace kgmod { ////////////////////////////////////////////// start namespace

class kgFifo : public kgMod {

	// 引数
	string iName_;   // i=
	size_t queSize_; // n=

	int iFD_;					//i=ファイルディスクリプタ
	vector<int> oFD_; //o=ファイルディスクリプタ

	void setArgs(void);
	void setArgs(int i,int o);

	void iClose(void) throw(kgError);
	void oClose(void) throw(kgError);

public:
	// コンストラクタ
	kgFifo(void);
	~kgFifo(void){}

	// 入出力ファイルオープン
	int run(void);
	int run(int i,int o);

};

}
