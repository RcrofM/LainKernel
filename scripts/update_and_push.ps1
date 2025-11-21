# Script para atualizar e enviar mudancas para o GitHub

Write-Host "=== Atualizando Repositorio LainKernel ===" -ForegroundColor Cyan
Write-Host ""

# Verificar status
Write-Host "Verificando mudancas..." -ForegroundColor Yellow
git status --short

Write-Host ""
$confirm = Read-Host "Deseja enviar essas mudancas para o GitHub? (s/n)"

if ($confirm -eq "s" -or $confirm -eq "S") {
    # Adicionar tudo
    Write-Host "Adicionando arquivos..." -ForegroundColor Yellow
    git add .
    
    # Commit
    Write-Host "Criando commit..." -ForegroundColor Yellow
    git commit -m "Rename project to LainKernel and improve documentation"
    
    # Push
    Write-Host "Enviando para o GitHub..." -ForegroundColor Yellow
    git push origin main
    
    if ($?) {
        Write-Host ""
        Write-Host "OK Sucesso! Mudancas enviadas." -ForegroundColor Green
    }
    else {
        Write-Host ""
        Write-Host "X Erro ao enviar. Verifique sua conexao ou credenciais." -ForegroundColor Red
    }
}
else {
    Write-Host "Operacao cancelada." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Pressione Enter para sair..."
Read-Host
