w = open('./spelunky_b2.js', 'w')
for l in open('./spelunky_b.js'):
    if 'new Date()' in l:
        l = l.replace('new Date().getTime()', 'window.getTime()');
    # if 'function (lsh, msh, fkl, pai, qai, rai, owh, gei)' in l:
    #     l = l + 'if (this.pbi == null) return;\npatch.drawText(this.pbi.rb, fkl, lsh, msh, 0, 0);\n}\n'
    #     l = l + '_xxx = () => {\n'
    # if 'function _0s2' in l:
    #     l = 'function _0s2(jaj, lsh, msh, gei) {\n'
    #     l = l + 'patch.drawSprite(jaj, 0, 0, lsh, msh, 0, 0, 0, 0, 0, 0, 0, gei);\n'
    #     l = l + '}\nfunction ___0s2() {'
    # if 'function _2s2(' in l:
    #     l = 'function _2s2(jaj, ozi, pzi, lsh, msh, vol, wol, bdj, aci, bci, lfi, mfi, gei) {'
    #     l = l + 'patch.drawSprite(jaj,ozi,pzi,lsh,msh,vol,wol,bdj,aci,bci,lfi,mfi,gei);\n'
    #     l = l + '}\nfunction ___2s2(\n'
    # if 'rai = hxk(rai, 360.0)' in l:
    #     l = l + '\npatch.drawObject(this,this.izi[loi],this.gc,this.hc,rai,lsh,msh,pai,qai,zei,gei);\n'
    w.write(l)