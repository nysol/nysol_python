# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mwrite(NysolMOD_CORE):
	kwd = n_core.getparalist("mwrite")
	inkwd = ["i="]
	outkwd = ["o="]
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mwrite,self).__init__("mwrite",nutil.args2dict(args,kw_args,Nysol_Mwrite.kwd))

def mwrite(self,*args, **kw_args):
	return Nysol_Mwrite(nutil.args2dict(args,kw_args,Nysol_Mwrite.kwd)).addPre(self)


setattr(NysolMOD_CORE, "mwrite", mwrite)
