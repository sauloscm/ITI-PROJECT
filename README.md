# Sistema de Gestão APS - Saúde da Família

Este repositório contém uma aplicação em Python desenvolvida como Trabalho de Conclusão de Curso (TCC) para suporte e inteligência estratégica na Atenção Primária à Saúde (UFMA). O projeto foi otimizado para realizar o cruzamento de dados de pacientes (e-SUS/PEC), calcular o Risco Familiar e gerar mapas e dashboards interativos para a gestão em saúde pública.

## Funcionalidades Principais

* **Classificação de Risco Familiar**: Implementa a Escala Coelho-Savassi automatizada com base nos dados extraídos do e-SUS.
* **Mapeamento Georreferenciado**: Utiliza mapas de calor e marcadores (Folium) para identificar áreas de vulnerabilidade (ex: alta concentração de HAS, DM ou idosos).
* **Dashboards Interativos**: Painel analítico gerado com Matplotlib e PyQt para visualização rápida de indicadores da equipe de saúde.
* **Sistema de Intervenções**: Gera sugestões de manejo clínico e ações de gestão baseadas no perfil epidemiológico da área.
* **Portal de Notícias Integrado**: Feed em tempo real com as últimas atualizações e campanhas do Ministério da Saúde.

## Configuração do Ambiente

O projeto exige estritamente o **Python 3.10** para garantir a compatibilidade e estabilidade das bibliotecas gráficas (PyQt5 e PyQtWebEngine) no Windows.

1. **Crie o arquivo de dependências**: Na raiz do projeto, garanta que existe um arquivo chamado `requirements.txt` com o seguinte conteúdo:

```text
pandas<2.3
numpy
matplotlib
PyQt5
PyQtWebEngine
folium
branca
requests
beautifulsoup4
```

2. **Configure o `.gitignore`**: Para não sobrecarregar o repositório com arquivos locais, crie um arquivo `.gitignore` com:

```text
venv/
__pycache__/
*.pyc
*.html
.vscode/
```

## Como Instalar o Python 3.10

Caso você não tenha o Python 3.10 ou utilize uma versão mais recente (como 3.12 ou 3.13), você pode instalá-lo lado a lado usando o gerenciador de pacotes nativo do Windows. Abra o PowerShell e execute:

```bash
winget install --id Python.Python.3.10 -e
```

> **Aviso:** Feche o terminal e abra um novo após a instalação para que o Windows reconheça o comando.

## Guia de Comandos

Siga a ordem abaixo no terminal (dentro da pasta do projeto) para isolar o ambiente e rodar o software sem conflitos.

### 1. Criação do Ambiente Virtual

**Criar a "bolha" forçando o Python 3.10:**

```bash
py -3.10 -m venv venv
```

### 2. Ativação do Ambiente

**Ativar o ambiente (Obrigatório toda vez que for usar):**

```bash
.\venv\Scripts\activate
```

> **Nota de Segurança:** Se o Windows bloquear a ativação apresentando um erro vermelho, execute o comando `Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process` e tente ativar novamente. O prefixo `(venv)` deve aparecer no terminal.

### 3. Instalação de Dependências

**Instalar as bibliotecas do `requirements.txt`:**

```bash
python -m pip install --upgrade pip
pip install -r requirements.txt
```

### 4. Execução do Sistema (GUI)

**Navegar até a pasta do código e rodar o software:**

```bash
cd TCC_software
python cerebro.py
```

## Estrutura do Projeto

* `cerebro.py`: Ponto de entrada principal, coordena as janelas e o processamento em segundo plano.
* `analise_dados.py`: Motor lógico de processamento Pandas e cálculo de risco Coelho-Savassi.
* `widget_mapa.py`: Geração e renderização do mapa HTML interativo usando Folium e WebEngine.
* `interface_dashboard.py`: Construção dos gráficos de pizza e KPIs dinâmicos da população.
* `interface_intervencao.py`: Lógica de busca ativa, alertas de gestão e sugestão de manejo.
* `interface_noticia.py`: Web scraping do portal GOV.BR e calendário de campanhas da saúde.
* `interface_upload.py`: Tela de "Drag and Drop" para importação dos relatórios CSV do e-SUS.
* `estilos.py`: Constantes de design e folhas de estilo CSS da interface moderna.
