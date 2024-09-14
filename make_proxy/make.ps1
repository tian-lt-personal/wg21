param(
  [switch]$Clean,
  [switch]$Stop
)

if ($Clean) {
  Remove-Item -Path .\*.aux -Recurse -Force
  Remove-Item -Path .\*.fdb_latexmk -Recurse -Force
  Remove-Item -Path .\*.idx -Recurse -Force
  Remove-Item -Path .\*.ind -Recurse -Force
  Remove-Item -Path .\*.ilg -Recurse -Force
  Remove-Item -Path .\*.log -Recurse -Force
  Remove-Item -Path .\*.out -Recurse -Force
  Remove-Item -Path .\*.pdf -Recurse -Force
  Remove-Item -Path .\*.xtr -Recurse -Force
  Remove-Item -Path .\*.ext -Recurse -Force
  Remove-Item -Path .\*.glo -Recurse -Force
  Remove-Item -Path .\*.fls -Recurse -Force
}

if ($Stop) {
  return
}

latexmk -pdf make_proxy
