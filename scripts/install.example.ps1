$projectDir = Split-Path $PSScriptRoot -Parent

$coolqRoot = "C:\Users\Richard\Lab\酷Q Air" # 修改为你的酷Q目录
$appId = Get-Content "$projectDir\app_id.txt"
$appOutDir = $args[0]

$coolqAppDevDir = "$coolqRoot\dev\$appId"
$dllName = "app.dll"
$dllPath = "$appOutDir\$dllName"
$jsonName = "app.json"
$jsonPath = "$projectDir\$jsonName"

Write-Host "正在拷贝插件到酷Q应用目录……"
New-Item -Path $coolqAppDevDir -ItemType Directory -ErrorAction SilentlyContinue
Copy-Item -Force $dllPath "$coolqAppDevDir\$dllName"
Copy-Item -Force $jsonPath "$coolqAppDevDir\$jsonName"
Write-Host "拷贝完成" -ForegroundColor Green
