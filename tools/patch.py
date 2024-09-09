w = open('./src/spelunky_b2.js', 'w')
for l in open('./src/spelunky_b.js'):
    for fn in ['joi','ssi','zwh','yli','qli']:
        l = l.replace(fn + ".arguments", "arguments")
    if 'new Date()' in l:
        l = l.replace('new Date().getTime()', 'window.getTime()');
    if 'function (lsh, msh, fkl, pai, qai, rai, owh, gei)' in l:
        l = l + 'if (this.pbi == null) return;\npatch.drawText(this.pbi.rb, fkl, lsh, msh, 0, 0);\n}\n'
        l = l + '_xxx = () => {\n'
    if 'function rml' in l:
        l = 'function rml(jaj, lsh, msh, gei) {\n'
        l = l + 'patch.drawSprite(jaj, 0, 0, lsh, msh, 0, 0, 0, 0, 0, 0, 0, gei);\n'
        l = l + '}\nfunction _rml() {'
    if 'function vml' in l:
        l = 'function vml(jaj, ozi, pzi, lsh, msh, vol, wol, bdj, aci, bci, lfi, mfi, gei) {'
        l = l + 'patch.drawSprite(jaj,ozi,pzi,lsh,msh,vol,wol,bdj,aci,bci,lfi,mfi,gei);\n'
        l = l + '}\nfunction _vml() {\n'
    if 'rai = hxk(rai, 360.0)' in l:
        l = l + '\npatch.drawObject(this,this.izi[loi],this.gc,this.hc,rai,lsh,msh,pai,qai,zei,gei);\n'
    w.write(l)