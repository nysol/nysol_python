#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take.mpolishing as mpolishing

args=margs.Margs(sys.argv,"ni=,nf=,ei=,ef=,-indirect,eo=,no=,th=,sim=,sup=,iter=,log=,O=")
mpolishing.mpolishing(args).run()



