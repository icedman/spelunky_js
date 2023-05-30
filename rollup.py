import sys

hd = (len(sys.argv) > 1 and sys.argv[1] == '-h')

def f(path):
    file = open(path, 'r')
    lines = file.readlines()
    for line in lines:
        if 'require(' in line:
            continue
        if 'module.exports' in line:
            break
        print(line.rstrip())

f('./src/monkey_patch.js')
if hd:
    f('./src/hd/tph_electronExtension.js')
    f('./src/hd/vph_html5Extension.js')
    f('./src/hd/uph_html5DisableLoadBar.js')
    f('./src/spelunky_hd.js')
else:
    f('./src/spelunky.js')
f('./src/index.js')