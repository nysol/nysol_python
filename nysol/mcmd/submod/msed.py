# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Msed(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("msed",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Msed,self).__init__("msed",nutil.args2dict(args,kw_args,Nysol_Msed._kwd))


def msed(self,*args, **kw_args):
	return Nysol_Msed(nutil.args2dict(args,kw_args,Nysol_Msed._kwd)).addPre(self)

setattr(NysolMOD_CORE, "msed", msed)
