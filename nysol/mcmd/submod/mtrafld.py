# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mtrafld(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd = n_core.getparalist("mtrafld",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtrafld,self).__init__("mtrafld",nutil.args2dict(args,kw_args,Nysol_Mtrafld._kwd))

def mtrafld(self,*args, **kw_args):
	return Nysol_Mtrafld(nutil.args2dict(args,kw_args,Nysol_Mtrafld._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mtrafld", mtrafld)
