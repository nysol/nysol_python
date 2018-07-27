# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvsort(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mvsort",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvsort,self).__init__("mvsort",nutil.args2dict(args,kw_args,Nysol_Mvsort._kwd))


def mvsort(self,*args, **kw_args):
	return Nysol_Mvsort(nutil.args2dict(args,kw_args,Nysol_Mvsort._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvsort", mvsort)

