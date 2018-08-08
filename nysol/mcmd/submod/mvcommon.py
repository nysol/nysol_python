# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvcommon(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mvcommon",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvcommon,self).__init__("mvcommon",nutil.args2dict(args,kw_args,Nysol_Mvcommon._kwd))

def mvcommon(self,*args, **kw_args):
	return Nysol_Mvcommon(nutil.args2dict(args,kw_args,Nysol_Mvcommon._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvcommon", mvcommon)
