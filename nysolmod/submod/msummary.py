# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Msummary(NysolMOD_CORE):
	kwd = n_core.getparalist("msummary")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msummary,self).__init__("msummary",nutil.args2dict(args,kw_args,Nysol_Msummary.kwd))

