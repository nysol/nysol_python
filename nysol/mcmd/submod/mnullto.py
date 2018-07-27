# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnullto(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mnullto",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnullto,self).__init__("mnullto",nutil.args2dict(args,kw_args,Nysol_Mnullto._kwd))


def mnullto(self,*args, **kw_args):
	return Nysol_Mnullto(nutil.args2dict(args,kw_args,Nysol_Mnullto._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnullto", mnullto)
