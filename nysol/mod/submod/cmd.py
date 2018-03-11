# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Excmd(NysolMOD_CORE):
	kwd = n_core.getparalist("cmd")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Excmd,self).__init__("cmd",nutil.arg2dict(args,kw_args,Nysol_Excmd.kwd))


def cmd(self,*args, **kw_args):
	return Nysol_Excmd(nutil.args2dict(args,kw_args,Nysol_Excmd.kwd)).addPre(self)

setattr(NysolMOD_CORE, "cmd", cmd)
