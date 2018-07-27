# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mtraflg(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mtraflg",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtraflg,self).__init__("mtraflg",nutil.args2dict(args,kw_args,Nysol_Mtraflg._kwd))


def mtraflg(self,*args, **kw_args):
	return Nysol_Mtraflg(nutil.args2dict(args,kw_args,Nysol_Mtraflg._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mtraflg", mtraflg)
