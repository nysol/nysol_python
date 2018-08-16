/* ////////// LICENSE INFO ////////////////////

 * Copyright (C) 2013 by NYSOL CORPORATION
 *
 * Unless you have received this program directly from NYSOL pursuant
 * to the terms of a commercial license agreement with NYSOL, then
 * this program is licensed to you under the terms of the GNU Affero General
 * Public License (AGPL) as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF 
 * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Please refer to the AGPL (http://www.gnu.org/licenses/agpl-3.0.txt)
 * for more details.

 ////////// LICENSE INFO ////////////////////*/
// =============================================================================
// kgMessage.cpp メッセージ関係クラス
// =============================================================================
#include <sstream>
#include <string>
#include <ctime>
#include <kgMessage.h>
#include <kgmsgincpysys.h>

using namespace std;
using namespace kglib;

// -----------------------------------------------------------------------------
// 出力処理
// -----------------------------------------------------------------------------
void kgMsgIncPySys::WriteMsgPySys(string v ,string t)
{
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	PySys_WriteStderr("%s; %s\n",v.c_str(),t.c_str());
	PyGILState_Release(gstate);

}
// -----------------------------------------------------------------------------
// v + commentをメッセージ出力する
// -----------------------------------------------------------------------------
void kgMsgIncPySys::output_ignore(const string& v)
{
	WriteMsgPySys(v,getTime());
}

// -----------------------------------------------------------------------------
// v + commentをメッセージ出力する
// -----------------------------------------------------------------------------
void kgMsgIncPySys::output(const string& v, const string& comment)
{
	if(isOn()){
		ostringstream ss;
		ss << header() << " " << comment << "; " << v;
		WriteMsgPySys(ss.str(),getTime());
	}
}

