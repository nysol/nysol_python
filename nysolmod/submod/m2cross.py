# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_M2cross(NysolMOD_CORE):
	kwd = n_core.getparalist("m2cross")
	def __init__(self,*args, **kw_args) :
		super(Nysol_M2cross,self).__init__("m2cross",nutil.args2dict(args,kw_args,Nysol_M2cross.kwd))

