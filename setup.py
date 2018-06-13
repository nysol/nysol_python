from distutils.core import setup, Extension
import re
import os
import subprocess

args = ['xml2-config','--cflags']
xxxxs= subprocess.check_output(args).decode().rstrip().split()

hedears = ['src','src/kgmod','src/mod']
for xxx in xxxxs:
	hedears.append(re.sub(r'^-I','',xxx))


def checkLibRun(cc,fname,paras):
	for para in paras:
		try:
			with open(os.devnull, 'w') as fnull:
				exit_code = subprocess.call([cc, fname, "-l"+para],
                                    stdout=fnull, stderr=fnull)
		except OSError :
			exit_code = 1

		if exit_code == 0:
			return para

	return ""

def check_for_boost():

	import distutils.sysconfig
	import tempfile
	import shutil
	
	# Create a temporary directory
	tmpdir = tempfile.mkdtemp()
	curdir = os.getcwd()
	os.chdir(tmpdir)

	compiler = os.environ.get('CC', distutils.sysconfig.get_config_var('CC'))

	# make sure to use just the compiler name without flags
	compiler = compiler.split()[0]
	filename = 'test.cpp'
	with open(filename,'w') as f :
		f.write("""
			int main ()
			{
				return main ();
			  return 0;
			}
			""")

	boostFLG =[]
	boostLibLists=[
		['boost_thread-mt','boost_thread'],
		['boost_filesystem-mt','boost_filesystem'],
		['boost_regex-mt','boost_regex'],
		['boost_system-mt','boost_system'],
		['boost_date_time']
	]
	for boostlist in boostLibLists:

		boostflg = checkLibRun(compiler,filename, boostlist)
		if boostflg =="" : 	
			#err
			raise Exception("not find "+ " or ".join(boostlist))

		boostFLG.append(boostflg)

	# Clean up
	os.chdir(curdir)
	shutil.rmtree(tmpdir)

	return boostFLG


nmodLibs = ['pthread','xml2']
nmodLibs.extend(check_for_boost())
umodLibs = ['pthread']
umodLibs.extend(check_for_boost())

module1 = Extension('nysol/_nysolshell_core',
                    sources = ['src/mod/kgsortchk.cpp','src/mod/kg2tee.cpp','src/mod/kgfifo.cpp','src/mod/kgtrafld.cpp',
                    						'src/kgexcmd.cpp','src/nysolshell.cpp','src/mod/kg2cat.cpp',
                    						'src/kgshell.cpp','src/kgsplitblock.cpp',
                    						'src/kgload.cpp','src/kgpyfunc.cpp',
                    						'src/kgmod/kgArgFld.cpp',
																'src/kgmod/kgArgs.cpp','src/kgmod/kgCSV.cpp',
																'src/kgmod/kgCSVout.cpp','src/kgmod/kgCSVutils.cpp',
																'src/kgmod/kgEnv.cpp','src/kgmod/kgError.cpp',
																'src/kgmod/kgFldBuffer.cpp','src/kgmod/kgFldHash.cpp',
																'src/kgmod/kgFunction.cpp','src/kgmod/kgMessage.cpp',
																'src/kgmod/kgMethod.cpp','src/kgmod/kgRange.cpp',
																'src/kgmod/kgTempfile.cpp','src/kgmod/kgVal.cpp',
																'src/kgmod/kgWildcard.cpp','src/kgmod/kgmod.cpp',
																'src/kgmod/kgmodincludesort.cpp','src/kgmod/kgsortf.cpp',
																'src/mod/kgshuffle.cpp','src/mod/kgunicat.cpp',
																'src/mod/kgcal.cpp','src/mod/kgcut.cpp','src/mod/kgsum.cpp','src/mod/kgjoin.cpp',
																'src/mod/kg2cross.cpp','src/mod/kgaccum.cpp','src/mod/kgavg.cpp',
																'src/mod/kgbest.cpp','src/mod/kgbucket.cpp','src/mod/kgchgnum.cpp',
																'src/mod/kgchgstr.cpp','src/mod/kgcombi.cpp','src/mod/kgcommon.cpp',
																'src/mod/kgcount.cpp','src/mod/kgcross.cpp','src/mod/kgdelnull.cpp',
																'src/mod/kgdformat.cpp','src/mod/kgduprec.cpp','src/mod/kgfldname.cpp',
																'src/mod/kgfsort.cpp','src/mod/kghashavg.cpp','src/mod/kghashsum.cpp',
																'src/mod/kgkeybreak.cpp','src/mod/kgmbucket.cpp','src/mod/kgmvavg.cpp',
																'src/mod/kgmvsim.cpp','src/mod/kgmvstats.cpp','src/mod/kgnewnumber.cpp',
																'src/mod/kgnewrand.cpp','src/mod/kgnewstr.cpp','src/mod/kgnjoin.cpp',
																'src/mod/kgnormalize.cpp','src/mod/kgnrcommon.cpp','src/mod/kgnrjoin.cpp',
																'src/mod/kgnullto.cpp','src/mod/kgnumber.cpp','src/mod/kgpadding.cpp',
																'src/mod/kgpaste.cpp','src/mod/kgproduct.cpp','src/mod/kgrand.cpp',
																'src/mod/kgrjoin.cpp','src/mod/kgsed.cpp','src/mod/kgsel.cpp',
																'src/mod/kgselnum.cpp','src/mod/kgselrand.cpp','src/mod/kgselstr.cpp',
																'src/mod/kgsetstr.cpp','src/mod/kgshare.cpp','src/mod/kgsim.cpp',
																'src/mod/kgslide.cpp','src/mod/kgsplit.cpp','src/mod/kgstats.cpp',
																'src/mod/kgsummary.cpp','src/mod/kgtonull.cpp','src/mod/kgtra.cpp',
																'src/mod/kgtraflg.cpp','src/mod/kguniq.cpp','src/mod/kgcat.cpp',
																'src/mod/kgvcat.cpp','src/mod/kgvcommon.cpp','src/mod/kgvcount.cpp',
																'src/mod/kgvdelim.cpp','src/mod/kgvdelnull.cpp','src/mod/kgvjoin.cpp',
																'src/mod/kgvnullto.cpp','src/mod/kgvreplace.cpp','src/mod/kgvsort.cpp',
																'src/mod/kgvuniq.cpp','src/mod/kgwindow.cpp','src/mod/kgsep.cpp',
																'src/mod/kgarff2csv.cpp','src/mod/kgtab2csv.cpp','src/mod/kgxml2csv.cpp'
                    	],
										include_dirs=hedears,
										libraries=nmodLibs
										)

