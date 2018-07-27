# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mduprec(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mduprec",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mduprec,self).__init__("mduprec",nutil.args2dict(args,kw_args,Nysol_Mduprec._kwd))

def mduprec(self,*args, **kw_args):
	return Nysol_Mduprec(nutil.args2dict(args,kw_args,Nysol_Mduprec._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mduprec", mduprec)
