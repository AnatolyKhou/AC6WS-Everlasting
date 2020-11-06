
set TARGT=C:\Users\Anatoly\OneDrive - Algolion.com\Projects\Everlasting\Deliveries\0200\

set SRCWS=E:\ALGOLiON\Dev\AC6WS Everlasting v3\

copy /Y "%SRCWS%AlgoEverMaster\Release\libAlgoEverMaster.a" "%TARGT%"
copy /Y "%SRCWS%AlgoEverSlave\Release\libAlgoEverSlave.a" "%TARGT%"
copy /Y "%SRCWS%AlgoEverSlave\LionSmart_Algolion.h" "%TARGT%"
copy /Y "%SRCWS%AlgoEverTest\src\main.c" "%TARGT%"
copy /Y "%SRCWS%Readme.docx" "%TARGT%"

