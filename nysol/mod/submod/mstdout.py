# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mstdout(NysolMOD_CORE):
	kwd = n_core.getparalist("mstdout")
	inkwd = ["i="]
	outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mstdout,self).__init__("mstdout",nutil.args2dict(args,kw_args,Nysol_Mstdout.kwd))

def mstdout(self,*args, **kw_args):
	return Nysol_Mstdout(nutil.args2dict(args,kw_args,Nysol_Mstdout.kwd)).addPre(self)


setattr(NysolMOD_CORE, "mstdout", mstdout)
