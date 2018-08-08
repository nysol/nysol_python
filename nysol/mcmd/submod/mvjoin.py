# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvjoin(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mvjoin",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvjoin,self).__init__("mvjoin",nutil.args2dict(args,kw_args,Nysol_Mvjoin._kwd))
		

def mvjoin(self,*args, **kw_args):
	return Nysol_Mvjoin(nutil.args2dict(args,kw_args,Nysol_Mvjoin._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvjoin", mvjoin)
