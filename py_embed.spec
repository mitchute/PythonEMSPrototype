# -*- mode: python -*-

root_files = [] # eplaunch/runner.py']

a = Analysis(
    root_files,
    pathex=[],
    binaries=[],
    datas=[],
    hiddenimports=[],
    hookspath=[],
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=None
)

pyz = PYZ(
    a.pure, a.zipped_data,
    cipher=None
)
