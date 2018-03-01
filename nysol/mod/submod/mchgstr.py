# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mchgstr(NysolMOD_CORE):
	kwd = n_core.getparalist("mchgstr")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mchgstr,self).__init__("mchgstr",nutil.args2dict(args,kw_args,Nysol_Mchgstr.kwd))

def mchgstr(self,*args, **kw_args):
	return Nysol_Mchgstr(nutil.args2dict(args,kw_args,Nysol_Mchgstr.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mchgstr", mchgstr)

