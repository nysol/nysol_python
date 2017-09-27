# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mcal(NysolMOD_CORE):
	kwd = n_core.getparalist("mcal")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcal,self).__init__("mcal",nutil.args2dict(args,kw_args,Nysol_Mcal.kwd))

