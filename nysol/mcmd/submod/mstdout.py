# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mstdout(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mstdout",3)
	_disabled_ouputlist = True
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mstdout,self).__init__("mstdout",nutil.args2dict(args,kw_args,Nysol_Mstdout._kwd))

def mstdout(self,*args, **kw_args):
	return Nysol_Mstdout(nutil.args2dict(args,kw_args,Nysol_Mstdout._kwd)).addPre(self)


setattr(NysolMOD_CORE, "mstdout", mstdout)
