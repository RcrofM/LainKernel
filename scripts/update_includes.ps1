# Script para atualizar includes

Write-Host "Atualizando includes dos arquivos..."

# security.c
$content = Get-Content "src\security\security.c" -Raw
$content = $content -replace '#include "security.h"', '#include "security.h"`n#include "../lib/string.h"`n#include <kernel/memory.h>`n#include "random.h"`n#include "audit.h"'
Set-Content "src\security\security.c" -Value $content

# random.c
$content = Get-Content "src\security\random.c" -Raw
$content = $content -replace '#include "random.h"', '#include "random.h"`n#include <kernel/kernel.h>'
Set-Content "src\security\random.c" -Value $content

# audit.c
$content = Get-Content "src\security\audit.c" -Raw
$content = $content -replace '#include "audit.h"', '#include "audit.h"`n#include <kernel/kernel.h>`n#include "../drivers/vga.h"`n#include "../lib/string.h"'
Set-Content "src\security\audit.c" -Value $content

# process.c
$content = Get-Content "src\process\process.c" -Raw
$content = $content -replace '#include "process.h"', '#include "process.h"`n#include <kernel/kernel.h>`n#include <kernel/memory.h>`n#include "../lib/string.h"'
Set-Content "src\process\process.c" -Value $content

# syscall.c
$content = Get-Content "src\process\syscall.c" -Raw
$content = $content -replace '#include "syscall.h"', '#include "syscall.h"`n#include <kernel/kernel.h>`n#include <kernel/memory.h>`n#include "../security/security.h"`n#include "../drivers/vga.h"`n#include "process.h"`n#include "../lib/string.h"`n#include "../security/audit.h"'
Set-Content "src\process\syscall.c" -Value $content

# string.c
$content = Get-Content "src\lib\string.c" -Raw
$content = $content -replace '#include "string.h"', '#include "string.h"'
Set-Content "src\lib\string.c" -Value $content

Write-Host "Includes atualizados com sucesso!"