lcmmod = Extension('nysol/take/_lcmlib',
                    sources = ['src/take/lcmrap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/lcm']
										)

seqmod = Extension('nysol/take/_lcmseqlib',
                    sources = ['src/take/lcmseqrap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/lcmseq']
										)

seqmodzero = Extension('nysol/take/_lcmseq_zerolib',
                    sources = ['src/take/lcmseq_zerorap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/lcmseq_zero']
										)

lcmtransmod = Extension('nysol/take/_lcmtranslib',
                    sources = ['src/take/lcmtransrun.cpp'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod']
										)


sspcmod = Extension('nysol/take/_sspclib',
                    sources = ['src/take/sspcrap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/sspc']
										)
grhfilmod = Extension('nysol/take/_grhfillib',
                    sources = ['src/take/grhfilrap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/grhfil']
										)
macemod = Extension('nysol/take/_macelib',
                    sources = ['src/take/macerap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/mace']
										)
simsetmod = Extension('nysol/take/_simsetlib',
                    sources = ['src/take/simsetrap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/simset']
										)
medsetmod = Extension('nysol/take/_medsetlib',
                    sources = ['src/take/medsetrap.c'],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=['src','src/kgmod','src/mod','src/take/medset']
										)
utilmod = Extension('nysol/util/_utillib',
                    sources = ['src/util/mmethods.cpp',
                    						'src/kgmod/kgArgFld.cpp',
																'src/kgmod/kgArgs.cpp','src/kgmod/kgCSV.cpp',
																'src/kgmod/kgCSVout.cpp','src/kgmod/kgCSVutils.cpp',
																'src/kgmod/kgEnv.cpp','src/kgmod/kgError.cpp',
																'src/kgmod/kgFldBuffer.cpp',
																'src/kgmod/kgMessage.cpp',
																'src/kgmod/kgMethod.cpp',
																'src/kgmod/kgTempfile.cpp','src/kgmod/kgVal.cpp',
																'src/kgmod/kgWildcard.cpp','src/kgmod/kgmod.cpp',
																'src/kgmod/kgmodincludesort.cpp','src/kgmod/kgsortf.cpp'
										],
										include_dirs=['src','src/kgmod'],
										libraries=umodLibs
										)


vsopmod = Extension('nysol/vsop/_vsoplib',
                    sources = [ 'src/vsop/vsop.cpp',
                    'src/vsop/SAPPOROBDD/src/BDDc/bddc.c',
                    'src/vsop/SAPPOROBDD/src/BDD+/BDD.cc',
                    'src/vsop/SAPPOROBDD/src/BDD+/CtoI.cc',
                    'src/vsop/SAPPOROBDD/src/BDD+/ZBDD.cc',
                    'src/vsop/SAPPOROBDD/src/BDD+/ZBDDDG.cc',
                    'src/vsop/SAPPOROBDD/src/BDDLCM/lcm-vsop.cc',
										'src/vsop/SAPPOROBDD/app/VSOP/table.cc',
										'src/vsop/SAPPOROBDD/app/VSOP/print.cc'
                    ],
										define_macros=[('B_64', None),('_NO_MAIN_',None)],
 										extra_compile_args=['-Wno-format-security','-Wno-error'],
										include_dirs=[
											'src/vsop/SAPPOROBDD/include',
											'src/vsop/SAPPOROBDD/src/BDDLCM/',
											'src/vsop/SAPPOROBDD/app/VSOP'
											]
										)

setup(name = 'nysol',
			packages=['nysol','nysol/util','nysol/mod','nysol/mod/submod','nysol/mod/nysollib','nysol/take','nysol/vsop','nysol/take/lib','nysol/take/lib/base'],
			version = '0.1',
			description = 'This is a demo package',
			scripts=['scripts/take/mfriends.py','scripts/take/mitemset.py',
								'scripts/take/mpolishing.py','scripts/take/msequence.py',
								'scripts/take/mtra2gc.py','scripts/take/mpal.py',
								'scripts/take/mclique.py'],
			ext_modules =[module1,lcmmod,sspcmod,grhfilmod,macemod,seqmod,
											seqmodzero,lcmtransmod,macemod,simsetmod,medsetmod,vsopmod,utilmod]
			)
       
