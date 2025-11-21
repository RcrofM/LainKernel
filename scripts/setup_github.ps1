# Script para configurar e enviar o projeto para o GitHub
# Execute este script DEPOIS de instalar o Git

Write-Host "=== Setup do Git e GitHub ===" -ForegroundColor Cyan
Write-Host ""

# Verificar se o Git esta instalado
$gitInstalled = Get-Command git -ErrorAction SilentlyContinue

if (-not $gitInstalled) {
    Write-Host "X Git nao encontrado!" -ForegroundColor Red
    Write-Host "Por favor, instale o Git primeiro:" -ForegroundColor Yellow
    Write-Host "  https://git-scm.com/download/win" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Depois de instalar, reinicie o terminal e execute este script novamente." -ForegroundColor Yellow
    exit 1
}

$gitVersion = git --version
Write-Host "OK Git encontrado: $gitVersion" -ForegroundColor Green
Write-Host ""

# Verificar se ja esta configurado
$userName = git config --global user.name
$userEmail = git config --global user.email

if (-not $userName -or -not $userEmail) {
    Write-Host "Configurando o Git pela primeira vez..." -ForegroundColor Yellow
    Write-Host ""
    
    $name = Read-Host "Digite seu nome completo"
    $email = Read-Host "Digite seu email (use o mesmo do GitHub)"
    
    git config --global user.name "$name"
    git config --global user.email "$email"
    
    Write-Host "OK Git configurado!" -ForegroundColor Green
}
else {
    Write-Host "OK Git ja configurado:" -ForegroundColor Green
    Write-Host "  Nome: $userName"
    Write-Host "  Email: $userEmail"
}

Write-Host ""
Write-Host "=== Inicializando Repositorio ===" -ForegroundColor Cyan
Write-Host ""

# Verificar se ja e um repositorio Git
if (Test-Path ".git") {
    Write-Host "OK Repositorio Git ja existe" -ForegroundColor Green
}
else {
    Write-Host "Inicializando repositorio Git..." -ForegroundColor Yellow
    git init
    Write-Host "OK Repositorio inicializado!" -ForegroundColor Green
}

Write-Host ""
Write-Host "=== Adicionando Arquivos ===" -ForegroundColor Cyan
Write-Host ""

# Adicionar todos os arquivos
git add .

Write-Host "OK Arquivos adicionados ao staging" -ForegroundColor Green

# Mostrar status
Write-Host ""
Write-Host "Status do repositorio:" -ForegroundColor Yellow
git status --short

Write-Host ""
$commit = Read-Host "Fazer commit? (s/n)"

if ($commit -eq "s" -or $commit -eq "S") {
    git commit -m "Initial commit: SecureOS Kernel with modular architecture"
    Write-Host "OK Commit realizado!" -ForegroundColor Green
}

Write-Host ""
Write-Host "=== Proximos Passos ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "1. Crie um repositorio no GitHub:" -ForegroundColor Yellow
Write-Host "   https://github.com/new" -ForegroundColor Cyan
Write-Host ""
Write-Host "2. Configure o nome do repositorio (exemplo: 'secureos-kernel')" -ForegroundColor Yellow
Write-Host ""
Write-Host "3. Escolha se quer publico ou privado" -ForegroundColor Yellow
Write-Host ""
Write-Host "4. NAO marque 'Initialize with README'" -ForegroundColor Red
Write-Host ""
Write-Host "5. Depois de criar, execute estes comandos:" -ForegroundColor Yellow
Write-Host ""

$username = Read-Host "Digite seu username do GitHub (ou pressione Enter para pular)"

if ($username) {
    Write-Host ""
    Write-Host "Execute estes comandos:" -ForegroundColor Green
    Write-Host "  git remote add origin https://github.com/$username/secureos-kernel.git" -ForegroundColor Cyan
    Write-Host "  git branch -M main" -ForegroundColor Cyan
    Write-Host "  git push -u origin main" -ForegroundColor Cyan
    Write-Host ""
    
    $addRemote = Read-Host "Adicionar remote agora? (s/n)"
    
    if ($addRemote -eq "s" -or $addRemote -eq "S") {
        $repoName = Read-Host "Nome do repositorio no GitHub (padrao: secureos-kernel)"
        if (-not $repoName) {
            $repoName = "secureos-kernel"
        }
        
        $remoteExists = git remote get-url origin 2>$null
        
        if ($remoteExists) {
            Write-Host "Remote 'origin' ja existe. Removendo..." -ForegroundColor Yellow
            git remote remove origin
        }
        
        git remote add origin "https://github.com/$username/$repoName.git"
        git branch -M main
        Write-Host "OK Remote configurado!" -ForegroundColor Green
        Write-Host ""
        Write-Host "Agora execute: git push -u origin main" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "=== Concluido! ===" -ForegroundColor Green
Write-Host ""
Write-Host "Para mais informacoes, veja: GIT_SETUP_GUIDE.md" -ForegroundColor Cyan
