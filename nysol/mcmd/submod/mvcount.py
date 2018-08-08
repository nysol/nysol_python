# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvcount(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mvcount",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvcount,self).__init__("mvcount",nutil.args2dict(args,kw_args,Nysol_Mvcount._kwd))


def mvcount(self,*args, **kw_args):
	return Nysol_Mvcount(nutil.args2dict(args,kw_args,Nysol_Mvcount._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvcount", mvcount)
