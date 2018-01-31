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
#include "Python.h"
#include <fcntl.h>
#include <kgmod.h>
#include <kgCSV.h>
#include <kgMessage.h>
#include <kgMethod.h>


#if PY_MAJOR_VERSION >= 3
extern "C" {
	PyMODINIT_FUNC PyInit__utillib(void);
}
#else
extern "C" {
	void init_utillib(void);
}
#endif

// =============================================================================
// mrecountメソッド
// =============================================================================
/*
 * call-seq:
 * mrecount -> int
 *
 * <b>説明</b> レコード数を返す。
 *
 * <b>書式</b> MCMD::mrecount(options)
 *
 * i=ファイル名
 * -nfn : 一行目を項目名行とみなさない。
 */
PyObject* mrecount(PyObject* self, PyObject* args){
	kgEnv  env;
	int recNo=0; // 返値

	try { // kgmodのエラーはrubyのエラーとは別に検知する(メッセージ表示のため)

		// 引数をopetionsにセット
		char *fname=NULL;
		int nfn = 0;

		if(!PyArg_ParseTuple(args, "s|i", &fname,&nfn)){
			Py_RETURN_NONE ;
		}
		kgCSVrec csv;
		csv.open( fname, &env, nfn, 4 );
    csv.read_header();

		while(EOF!=csv.read()) recNo++;
		csv.close();

	}catch(kgError& err){ // kgmod関係エラーのchatch
		err.addModName("mrecount");
		kgMsg msg(kgMsg::ERR, &env);
		msg.output(err.message());
		return PyLong_FromLong(-1);
	}
	// 件数を返す
	return PyLong_FromLong(recNo);

}

// =============================================================================
// mheaderメソッド
// =============================================================================
/*
 * call-seq:
 * mheader -> int
 *
 * <b>説明</b> 項目名配列を返す。
 *
 * <b>書式</b> MCMD::mheader(options)
 *
 * i=ファイル名
 */
PyObject* mheader(PyObject* self, PyObject* args){

	kgEnv  env;
	string name; // rubyスクリプト名
	string argstr;

	try { // kgmodのエラーはrubyのエラーとは別に検知する(メッセージ表示のため)

		// 引数をopetionsにセット
		char *fname=NULL;
		int nfn = 0;

		if(!PyArg_ParseTuple(args, "s|i", &fname,&nfn)){
			Py_RETURN_NONE ;
		}

		kgCSVrec csv;
		csv.open( fname, &env, nfn, 4 );
    csv.read_header();
		size_t fldSize=csv.fldSize();
		PyObject* names = PyList_New(fldSize);

		if(nfn){
			for(size_t i=0; i<fldSize; i++){
				PyList_SetItem( names, i, PyLong_FromLong(i));
			}
		}else{
			for(size_t i=0; i<fldSize; i++){
				PyList_SetItem( names, i, Py_BuildValue("s",csv.fldName(i).c_str()) );
			}
		}
		csv.close();

		return names;

	}catch(kgError& err){ // kgmod関係エラーのchatch
		err.addModName("mheader");
		kgMsg msg(kgMsg::ERR, &env);
		msg.output(err.message());
			Py_RETURN_NONE ;
	}
}


static PyMethodDef utilmethods[] = {
	{"mrecount",(PyCFunction)mrecount, METH_VARARGS  },
	{"mheader" ,(PyCFunction)mheader , METH_VARARGS  },
	{NULL}
};


#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_utillib",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    utilmethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit__utillib(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	return m;
}

#else

void init_utillib(void){
	Py_InitModule("_utillib", utilmethods);
}

#endif

