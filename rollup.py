import sys

hd = (len(sys.argv) > 1 and sys.argv[1] == '-h')
nd = (len(sys.argv) > 1 and sys.argv[1] == '-n')

def f(path):
    file = open(path, 'r')
    lines = file.readlines()
    for line in lines:
        if 'require(' in line:
            continue
        if 'module.exports' in line:
            break

        # if 'lcc(udc, "__view_xview__", 0) - ' in  line:
        #     line = line.replace('lcc(udc, "__view_xview__", 0) - ', 'lcc(udc, "__view_xview__", 0) - 1.25 * ')
        # if 'lcc(udc, "__view_yview__", 0) - ' in  line:
        #     line = line.replace('lcc(udc, "__view_yview__", 0) - ', 'lcc(udc, "__view_yview__", 0) - 1.25 * ')

        print(line.rstrip())

f('./src/monkey_patch.js')
if hd:
    f('./src/hd/tph_electronExtension.js')
    f('./src/hd/vph_html5Extension.js')
    f('./src/hd/uph_html5DisableLoadBar.js')
    f('./src/spelunky_hd.js')
    f('./src/monkey_patch_hd.js')
else:
    f('./src/spelunky_a.js')
    f('./src/spelunky_b2.js')
f('./src/index.js')