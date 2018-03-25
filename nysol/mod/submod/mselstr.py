# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mselstr(NysolMOD_CORE):
	kwd = n_core.getparalist("mselstr")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mselstr,self).__init__("mselstr",nutil.args2dict(args,kw_args,Nysol_Mselstr.kwd))

def mselstr(self,*args, **kw_args):
	return Nysol_Mselstr(nutil.args2dict(args,kw_args,Nysol_Mselstr.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mselstr", mselstr)
