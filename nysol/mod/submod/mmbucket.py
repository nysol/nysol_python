# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mmbucket(NysolMOD_CORE):
	kwd = n_core.getparalist("mmbucket")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmbucket,self).__init__("mmbucket",nutil.args2dict(args,kw_args,Nysol_Mmbucket.kwd))

def mmbucket(self,*args, **kw_args):
	return Nysol_Mmbucket(nutil.args2dict(args,kw_args,Nysol_Mmbucket.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmbucket", mmbucket)
