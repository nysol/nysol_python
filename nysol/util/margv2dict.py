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
import re

def margv2dict(argv, klist ,mandatory=None,conv=None):
	
	if conv == None:
		conv = {} 

	if isinstance(klist[0],str) :
		keyPair   = set(klist[0].split(','))
	elif isinstance(klist[0],list) :
		keyPair   = set(klist[0])
	else:
		raise TypeError("keyilst type error")

	if isinstance(klist[1],str) :
		keySingle = set(klist[1].split(','))
	elif isinstance(klist[1],list) :
		keySingle = set(klist[1])
	else:
		raise TypeError("keyilst type error")

	kw_args ={}
	for arg in argv[1:] :
		val = arg.split("=",1)

		if len(val)==1 :
			if val[0][0] == '-':
				valm = re.sub(r'^-', "", val[0])
				if valm in keySingle:
					if valm in conv:
						kw_args[conv[valm]] = True
					else:
						kw_args[valm] = True
			else:
				raise TypeError("unkonwn parameter "+ val)

		elif len(val)==2 :
			if val[0] in keyPair:
				if val[0] in conv:
						kw_args[conv[val[0]]] = val[1]
				else:
						kw_args[val[0]] = val[1]
			else:	
				raise TypeError("unkonwn parameter "+ val[0])

	mlist =None
	if mandatory == None:
		return kw_args
	if isinstance(mandatory,str) :
		mlist = mandatory.split(',')
	elif isinstance(mandatory,list) :
		mlist = mandatory
	else:	
		raise TypeError("mandatory type error")


	for mkey in mlist :
		if not mkey in kw_args:
			raise ValueError(mkey + "is mandatory ")
	
	return kw_args
