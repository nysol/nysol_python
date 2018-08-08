# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcat(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mcat",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcat,self).__init__("mcat",nutil.args2dict(args,kw_args,Nysol_Mcat._kwd))

def mcat(self,*args, **kw_args):
	return Nysol_Mcat(nutil.args2dict(args,kw_args,Nysol_Mcat._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcat", mcat)

