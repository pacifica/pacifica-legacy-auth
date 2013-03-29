set PATH=%PATH%;C:\wix;
del %PACIFICASDK%config\pacificaauthconfig.msi
del %PACIFICASDK%config\pacificaauthconfig.wixobj
candle -out %PACIFICASDK%config\pacificaauthconfig.wixobj %PACIFICASDK%config\pacificaauthconfig.wxs
light -out %PACIFICASDK%config\pacificaauthconfig.msi %PACIFICASDK%config\pacificaauthconfig.wixobj
