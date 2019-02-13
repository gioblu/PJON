%ECHO OFF
cls
for /L %%N in (2,1,%2) do (
    start TestClient %%N %1 %3
)
timeout 2
TestSender %1 %2
echo Total errors: %errorlevel%

