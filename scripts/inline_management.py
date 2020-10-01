import os
import shutil
from pathlib import Path
import ntpath

def copyGlob(source):
    for path in source:
        shutil.copy(path, os.path.join(datapath, ntpath.basename(path)))

# create path objects
datapath = Path(os.path.join(os.getcwd(), 'data'))
uipath = Path(os.path.join(os.getcwd(), 'management_ui'))

# remove dir if exists
if datapath.exists() and datapath.is_dir():
    shutil.rmtree(datapath)

# create dir
datapath.mkdir()

# copy html, css and js files
copyGlob(uipath.glob('*.html'));
copyGlob(uipath.glob('*.css'));
copyGlob(uipath.glob('*.js'));
