# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvdelnull(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mvdelnull",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvdelnull,self).__init__("mvdelnull",nutil.args2dict(args,kw_args,Nysol_Mvdelnull._kwd))


def mvdelnull(self,*args, **kw_args):
	return Nysol_Mvdelnull(nutil.args2dict(args,kw_args,Nysol_Mvdelnull._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvdelnull", mvdelnull)
