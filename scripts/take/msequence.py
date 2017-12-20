#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.take.margs as margs 
import nysol.take.msequence as msequence


args=margs.Margs(sys.argv,"i=,c=,x=,O=,tid=,time=,item=,class=,taxo=,s=,S=,sx=,Sx=,g=,p=,-uniform,l=,u=,top=,gap=,win=,-padding,T=,-mcmdenv","i=")
msequence.msequence(args).run()

