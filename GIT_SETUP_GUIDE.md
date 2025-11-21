# Guia: Como Subir o Projeto para o GitHub

## Pré-requisitos

1. **Instalar o Git**
   - Baixe em: https://git-scm.com/download/win
   - Durante a instalação, aceite as opções padrão
   - Reinicie o terminal após instalar

2. **Criar conta no GitHub**
   - Acesse: https://github.com
   - Clique em "Sign up" e crie sua conta gratuita

## Passo 1: Configurar o Git (primeira vez apenas)

Abra o PowerShell ou terminal e execute:

```bash
git config --global user.name "Seu Nome"
git config --global user.email "seu-email@exemplo.com"
```

> ⚠️ Use o mesmo email da sua conta GitHub!

## Passo 2: Criar arquivo .gitignore

Já vou criar um arquivo `.gitignore` para você, que diz ao Git quais arquivos **não** devem ser enviados (como arquivos compilados).

## Passo 3: Inicializar o repositório Git local

No terminal, dentro da pasta do projeto (`c:\Users\Gabriel\Documents\project`):

```bash
# Inicializar o repositório Git
git init

# Adicionar todos os arquivos
git add .

# Fazer o primeiro commit
git commit -m "Initial commit: Kernel project with reorganized structure"
```

## Passo 4: Criar repositório no GitHub

1. Acesse: https://github.com/new
2. Preencha:
   - **Repository name**: `kernel-project` (ou o nome que preferir)
   - **Description**: "Custom kernel project with modular architecture"
   - **Public** ou **Private**: escolha se quer que seja público ou privado
   - ❌ **NÃO** marque "Initialize with README" (já temos um!)
3. Clique em **"Create repository"**

## Passo 5: Conectar seu projeto local ao GitHub

O GitHub vai mostrar comandos. Use estes (substitua `SEU-USUARIO` pelo seu username):

```bash
# Adicionar o repositório remoto
git remote add origin https://github.com/SEU-USUARIO/kernel-project.git

# Renomear branch para main (padrão atual)
git branch -M main

# Enviar o código para o GitHub
git push -u origin main
```

> 💡 O GitHub pode pedir suas credenciais. Use seu username e um **Personal Access Token** (não a senha).

## Passo 6: Criar Personal Access Token (se necessário)

Se o GitHub pedir senha ao fazer `git push`:

1. Acesse: https://github.com/settings/tokens
2. Clique em **"Generate new token"** → **"Generate new token (classic)"**
3. Dê um nome: "Git Push Token"
4. Marque o escopo: **`repo`** (acesso completo aos repositórios)
5. Clique em **"Generate token"**
6. **COPIE O TOKEN** (você só verá uma vez!)
7. Use o token como senha quando o Git pedir

## Comandos Úteis para o Futuro

### Fazer alterações e enviar para o GitHub:

```bash
# Ver o que mudou
git status

# Adicionar arquivos modificados
git add .

# Fazer commit com mensagem descritiva
git commit -m "Descrição das mudanças"

# Enviar para o GitHub
git push
```

### Ver histórico de commits:

```bash
git log --oneline
```

### Criar uma branch para experimentar:

```bash
git checkout -b nome-da-branch
```

## Estrutura Recomendada do README

Vou atualizar seu README.md para ficar mais profissional e atrativo no GitHub!

---

## Dicas Extras

### 🎯 Torne seu projeto mais atrativo:

1. **README.md completo** - com descrição, como compilar, screenshots
2. **LICENSE** - adicione uma licença (MIT é popular)
3. **CONTRIBUTING.md** - se quiser aceitar contribuições
4. **GitHub Topics** - adicione tags como `kernel`, `operating-system`, `c`, `x86`

### 📊 Mostre seu progresso:

- Use **GitHub Projects** para organizar tarefas
- Crie **Issues** para bugs e features
- Use **Releases** quando tiver versões estáveis

---

## Precisa de Ajuda?

Se tiver algum erro durante o processo, me avise! Posso te ajudar a resolver.
