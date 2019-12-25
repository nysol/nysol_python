import sys
from distutils.util import get_platform
plat_name = get_platform()
plat_specifier = ".%s-%d.%d" % (plat_name, *sys.version_info[:2])
print(plat_specifier)

import glob
import re
import chardet

def search(ext):
    pattern = re.compile('#include <(.*\..*)>')

    for cpp_path in glob.glob(f'**/*.{ext}', recursive=True):
        with open(cpp_path, 'rb') as f:
            detected = chardet.detect(f.read())

        with open(cpp_path, 'r') as f:
            if 'utf-8' in detected['encoding']:
                prerequisites = []  # 前提条件
                for line in f.readlines():
                    if '#include' in line:
                        # print(f'{cpp_path}\t{line.strip()}')
                        m = pattern.match(line)
                        if m and not 'boost' in m.groups()[0]:
                            prerequisites.append(m.groups()[0])
                            # print(f'{cpp_path}\t{m.groups()[0]}')
                if len(prerequisites) > 0:
                    print(f"{cpp_path}: {' '.join(prerequisites)}")

# search('cpp')
# search('h')

# import sysconfig
# import pprint
# pprint.pprint(sysconfig.get_config_vars('INCLUDEPY'))

