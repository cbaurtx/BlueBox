[build-menu]
FT_01_LB=_Build
FT_01_CM=idf.py -C ~/Development/Projects/BlueBox/Source/ build
FT_01_WD=
FT_02_LB=_Lint
FT_02_CM=cppcheck --language=c --enable=warning,style --template=gcc "%f"
FT_02_WD=
FT_00_LB=Memory map
FT_00_CM=idf.py -C ~/Development/Projects/BlueBox/Source/ size-components
FT_00_WD=
EX_00_LB=Monitor
EX_00_CM=xterm  -fa monaco -fs 14 -hold -e "cd ~/Development/Projects/BlueBox/Source/;. ~/Development/Projects/BlueBox/Source/adf_init  ;idf.py monitor
EX_00_WD=
EX_01_LB=Flash
EX_01_CM=cd ~/Development/Projects/BlueBox/Source/ ;. ~/Development/Projects/BlueBox/Source/adf_init  ;idf.py flash
EX_01_WD=
