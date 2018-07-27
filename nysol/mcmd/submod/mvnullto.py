# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mvnullto(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mvnullto",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvnullto,self).__init__("mvnullto",nutil.args2dict(args,kw_args,Nysol_Mvnullto._kwd))

def mvnullto(self,*args, **kw_args):
	return Nysol_Mvnullto(nutil.args2dict(args,kw_args,Nysol_Mvnullto._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvnullto", mvnullto)
