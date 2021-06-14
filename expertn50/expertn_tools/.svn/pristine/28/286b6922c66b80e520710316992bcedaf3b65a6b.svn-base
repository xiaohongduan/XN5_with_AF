# -*- mode: python -*-
a = Analysis([os.path.join(HOMEPATH,'support\\_mountzlib.py'), os.path.join(HOMEPATH,'support\\useUnicode.py'), 'xpnhelper.py'],
             pathex=['C:\\Documents and Settings\\Christian\\My Documents\\projects\\expertn_tools'])
pyz = PYZ(a.pure)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=1,
          name=os.path.join('build\\pyi.win32\\xpnhelper', 'xpnhelper.exe'),
          debug=False,
          strip=False,
          upx=True,
          console=False )
coll = COLLECT( exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=False,
               upx=True,
               name=os.path.join('dist', 'xpnhelper'))
app = BUNDLE(coll,
             name=os.path.join('dist', 'xpnhelper.app'))
