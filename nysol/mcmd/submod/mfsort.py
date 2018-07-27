# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mfsort(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mfsort",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mfsort,self).__init__("mfsort",nutil.args2dict(args,kw_args,Nysol_Mfsort._kwd))

def mfsort(self,*args, **kw_args):
	return Nysol_Mfsort(nutil.args2dict(args,kw_args,Nysol_Mfsort._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mfsort", mfsort)
