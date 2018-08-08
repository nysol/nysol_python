# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mhashavg(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mhashavg",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mhashavg,self).__init__("mhashavg",nutil.args2dict(args,kw_args,Nysol_Mhashavg._kwd))

def mhashavg(self,*args, **kw_args):
	return Nysol_Mhashavg(nutil.args2dict(args,kw_args,Nysol_Mhashavg._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mhashavg", mhashavg)
