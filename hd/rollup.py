import sys

def f(path):
    file = open(path, 'r')
    lines = file.readlines()
    for line in lines:
        print(line.rstrip())

f('./monkey_patch.js')
f('./tph_electronExtension.js')
f('./vph_html5Extension.js')
f('./uph_html5DisableLoadBar.js')
f('./spelunky_a.js')
f('./spelunky_b2.js')
f('./monkey_patch_hd.js')
f('./index.js')
