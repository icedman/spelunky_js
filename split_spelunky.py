w = open('./src/spelunky_a.js', 'w')
for l in open('./src/spelunky/spelunky_raw.js'):
    l = l.strip()
    if l.startswith('function czb()'):
        w = open('./src/spelunky_b.js', 'w')
    w.write(l + '\n')