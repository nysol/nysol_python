# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mtonull(NysolMOD_CORE):
	kwd = n_core.getparalist("mtonull")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtonull,self).__init__("mtonull",nutil.args2dict(args,kw_args,Nysol_Mtonull.kwd))

