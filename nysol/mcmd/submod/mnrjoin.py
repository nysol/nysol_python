# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnrjoin(NysolMOD_CORE):
	_kwd ,_inkwd,_outkwd = n_core.getparalist("mnrjoin",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnrjoin,self).__init__("mnrjoin",nutil.args2dict(args,kw_args,Nysol_Mnrjoin._kwd))


def mnrjoin(self,*args, **kw_args):
	return Nysol_Mnrjoin(nutil.args2dict(args,kw_args,Nysol_Mnrjoin._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnrjoin", mnrjoin)
