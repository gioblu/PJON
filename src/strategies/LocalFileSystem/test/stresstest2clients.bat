%ECHO OFF
cls
for /L %%N in (1,1,%2) do (
    start TestClient %%N %1
)
timeout 2
TestSender %1 %2
echo Total errors: %errorlevel%

