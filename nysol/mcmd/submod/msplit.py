# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Msplit(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("msplit",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Msplit,self).__init__("msplit",nutil.args2dict(args,kw_args,Nysol_Msplit._kwd))


def msplit(self,*args, **kw_args):
	return Nysol_Msplit(nutil.args2dict(args,kw_args,Nysol_Msplit._kwd)).addPre(self)

setattr(NysolMOD_CORE, "msplit", msplit)
