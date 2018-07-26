# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Writelist(NysolMOD_CORE):
	_kwd = n_core.getparalist("writelist")
	_kwd[0].append("dtype")
	_inkwd = ["i="]
	_outkwd = ["o="]

	def __init__(self,*args, **kw_args) :
		super(Nysol_Writelist,self).__init__("writelist",nutil.args2dict(args,kw_args,Nysol_Writelist._kwd,uk="o"))


def writelist(self,*args, **kw_args):
	return Nysol_Writelist(nutil.args2dict(args,kw_args,Nysol_Writelist._kwd,uk="o")).addPre(self)

setattr(NysolMOD_CORE, "writelist", writelist)

