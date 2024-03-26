@echo off

start "" ".\x64\Release\ConsoleApplication1.exe"

start "" ".\server.exe" -m capybarahermes-2.5-mistral-7b.Q4_K_M.gguf -c 4096 --port 8080 --chat-template chatml -t 4
