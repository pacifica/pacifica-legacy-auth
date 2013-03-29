start /wait msiexec /x pacificaauthsdk.msi /qn /l* "C:\Temp\pacificaauthsdk.uninstall.log"
start msiexec /x pacificaauth.msi /qn /l* "C:\Temp\pacificaauth.uninstall.log"
