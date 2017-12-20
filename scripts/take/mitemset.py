#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.take.margs as margs
import nysol.take.mitemset as mitemset


args=margs.Margs(sys.argv,"i=,x=,O=,tid=,item=,class=,taxo=,type=,s=,S=,sx=,Sx=,g=,p=,-uniform,l=,u=,top=,T=,-replaceTaxo")
mitemset.mitemset(args).run()

