# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mcount(NysolMOD_CORE):
	kwd = n_core.getparalist("mcount")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcount,self).__init__("mcount",nutil.args2dict(args,kw_args,Nysol_Mcount.kwd))

