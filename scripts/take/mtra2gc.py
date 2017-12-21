#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs 
import nysol.take.mtra2gc as mtra2gc

args=margs.Margs(sys.argv,"i=,no=,eo=,log=,tid=,item=,s=,S=,sim=,th=,-node_support,T=,-num","i=,tid=,item=,eo=")
mtra2gc.mtra2gc(args).run()


