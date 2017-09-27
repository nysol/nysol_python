# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mvdelnull(NysolMOD_CORE):
	kwd = n_core.getparalist("mvdelnull")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvdelnull,self).__init__("mvdelnull",nutil.args2dict(args,kw_args,Nysol_Mvdelnull.kwd))

