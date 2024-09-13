w = open('./spelunky_a.js', 'w')
for l in open('./SpelunkyClassicHD.js'):
    l = l.strip()
    if l.startswith('function gmlInitGlobal()'):
        w = open('./spelunky_b.js', 'w')
    w.write(l + '\n')