set PATH=%PATH%;c:\wix
heat dir "build\release" -srd -cg PacificaAuthRelease -gg -sfrag -dr INSTALLDIR -var var.PacificaAuthRelease -out pacificaauthrelease.wxs
rem Comment this out while wix heat is broken. || goto :error
candle pacificaauth.wxs pacificaauthrelease.wxs -dPacificaAuthRelease=build\release || goto :error
light -out pacificaauth pacificaauth.wixobj pacificaauthrelease.wixobj || goto :error
heat dir "go\pkg" -cg PacificaAuthGO -gg -sfrag -dr GODIR -var var.PacificaGO -out pacificaauthgo.wxs || goto :error
candle pacificaauthsdk.wxs pacificaauthgo.wxs -dPacificaGO=go\pkg || goto :error
light -out pacificaauthsdk pacificaauthsdk.wixobj pacificaauthgo.wixobj || goto :error
exit /b 0

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
