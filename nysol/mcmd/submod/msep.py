# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Msep(NysolMOD_CORE): 

	_kwd ,_inkwd,_outkwd = n_core.getparalist("msep",3)
	_disabled_ouputlist = True

	def __init__(self,*args, **kw_args) :
		super(Nysol_Msep,self).__init__("msep",nutil.args2dict(args,kw_args,Nysol_Msep._kwd))


def msep(self,*args, **kw_args):
	return Nysol_Msep(nutil.args2dict(args,kw_args,Nysol_Msep._kwd)).addPre(self)

setattr(NysolMOD_CORE, "msep", msep)
