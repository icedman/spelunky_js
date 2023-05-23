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
f('./src/spelunky.js')
f('./src/index.js')