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

import os, sys

def dsplen(data):
				
	if sys.version_info.major < 3:
		datax = data.decode('utf-8')
	else:
		datax = data

	fsize=0
	for charstr in datax:
		if ord(charstr)<128 :
			fsize +=1
		else:
			fsize +=2
	return fsize

def dspchg(data,size):

	if sys.version_info.major < 3:
		datax = data.decode('utf-8')
	else:
		datax = data

	fsize=0

	for i , charstr in enumerate(datax):

		if ord(charstr)<128 :
			fsize +=1
		else:
			fsize +=2

		if fsize > size-3:
			return data[0:i]+"..."

	return data

def sizeCHK(data,premax):
	
	for lin in data:

		for i, fdata in enumerate(lin):

			fsize = dsplen(fdata)
			if fsize > premax[i] :
				premax[i] = fsize


def chgDSPstr(pre,hsizeL,fsizeL,head,dmy=False,fldnameLimit=15):

	try:
		if hasattr(os,"get_terminal_size"):
			width = os.get_terminal_size().columns
		else:
			_,width_str = os.popen('stty size').read().split()
			width = int(width_str)
	except:
		width=80

	sufmax = int(hsizeL)

	# fldcut check
	fldmax = [ 0 for i in range(len(pre[0]))]
	if head == None:
		sizeCHK(pre,fldmax)
	else:
		sizeCHK([head]+pre,fldmax)

	for i in range(len(fldmax)):
		if fldmax[i] > fldnameLimit:
			fldmax[i] = fldnameLimit

	fldcut=False
	if sum(fldmax) +len(fldmax) > width:

		dspfldNo =[]
		restW = width - (fldmax[-1]+5)

		for i ,v in enumerate(fldmax):

			if restW - v > 0 :
				dspfldNo.append(i) 
			else:
				dspfldNo.append(len(fldmax)-1)
				break 

			restW -= (v+1)

		fldcut = True

	else:
		
		dspfldNo =  [i for i in range(len(fldmax))]

	# make output string
	outstr=[]
	
	
	if head != None:
		lin = head

		newstr=[]

		for pos in dspfldNo:

			if fldcut and pos == len(fldmax)-1:
				newstr.append("...")   

			dlen = dsplen(lin[pos])

			if dlen <= fldmax[pos] :

				fmtdlen = (fldmax[pos]-dlen)+len(lin[pos])
				fmt = "%%%ds"%(fmtdlen)
				newstr.append(fmt%(lin[pos]))
			
			else:

				newdata = dspchg(lin[pos],fldmax[pos])
				fmtdlen = (fldmax[pos]-dsplen(newdata))+len(newdata)
				fmt = "%%%ds"%(fmtdlen)
				newstr.append(fmt%(newdata))

		outstr.append(" ".join(newstr))

	for i,lin in enumerate(pre):

		# dmy行
		if dmy and i == sufmax :

			dmystr = []

			for pos in dspfldNo:

				if fldcut and pos == len(fldmax)-1:
					dmystr.append("...")   

				dmylen = fldmax[pos] if fldmax[pos]<3 else 3
				fmt = "%%%ds"%(fldmax[pos])
				dmystr.append(fmt%("."*dmylen))
							
			outstr.append(" ".join(dmystr))


		newstr=[]

		for pos in dspfldNo:

			if fldcut and pos == len(fldmax)-1:
				newstr.append("...")   

			dlen = dsplen(lin[pos])

			if dlen <= fldmax[pos] :

				fmtdlen = (fldmax[pos]-dlen)+len(lin[pos])
				fmt = "%%%ds"%(fmtdlen)
				newstr.append(fmt%(lin[pos]))
			
			else:

				newdata = dspchg(lin[pos],fldmax[pos])
				fmtdlen = (fldmax[pos]-dsplen(newdata))+len(newdata)
				fmt = "%%%ds"%(fmtdlen)
				newstr.append(fmt%(newdata))

		outstr.append(" ".join(newstr))

	return outstr



def chgDSPhtml(dspdata, hsize ,fsize , maxno,head):

		yLimit = hsize + fsize

		mid = hsize
		arange_number = 0

		dmy = ( maxno > yLimit )

		outstrList=[]
		outstrList.append("<style scoped>")
		'''
		outstrList.append(".nysolnmp {")
		outstrList.append("border: none;")
		outstrList.append("border-collapse: collapse;")
		outstrList.append("border-spacing: 0;")
		outstrList.append("color: black;")
		outstrList.append("table-layout: fixed;")
		outstrList.append("}")
		outstrList.append(".nysolnmp thead th{")
		outstrList.append("border-bottom: 1px solid black;")
		outstrList.append("text-align: right;")
		outstrList.append("vertical-align: bottom;")
		outstrList.append("padding: 0.5em;")
		outstrList.append("white-space: normal;")
		outstrList.append("}")
		
		outstrList.append(".nysolnmp td,th{")
		outstrList.append("text-align: right;")
		outstrList.append("padding: 0.5em;")
		outstrList.append("white-space: normal;")
		outstrList.append("}")

		outstrList.append(".nysolnmp tbody tr:nth-child(2n+1){")
		outstrList.append("background: #f7f7f7;")
		outstrList.append("}")

		'''
		outstrList.append(".dataframe tbody tr th:only-of-type {")
		outstrList.append("vertical-align: middle;")
		outstrList.append("}")
		outstrList.append(".dataframe tbody tr th {")
		outstrList.append("vertical-align: top;")
		outstrList.append("}")

		outstrList.append(".dataframe thead th {")
		outstrList.append("text-align: right;")
		outstrList.append("}")


		outstrList.append("</style>")

		outstrList.append("<div>")
		outstrList.append("<table class='dataframe'>")
		outstrList.append("<thead>")
		if head != None:
			outstrList.append("<tr>")
			outstrList.append("<th></th>")
			for v in head:
				outstrList.append("<th>{}</th>".format(v))
			outstrList.append("</tr>")

		outstrList.append("</thead>")
		outstrList.append("<tbody>")

		for i,ldata in enumerate(dspdata):

			if i == mid and dmy: 
				arange_number = maxno - yLimit
				outstrList.append("<tr>")
				outstrList.append("<th>...</th>")
				for v in ldata:
					outstrList.append("<td>...</td>")
				outstrList.append("</tr>")


			outstrList.append("<tr>")
			outstrList.append("<th>{}</th>".format(i+arange_number))

			for v in ldata:
				outstrList.append("<td>{}</td>".format(v))
			outstrList.append("</tr>")

		outstrList.append("</tbody>")
		outstrList.append("</table>")
		outstrList.append("</div>")

		return outstrList


