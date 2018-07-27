# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcommon(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mcommon",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcommon,self).__init__("mcommon",nutil.args2dict(args,kw_args,Nysol_Mcommon._kwd))

def mcommon(self,*args, **kw_args):
	return Nysol_Mcommon(nutil.args2dict(args,kw_args,Nysol_Mcommon._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcommon", mcommon)
