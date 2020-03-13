#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import shutil

import nysol.mcmd as nm
import nysol.util as nu
import nysol.util.margs as margs

from nysol.util.mtemp import Mtemp
from nysol.util.mmkdir import mkDir
from nysol.util.mparallel import meach as meach
from nysol.util.mrecount import mrecount


class mccomp(object):

	helpMSG="""
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) 連結成分を出力する

用法) #{$cmd} ei= ef= [ni=] [nf=] [o=] [T=] [-verbose] [-mcmdenv] [--help]

  ファイル名指定
  ei=    : 枝データファイル
  ef=    : 枝データ上の2つの節点項目名(省略時は"node1,node2")
  ni=    : 節点データファイル
  nf=    : 節点データ上の節点項目名(省略時は"node")
  o=     : 出力ファイル(連結成分ID-枝:-nodeを指定することでクリークID-節点に変更可能,省略時は標準出力)

  その他
  T= : ワークディレクトリ(default:/tmp)
  -verbose : Rの実行ログを出力
  --help : ヘルプの表示

入力形式)
一般グラフを節点ペアで表現した形式。

o=の出力形式)
節点と連結成分IDを出力する。
出力項目は"id,node,size"の3項目である。
sizeは連結成分を構成する節点数である。

例)
$ cat data/edge.csv 
n1,n2
a,d
a,e
b,f
d,e
f,g
g,b
g,h

$ #{$cmd} ei=edge.csv ef=n1,n2 o=output.csv
##END# #{$cmd} ei=edge.csv ef=n1,n2 -node o=output.csv
$ cat output.csv 
id%0,node,size
1,a,3
1,d,3
1,e,3
2,b,4
2,f,4
2,g,4
2,h,4

例) 節点ファイルも指定した例
$ cat node.csv 
n
a
b
c
d
e
f
g
h

$ #{$cmd} ei=edge.csv ef=n1,n2 ni=node.csv nf=n o=output.csv
#END# #{$cmd} ei=edge.csv o=output.csv ef=n1,n2 ni=node.csv nf=n
$ cat output.csv 
id%0,node,size
1,a,3
1,d,3
1,e,3
2,b,4
2,f,4
2,g,4
2,h,4
3,c,1
4,i,1

# Copyright(c) NYSOL 2012- All Rights Reserved.
"""

	verInfo="version=0.1"

	paramter = {	
		"ei":"str",
		"ef":"str",
		"ni":"str",
		"nf":"str",
		"o":"str",
		"verbose":"bool"
	}
	paramcond = {	
		"hissu": ["ei","ef"]
	}	

	def help():
		print(mccomp.helpMSG) 

	def ver():
		print(mccomp.verInfo)

	def __param_check_set(self , kwd):

		# 存在チェック
		for k,v in kwd.items():
			if not k in mccomp.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )

		self.msgoff = True

		self.oFile  = kwd["o"] if "o" in kwd else None

		self.ei  = kwd["ei"] 
		ef0 = kwd["ef"].split(",")
		self.ef1 = ef0[0]
		self.ef2 = ef0[1] 

		self.ni = kwd["ni"] if "ni" in kwd else None
		self.nf = kwd["nf"] if "nf" in kwd else None
		self.verbose  = kwd["verbose"]  if "verbose" in kwd else False



	def __cmdline(self):
		cmdline = self.__class__.__name__
		for k,v in self.args.items():
			if type(v) is bool :
				if v == True :
					cmdline += " -" + str(k)
			else:
				cmdline += " " + str(k) + "=" + str(v)
		return cmdline 

	def __init__(self,**kwd):
		#パラメータチェック
		self.args = kwd
		self.__param_check_set(kwd)



	####
	# generating the R script for graph features
	# pars: parameters for each graph feature
	def genRscript(self,eFile,oFile,cidFile,scpFile):
		
		r_proc = '''
library(igraph)
## reading edge file
g=read.graph("{eFile}",format="edgelist",directed=FALSE)
c=components(g)
seq=0:(length(c$membership)-1)
dat=data.frame(id=c$membership,nid=seq,size=c$csize[c$membership])
write.csv(dat,file="{oFile}",quote=FALSE,row.names = FALSE)
write.table(max(c$membership),file="{cidFile}",col.names = FALSE,row.names = FALSE)
		'''.format( eFile = eFile ,oFile = oFile,cidFile = cidFile)

		with open(scpFile,"w") as fpw:
			fpw.write(r_proc)


	def conv2num(self,ei,ni,ef1,ef2,nf,numFile,mapFile,isoFile):
		#MCMD::msgLog("converting graph files into a pair of numbered nodes ...")
		#wf=MCMD::Mtemp.new
		#wf1=wf.file
		#wf2=wf.file
		#wf3=wf.file
		
		allinObj =[]		
		
		
		inobj1 = nm.mcut(f="%s:node"%(ef1),i=ei )
		inobj2 = nm.mcut(f="%s:node"%(ef2),i=ei )

		allinObj.append(inobj1)
		allinObj.append(inobj2)

	
		if nf :
			allinObj.append(nm.mcut(f="%s:node"%(nf),i=ni) )
		
		#nodes list that are included in edge
		ne_list =  nm.muniq(k="node",i=[inobj1,inobj2]).msetstr(v=1,a="eNode")
		
		# isolate nodes list
		iso_f = nm.mcommon(i=allinObj ,k="node",m=ne_list,r=True).mcut(f="node",o=isoFile)
		iso_f.run()

		# create a mapping table between the original node label and the number iGraph will use
		map_f	=   nm.muniq(i=allinObj , k="node" )
		map_f	<<= nm.mjoin(m=ne_list , k="node" ,f="eNode" )
		map_f	<<= nm.mnullto( f="eNode",v=0 )
		map_f <<= nm.mnumber(s="eNode%r,node",a="nid",o=mapFile) 
		map_f.run()
		
		# create a data file that R script read
		f=None
		f <<= nm.mjoin(k=ef1,K="node",m=mapFile,f="nid:nid1",i=ei)
		f <<= nm.mjoin(k=ef2,K="node",m=mapFile,f="nid:nid2")
		f <<= nm.mcut(f="nid1,nid2",nfno=True)
		f <<= nm.cmd("tr ',' ' ' " )
		f <<= nm.mwrite(o=numFile)
		f.run()



	# ============
	# entry point
	def run(self,**kw_args):
	
		os.environ['KG_ScpVerboseLevel'] = "2"
		if "msg" in kw_args:
			if kw_args["msg"] == "on":
				os.environ['KG_ScpVerboseLevel'] = "4"

		# convert the original graph to one igraph can handle
		temp=Mtemp()
		numFile = temp.file()
		mapFile = temp.file()
		isoFile = temp.file()
		cluFile = temp.file()
		cidFile = temp.file()
		scpFile = temp.file()


		self.conv2num(self.ei,self.ni,self.ef1,self.ef2,self.nf,numFile,mapFile,isoFile)
		self.genRscript(numFile,cluFile,cidFile,scpFile)

		if self.verbose :
			os.system("R --vanilla -q < %s"%(scpFile))
		else:
			os.system("R --vanilla -q --slave < %s 2>/dev/null "%(scpFile))
			
		cid=0
		with open(cidFile) as rfp:
			xxx = rfp.read()
			cid = int(xxx.rstrip())

		isc = None
		isc <<= nm.mnumber(s="node", S=cid+1 ,a="id",i=isoFile)
		isc <<= nm.msetstr(v=1,a="size")
		isc <<= nm.mcut(f="id,node,size")
		if self.nf:
			isc <<= nm.mfldname(f="node:"+self.nf) 

		# #{cluFile}
		# id,nid,size
		# 1,0,3
		# 2,1,4
		# 1,2,3
		# 1,3,3
		# 2,4,4
		# 2,5,4
		# 2,6,4
		cln = None
		cln <<= nm.mjoin(k="nid",m=mapFile,i=cluFile,f="node")
		cln <<= nm.mcut(f="id,node,size")
		if self.nf:
			cln <<= nm.mfldname(f="node:"+self.nf) 

		nm.msortf(f="id",i=[isc,cln],o=self.oFile).run()

		nu.mmsg.endLog(self.__cmdline())

