#!/usr/bin/env python
# -*- coding: utf-8 -*-
#/* ////////// LICENSE INFO ////////////////////
#
# * Copyright (C) 2013 by NYSOL CORPORATION
# *
# * Unless you have received this program directly from NYSOL pursuant
# * to the terms of a commercial license agreement with NYSOL, then
# * this program is licensed to you under the terms of the GNU Affero General
# * Public License (AGPL) as published by the Free Software Foundation,
# * either version 3 of the License, or (at your option) any later version.
# * 
# * This program is distributed in the hope that it will be useful, but
# * WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF 
# * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# *
# * Please refer to the AGPL (http://www.gnu.org/licenses/agpl-3.0.txt)
# * for more details.
#
# ////////// LICENSE INFO ////////////////////*/
import os
import datetime

def errorLog(msg=""):
	vbl = os.getenv("KG_ScpVerboseLevel")
	if not vbl or int(vbl)>=1 :
		t = datetime.datetime.today()
		print("#ERROR# %s; %s"%( msg , t.strftime('%Y/%m/%d %H:%M:%S') ) )

def warningLog(msg=""):
	vbl = os.getenv("KG_ScpVerboseLevel")
	if not vbl or int(vbl) >= 2 :
		t = datetime.datetime.today()
		print("#WARNING# %s; %s"%( msg , t.strftime('%Y/%m/%d %H:%M:%S') ) )

def endLog(msg=""):
	vbl = os.getenv("KG_ScpVerboseLevel")
	if not vbl or int(vbl) >=3 :
		t = datetime.datetime.today()
		print("#END# %s; %s"%( msg , t.strftime('%Y/%m/%d %H:%M:%S') ) )


def msgLog(msg="",time=True,header=True):
	vbl = os.getenv("KG_ScpVerboseLevel")
	if not vbl or int(vbl) >=4 :
		str = ""
		str = "#MSG# " if header else ""
		str = str + msg + "; "

		if time :
			t = datetime.datetime.today()
			str = str + (t.strftime('%Y/%m/%d %H:%M:%S'))
		print(str)

		#STDERR.puts "#END# #{msg}; #{Time.now.strftime('%Y/%m/%d %H:%M:%S')}"
		#STDERR.puts str
