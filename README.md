# Front-End Compilateur Micro en Deux Passes

<div align="center">

![Langage](https://img.shields.io/badge/langage-C-blue?style=for-the-badge&logo=c)
![Licence](https://img.shields.io/badge/licence-Académique-green?style=for-the-badge)
![Statut](https://img.shields.io/badge/statut-Éducatif-orange?style=for-the-badge)
![Phase](https://img.shields.io/badge/phase-Lexer%20%2B%20Parser-purple?style=for-the-badge)
![Build](https://img.shields.io/badge/build-CodeBlocks%20%7C%20GCC-red?style=for-the-badge)

**Un front-end complet de compilateur en deux passes pour le langage Micro**

*Analyse lexicale et analyse syntaxique par descente récursive — en C*

[Aperçu](#-aperçu) •
[Architecture](#-architecture-en-deux-passes) •
[Compilation](#-compilation-du-projet) •
[Utilisation](#-exécution-du-compilateur) •
[Théorie](#-contexte-académique--théorie) •
[Extensions](#-extensions-suggérées-pour-la-recherche)

</div>

---

## 📋 Table des Matières

<details>
<summary>Cliquez pour développer</summary>

- [Aperçu](#-aperçu)
- [Structure du Dépôt](#-structure-du-dépôt)
- [Architecture en Deux Passes](#-architecture-en-deux-passes)
- [Le Langage Micro](#-le-langage-micro)
- [Spécification des Tokens](#-spécification-des-tokens)
- [Analyse Détaillée du Code](#-analyse-détaillée-du-code)
- [Compilation du Projet](#-compilation-du-projet)
- [Exécution du Compilateur](#-exécution-du-compilateur)
- [Exemple d'Entrée & Sortie Attendue](#-exemple-dentrée--sortie-attendue)
- [Gestion et Récupération des Erreurs](#-gestion-et-récupération-des-erreurs)
- [Comparaison avec les Compilateurs Réels](#-comparaison-avec-les-compilateurs-réels)
- [Contexte Académique & Théorie](#-contexte-académique--théorie)
- [Limitations & Problèmes Connus](#-limitations--problèmes-connus)
- [Extensions Suggérées](#-extensions-suggérées-pour-la-recherche)
- [Stratégie de Test](#-stratégie-de-test)
- [Licence & Remerciements](#-licence--remerciements)
- [Carte de Référence Rapide](#-carte-de-référence-rapide)

</details>

---

## 🔍 Aperçu

Ce projet est un **front-end de compilateur en deux passes** pour le langage **Micro**, implémenté en C. Il réalise les deux premières phases canoniques de la compilation :

1. **Passe 1 — Analyse lexicale** : lit `input.txt`, produit un flux de tokens dans `sortie_analyseur.txt`.
2. **Passe 2 — Analyse syntaxique** : relit le fichier de tokens et le valide contre la grammaire Micro via une **descente récursive**, en signalant les erreurs de syntaxe avec les numéros de ligne.

C'est une implémentation pédagogique conçue pour refléter la façon dont les compilateurs réels (GCC, Clang, MSVC, javac) séparent les préoccupations entre lexing et parsing.

### Pipeline

```mermaid
graph LR
    A[input.txt] --> B[PASSE 1 : SCANNER]
    B --> C[sortie_analyseur.txt]
    C --> D[PASSE 2 : PARSER]
    D --> E[diagnostics]
    
    style B fill:#4a90d9,stroke:#2c5aa0,color:#fff
    style D fill:#9b59b6,stroke:#7d3c98,color:#fff
    style E fill:#f5a623,stroke:#d4891a,color:#fff
```

### Le Langage Supporte

| Catégorie | Éléments |
|-----------|----------|
| **Mots-clés** | `begin`, `end`, `read`, `write` *(insensible à la casse)* |
| **Identificateurs** | `[a-zA-Z][a-zA-Z0-9_]*` |
| **Littéraux Entiers** | `[0-9]+` |
| **Littéraux Flottants** | `13.7`, `.5`, `5e-2`, `1.5E+3` |
| **Opérateurs** | `:=`, `+`, `-` |
| **Ponctuation** | `(`, `)`, `;`, `,` |
| **Commentaires** | `--` jusqu'à la fin de ligne |
| **Fin de Fichier** | `SCANEOF` |

---

## 📁 Structure du Dépôt

```
.
├── bin/
│   └── Debug/
│       └── TP_TP_TP.exe        # Binaire compilé (Windows)
├── obj/
│   └── Debug/
│       ├── main.o              # Fichier objet pour main.c
│       └── parser.o            # Fichier objet pour parser.c
├── 📄 main.c                   # Passe 1 : analyseur lexical + driver
├── 📄 parser.c                 # Passe 2 : parseur à descente récursive
├── 📄 parser.h                 # Enum de tokens partagé + prototypes
├── 📝 input.txt                # Entrée de test (avec erreurs délibérées)
├── 📤 sortie_analyseur.txt     # Flux de tokens produit par la Passe 1
├── 🔧 TP.cbp                   # Fichier projet Code::Blocks
├── 🔧 TP.depend                # Cache de dépendances Code::Blocks
├── 🔧 TP.layout                # Cache de layout Code::Blocks
├── 🔧 TP_TP_TP.depend          # Cache de dépendances alternatif
├── 🔧 TP_TP_TP.layout          # Cache de layout alternatif
└── 📦 Analyseur_lexical_2pass.xml # Snapshot Repomix du dépôt
```

> **Remarque :** Les fichiers binaires (`bin/Debug/TP_TP_TP.exe`, `obj/Debug/*.o`) sont des artefacts compilés et sont exclus de la représentation packagée.

---

## 🏗️ Architecture en Deux Passes

### Pourquoi deux passes ?

Un front-end **en deux passes** sépare le **lexing** du **parsing** via un fichier intermédiaire (`sortie_analyseur.txt`). Cela offre plusieurs avantages :

| Avantage | Explication |
|----------|-------------|
| **Modularité** | Le scanner et le parseur sont des programmes séparés communiquant par fichier. |
| **Débogabilité** | Le flux de tokens peut être inspecté indépendamment du parseur. |
| **Réutilisabilité** | Le fichier de tokens peut alimenter plusieurs parseurs (grammaires différentes). |
| **Pédagogie** | Les étudiants peuvent voir la sortie de chaque phase isolément. |
| **Pipeline de compilation** | Reflète les pipelines réels avec représentations intermédiaires. |

### Pourquoi la plupart des compilateurs réels utilisent une seule passe

Les compilateurs modernes (GCC, Clang, javac) **entrelacent** typiquement lexing et parsing (une passe), car :

- Éviter l'I/O sur le flux de tokens est **plus rapide**.
- Le parseur doit souvent donner un retour au lexer (ex. désambiguïsation de `>>` en C++).
- Les diagnostics peuvent être émis immédiatement.

> Ce projet utilise délibérément deux passes pour la **clarté et l'enseignement**.

### Pipeline Détaillé

```
┌──────────────┐    ┌──────────────────┐    ┌────────────────────┐    ┌──────────────┐
│  input.txt   │───►│  PASSE 1 : LEXER │───►│ sortie_analyseur   │───►│ PASSE 2 :    │
│  (source)    │    │  scanner()       │    │ .txt (tokens)      │    │ PARSER       │
└──────────────┘    └──────────────────┘    └────────────────────┘    └──────────────┘
                                                                              │
                                                                              ▼
                                                                       diagnostics sur
                                                                       stdout
```

---

## 🔤 Le Langage Micro

Micro est un langage impératif minimal utilisé dans les cours de construction de compilateurs, inspiré par *Compiler Construction: Principles and Practice* de **Kenneth C. Louden**. Sa grammaire est un sous-ensemble de type Pascal :

```
<system_goal>  → <program> SCANEOF
<program>      → "begin" <inst_list> "end"
<inst_list>    → <inst> { <inst> }
<inst>         → id ":=" <expr> ";"
               | "read" "(" <id_list> ")" ";"
               | "write" "(" <expr_list> ")" ";"
<id_list>      → id { "," id }
<expr_list>    → <expr> { "," <expr> }
<expr>         → <prim> { <add_op> <prim> }
<add_op>       → "+" | "-"
<prim>         → "(" <expr> ")"
               | id
               | int_literal
               | float_literal
```

### Conventions Lexicales

| Élément | Motif | Exemple |
|---------|-------|---------|
| Mot-clé | mot réservé *(insensible à la casse)* | `begin`, `END` |
| Identificateur | `lettre (lettre \| chiffre \| _)*` | `x`, `var1`, `_t` |
| Entier | `chiffre+` | `52`, `0` |
| Flottant | `chiffre+ . chiffre+ [exposant]` | `13.7`, `5e-2` |
| Affectation | `:=` | `:=` |
| Plus / Moins | `+` / `-` | `+`, `-` |
| Parenthèses | `(` / `)` | `(`, `)` |
| Point-virgule | `;` | `;` |
| Virgule | `,` | `,` |
| Commentaire | `--` … fin de ligne | `-- ceci est ok` |
| Espace blanc | espace, tabulation, saut de ligne | *(ignoré)* |

---

## 🏷️ Spécification des Tokens

Les tokens sont définis dans `parser.h` comme un `enum` C :

```c
typedef enum enu {
    BEGIN, END, READ, WRITE,
    ID, INTLITERAL,
    LPAREN, RPAREN,
    SEMICOLON, COMMA,
    ASSIGNOP, PLUSOP, MINUSOP,
    SCANEOF,
    FLOATLITERAL
} token;
```

Leurs noms imprimables sont stockés dans `parser.c` :

```c
char tokens[15][13] = {
    "BEGIN", "END", "READ", "WRITE", "ID", "INTLITERAL",
    "LPAREN", "RPAREN", "SEMICOLON", "COMMA", "ASSIGNOP",
    "PLUSOP", "MINUSOP", "SCANEOF", "FLOATLITERAL"
};
```

> **Remarque :** `main.c` et `parser.c` utilisent tous deux `tokens[]` — le tableau est défini dans `parser.c` et déclaré de manière `extern`-like dans `parser.h`. C'est un pattern courant (mais légèrement fragile) dans les petits projets C.

---

## 📖 Analyse Détaillée du Code

### `main.c` — Analyseur Lexical

#### État Global

```c
int ligneid = 1;      /* numéro de ligne courant */
int newline = 0;      /* si on a vu une nouvelle ligne à émettre */

typedef struct {
    char token_tab[MAX];  /* MAX = 50 */
    int indice;
} token_buffer;

token_buffer buffer;
```

| Variable | Rôle |
|----------|------|
| `ligneid` | Suit la ligne courante pour les diagnostics |
| `newline` | Signale au driver d'émettre un `\n` dans le fichier de tokens |
| `buffer` | Accumule le lexème courant |

#### Aides de Tampon

```c
void clear_buffer() { buffer.indice = 0; }

void buffer_char(int car) {
    buffer.token_tab[buffer.indice] = (char)car;
    buffer.token_tab[buffer.indice + 1] = '\0';
    buffer.indice++;
}
```

> **Note :** Contrairement à la version précédente dans `essay.c`, ceci utilise un `indice` plutôt que d'appeler `strlen` sur chaque caractère — un ajout **O(1)**. C'est une vraie amélioration.

#### Pliage de Casse & Reconnaissance de Mots-Clés

```c
void to_maj() {
    for (int i = 0; i < buffer.indice; i++)
        if (buffer.token_tab[i] >= 'a' && buffer.token_tab[i] <= 'z')
            buffer.token_tab[i] = 'A' + buffer.token_tab[i] - 'a';
}

token check_reserved() {
    to_maj();
    if (!strcmp(buffer.token_tab, "BEGIN")) return BEGIN;
    if (!strcmp(buffer.token_tab, "END"))   return END;
    if (!strcmp(buffer.token_tab, "READ"))  return READ;
    if (!strcmp(buffer.token_tab, "WRITE")) return WRITE;
    return ID;
}
```

Les mots-clés sont **insensibles à la casse** — `begin`, `BEGIN`, `Begin` fonctionnent tous. Cela correspond aux langages de type Pascal.

#### Aides Numériques

```c
void read_number(FILE* myFile, char* c) {
    for ((*c) = fgetc(myFile); isdigit(*c); (*c) = fgetc(myFile))
        buffer_char(*c);
    ungetc(*c, myFile);
}

void exposant(FILE* myFile, char* c) {
    buffer_char(*c);
    (*c) = fgetc(myFile);
    if ((!isdigit(*c)) && (*c != '+') && (*c != '-'))
        ungetc(*c, myFile);
    else {
        if ((*c == '+') || (*c == '-')) {
            buffer_char(*c);
            (*c) = fgetc(myFile);
        }
        if (isdigit(*c)) read_number(myFile, c);
        else ungetc(*c, myFile);
    }
}
```

`read_number` consomme une suite de chiffres. `exposant` gère la partie exposant d'un flottant, y compris le signe optionnel.

#### Le Scanner

```c
token scanner(FILE *inputFile) {
    int in_char, c = 0;
    clear_buffer();
    if (feof(inputFile)) return SCANEOF;

    while ((in_char = fgetc(inputFile)) != EOF) {
        if (in_char == '\n') {
            ligneid++;
            if (!newline) continue;
            newline = 0;
            return -1;          /* signal de saut de ligne */
        }
        if (isspace(in_char)) continue;

        if (isalpha(in_char)) {
            buffer_char(in_char);
            for (c = fgetc(inputFile); isalnum(c) || c == '_'; c = fgetc(inputFile))
                buffer_char(c);
            ungetc(c, inputFile);
            return check_reserved();
        }
        else if (isdigit(in_char)) {
            buffer_char(in_char);
            read_number(inputFile, &c);
            if (c == '.') {
                c = fgetc(inputFile);
                read_number(inputFile, &c);
                if ((c != 'e') && (c != 'E')) return FLOATLITERAL;
            }
            if ((c == 'e') || (c == 'E')) {
                c = fgetc(inputFile);
                exposant(inputFile, &c);
                return FLOATLITERAL;
            }
            return INTLITERAL;
        }
        else if (in_char == '.') {
            c = fgetc(inputFile);
            if (isdigit(c)) {
                read_number(inputFile, &c);
                if ((c == 'e') || (c == 'E')) {
                    c = fgetc(inputFile);
                    exposant(inputFile, &c);
                }
                return FLOATLITERAL;
            } else {
                ungetc(c, inputFile);
                lexical_error('.');
            }
        }
        else if (in_char == '(') return LPAREN;
        else if (in_char == ')') return RPAREN;
        else if (in_char == ';') return SEMICOLON;
        else if (in_char == ',') return COMMA;
        else if (in_char == '+') return PLUSOP;

        else if (in_char == ':') {
            c = fgetc(inputFile);
            if (c == '=') return ASSIGNOP;
            else { ungetc(c, inputFile); lexical_error(in_char); }
        }
        else if (in_char == '-') {
            c = fgetc(inputFile);
            if (c == '-') {                     /* commentaire */
                do in_char = fgetc(inputFile);
                while (in_char != '\n');
                ligneid++;
            } else {
                ungetc(c, inputFile);
                return MINUSOP;
            }
        }
        else lexical_error(in_char);
    }
    return SCANEOF;
}
```

Caractéristiques clés :

| Fonctionnalité | Implémentation |
|----------------|----------------|
| Suivi de ligne | `ligneid++` sur `\n` et après commentaires |
| Token de saut de ligne | Retourne `-1` pour signaler « émettre un saut de ligne » |
| Espace blanc | Ignoré, sauf `\n` qui est suivi |
| Identificateurs | Début `isalpha`, puis `isalnum` / `_` |
| Entiers | Début `isdigit`, puis `isdigit` |
| Flottants | Fraction + exposant, tous deux supportés |
| Flottants à point initial | `.5` reconnu comme `FLOATLITERAL` |
| Affectation | `:=` uniquement ; `:` seul est une erreur lexicale |
| Commentaires | `--` consommé jusqu'à la fin de ligne |
| EOF | `SCANEOF` |

#### Le Driver : `analyseurlex()`

```c
void analyseurlex() {
    FILE* myFile = fopen("input.txt", "r");
    FILE* output = fopen("sortie_analyseur.txt", "w");
    if (!myFile || !output) { printf("Error! ...\n"); exit(-1); }

    while (strcmp(tokens[i = scanner(myFile)], "SCANEOF")) {
        if (i >= 0) {
            fprintf(output, "%s ", tokens[i]);
            newline = 1;
        } else {
            fprintf(output, "%c", '\n');
        }
    }
    fprintf(output, "%s", tokens[13]);   /* SCANEOF */
    fclose(output);
    fclose(myFile);
}
```

- Boucle jusqu'à ce que `scanner()` retourne `SCANEOF`.
- Sur un vrai token (`i >= 0`), écrit son nom suivi d'un espace.
- Sur `-1` (saut de ligne), écrit `\n` et réinitialise `newline`.
- Écrit `SCANEOF` à la fin.

> **Bug :** `tokens[]` est indexé même quand `i` peut être `-1` dans `strcmp(tokens[i = scanner(myFile)], "SCANEOF")`. Comme `-1` n'est pas un index valide, c'est un **comportement indéfini**. En pratique, la plupart des compilateurs lisent accidentellement le dernier élément de `tokens`, ce qui explique pourquoi ça « marche ». C'est un vrai bug à corriger (voir [Limitations](#-limitations--problèmes-connus)).

#### `main()`

```c
int main() {
    analyseurlex();
    printf("Fin analyseur lexical\n");
    system_goal();
    return 0;
}
```

Exécute la Passe 1, puis la Passe 2, dans un seul processus.

---

### `parser.c` — Analyseur Syntaxique

#### État Partagé

```c
FILE* output = NULL;    /* réutilise le nom "output", mais ouvre en lecture */
token courant;          /* token courant */
int nb = 1;             /* numéro de ligne courant dans le fichier de tokens */
char c;                 /* dernier caractère lu par next_token (détection \n) */

char tokens[15][13] = { ... };
```

#### `next_token()` — Lit le Fichier de Tokens

```c
token next_token() {
    char a;
    char *token_name = malloc(20 * sizeof(char));
    fscanf(output, "%s%c%c", token_name, &a, &c);
    if (c != '\n') ungetc(c, output);
    else nb++;

    if (!strcmp(token_name, "BEGIN")) return BEGIN;
    /* ... de nombreux appels strcmp ... */
    if (!strcmp(token_name, "FLOATLITERAL")) return FLOATLITERAL;
    return 15;   /* token inconnu */
}
```

C'est un **décodeur chaîne-nom-de-token → enum**. Il utilise `fscanf` avec `%s%c%c` pour lire :
- `token_name` — le nom du token.
- `a` — l'espace après le token.
- `c` — le caractère suivant (pour détecter `\n`).

Si `c` n'est pas `\n`, il est repoussé. Sinon, le compteur de ligne `nb` est incrémenté.

> **Fuite mémoire :** `token_name` est alloué avec `malloc` à chaque appel et **jamais libéré**. Un parseur de production éviterait cela. Voir [Limitations](#-limitations--problèmes-connus).

#### `match()` — Le Cœur du Parseur

```c
void match(token t) {
    token to = courant;
    if (to == t) {
        courant = next_token();
    } else {
        printf("Found %s ", tokens[to]);
        syntax_error(t);
        /* NOTE : n'avance PAS */
    }
}
```

- En cas de correspondance, avance.
- En cas de non-correspondance, signale l'erreur mais **n'avance pas** — cela peut causer des erreurs en cascade, mais évite aussi de sauter des tokens valides.

#### `system_goal()` — Le Point d'Entrée

```c
void system_goal() {
    output = fopen("sortie_analyseur.txt", "r");
    if (!output) { printf("ERREUR D'OUVERTURE !!!!"); exit(-1); }
    program();
    match(SCANEOF);
    fclose(output);
}
```

Ouvre le fichier de tokens et lance le parseur.

#### Fonctions de Grammaire

Toutes les règles de grammaire sont transcrites directement en fonctions :

| Fonction | Règle de Grammaire |
|----------|-------------------|
| `program()` | `begin inst_list end` |
| `inst_list()` | `inst { inst }` |
| `inst()` | `id := expr ;`, `read ( id_list ) ;`, `write ( expr_list ) ;` |
| `id_list()` | `id { , id }` |
| `expr_list()` | `expr { , expr }` |
| `expression()` | `prim { add_op prim }` |
| `add_op()` | `+` ou `-` |
| `prim()` | `( expr )`, `id`, `int`, `float` |

#### `inst_list()` — Avec Dispatch

```c
void inst_list() {
    inst();
    while (1) {
        switch (courant) {
            case ID:
            case READ:
            case WRITE:
                inst();
                break;
            case END:
            case SCANEOF:
                return;
            default:
                syntax_error(-1);
                courant = next_token();
                break;
        }
    }
}
```

Forme **itérative** classique de `inst { inst }`, avec récupération d'erreur (saute les tokens inconnus).

#### `prim()` — Avec Récupération en Mode Panique

```c
void prim() {
    token tok = courant;
    switch (tok) {
        case LPAREN: match(LPAREN); expression(); match(RPAREN); break;
        case ID: match(ID); break;
        case INTLITERAL: match(INTLITERAL); break;
        case FLOATLITERAL: match(FLOATLITERAL); break;
        case END: return;
        case SCANEOF: printf("Found %s", tokens[courant]); break;
        default:
            while (courant != RPAREN && courant != COMMA && courant != READ) {
                if (courant == END || courant == SCANEOF) {
                    printf("Found unexpected %s ", tokens[courant]);
                    break;
                }
                printf("Found %s ", tokens[courant]);
                syntax_error(-1);
                if (courant == SEMICOLON) break;
                else courant = next_token();
            }
            break;
    }
}
```

C'est la **récupération en mode panique** : en cas de token inattendu, saute vers l'avant jusqu'à un token de synchronisation (`)`, `,`, `read`, `end`, EOF, ou `;`).

#### `id_list()` — Avec Récupération Détaillée

```c
void id_list() {
    int contin = 1;
    while (contin) {
        switch (courant) {
            case ID: match(ID); contin = 0; break;
            case COMMA: syntax_error(4); contin = 0; break;
            case RPAREN: syntax_error(4); contin = 0; break;
            case END: return;
            case SCANEOF: printf("Found %s", tokens[courant]); contin = 0; break;
            default:
                printf("Found %s", tokens[courant]);
                courant = next_token();
        }
    }
    while (courant == COMMA) {
        match(COMMA);
        if (courant != ID) {
            if (courant == COMMA || courant == RPAREN) syntax_error(4);
            else {
                printf("FOUND %s ", tokens[courant]);
                syntax_error(4);
                courant = next_token();
            }
        } else match(ID);
    }
}
```

#### `syntax_error()` — Le Rapporteur

```c
void syntax_error(token t) {
    int temp;
    if (c == '\n') temp = nb - 1;
    else temp = nb;
    switch (t) {
        case -1: printf("Expression manquante ou erreur de syntaxe ligne %d\n", temp); break;
        default: printf("Excpected %s ligne %d\n", tokens[t], temp);
    }
}
```

Signale le token attendu (ou un message générique « expression manquante ») et le numéro de ligne.

---

### `parser.h` — Interface Partagée

```c
#include <stdio.h>
#include <stdlib.h>

typedef enum enu {
    BEGIN, END, READ, WRITE, ID, INTLITERAL, LPAREN, RPAREN,
    SEMICOLON, COMMA, ASSIGNOP, PLUSOP, MINUSOP, SCANEOF, FLOATLITERAL
} token;

char tokens[15][13];

void system_goal();
void program();
void match(token t);
token next_token();
void syntax_error(token t);
void inst_list();
void inst();
void prim();
void id_list();
void expression();
void expr_list();
int add_op();
```

> ⚠️ **Attention :** `char tokens[15][13];` est déclaré (pas `extern`) dans `parser.h`. `main.c` et `parser.c` l'incluent tous deux. En C, c'est une **définition tentative** — cela fonctionne parce qu'une unité de traduction fournit l'initialiseur et l'autre une copie zéro-initialisée. Ce n'est **pas** strictement correct et peut causer des erreurs de linker sur certains compilateurs. La correction est `extern char tokens[15][13];` dans l'en-tête, avec la définition dans exactement un fichier `.c`.

---

## 🔨 Compilation du Projet

### Avec Code::Blocks

Le projet est livré avec `TP.cbp` — un fichier projet Code::Blocks.

1. Ouvrir **Code::Blocks**.
2. `File → Open → TP.cbp`.
3. Sélectionner la cible **Debug**.
4. `Build → Build` (ou `Ctrl+F9`).
5. `Build → Run` (ou `Ctrl+F10`).

Binaire de sortie : `bin/Debug/TP_TP_TP.exe` (Windows) ou `bin/Debug/TP_TP_TP` (Linux/macOS).

> Le `.cbp` spécifie `-Wall` et `-g` pour Debug, `-O2 -s` pour Release.

### Avec GCC en Ligne de Commande

```bash
gcc -Wall -Wextra -O2 -o TP_TP_TP main.c parser.c
```

- `main.c` et `parser.c` doivent être compilés et liés ensemble.
- `parser.h` est inclus mais non compilé.

#### Build de Débogage (recommandé)

```bash
gcc -Wall -Wextra -g -fsanitize=address,undefined -o TP_TP_TP_dbg main.c parser.c
```

Le build avec sanitizer est **fortement recommandé** — il attrape le bug d'indexation `tokens[-1]` dans `analyseurlex()` et tout problème mémoire.

#### Windows (MinGW)

```bash
gcc -Wall -Wextra -O2 -o TP_TP_TP.exe main.c parser.c
```

### Avec Make

Créez un `Makefile` :

```make
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11
TARGET  = TP_TP_TP
SRC     = main.c parser.c

all: $(TARGET)

$(TARGET): $(SRC) parser.h
	$(CC) $(CFLAGS) -o $@ $(SRC)

debug: $(SRC) parser.h
	$(CC) $(CFLAGS) -g -fsanitize=address,undefined -o $(TARGET)_dbg $(SRC)

clean:
	rm -f $(TARGET) $(TARGET)_dbg *.o

.PHONY: all debug clean
```

Puis :

```bash
make          # build release
make debug    # build avec sanitizer
make clean    # supprimer les artefacts
```

---

## 🚀 Exécution du Compilateur

Le programme lit `input.txt` et écrit `sortie_analyseur.txt` (les deux chemins sont **codés en dur**).

### Utilisation de Base

```bash
./TP_TP_TP
```

Vous devriez voir :

```
Fin analyseur lexical
Found ...
Excpected ...
...
```

- `Fin analyseur lexical` — la Passe 1 est terminée.
- Le reste — les erreurs de syntaxe de la Passe 2 (s'il y en a).

### Inspection du Flux de Tokens

```bash
cat sortie_analyseur.txt
```

### Sous Windows

```cmd
bin\Debug\TP_TP_TP.exe
type sortie_analyseur.txt
```

### Avec le Build Sanitizer

```bash
./TP_TP_TP_dbg
```

Cela signalera le problème `tokens[-1]` dans `analyseurlex()` comme une lecture hors limites.

---

## 📊 Exemple d'Entrée & Sortie Attendue

### Entrée : `input.txt`

```micro
BEGIN
WRITE (var1 ,(5+13.7), )#;
READ 'var1);
V1 = 5e-2 + (120 - 56
WRITE( (hello) , , , );
var :=  ;
READ (a,b, , );
write((1+2)-7,:=,9);
write(++,a+b);
read(a,b;
read(a,,,b)
write(:=,1+2);
var;
var:=6:=9;
END
```

C'est une entrée **délibérément riche en erreurs**, conçue pour exercer les deux chemins d'erreur lexicale et syntaxique :

- `13.7` et `5e-2` — littéraux flottants.
- `#` et `'` — caractères illégaux.
- `=` — invalide (seul `:=` est valide).
- `(120 - 56` — parenthèse fermante manquante.
- `var := ;` — expression manquante.
- `read(a,b;` — délimiteurs non appariés.
- `write(:=,1+2);` — `:=` utilisé comme expression.
- `var:=6:=9;` — deux affectations dans une seule instruction.

### Sortie de la Passe 1 : `sortie_analyseur.txt`

```
BEGIN 
WRITE LPAREN ID COMMA LPAREN INTLITERAL PLUSOP FLOATLITERAL RPAREN COMMA RPAREN SEMICOLON 
READ ID RPAREN SEMICOLON 
ID FLOATLITERAL PLUSOP LPAREN INTLITERAL MINUSOP INTLITERAL 
WRITE LPAREN LPAREN ID RPAREN COMMA COMMA COMMA RPAREN SEMICOLON 
ID ASSIGNOP SEMICOLON 
READ LPAREN ID COMMA ID COMMA COMMA RPAREN SEMICOLON 
WRITE LPAREN LPAREN INTLITERAL PLUSOP INTLITERAL RPAREN MINUSOP INTLITERAL COMMA ASSIGNOP COMMA INTLITERAL RPAREN SEMICOLON 
WRITE LPAREN PLUSOP PLUSOP COMMA ID PLUSOP ID RPAREN SEMICOLON 
READ LPAREN ID COMMA ID SEMICOLON 
READ LPAREN ID COMMA COMMA COMMA ID RPAREN 
WRITE LPAREN ASSIGNOP COMMA INTLITERAL PLUSOP INTLITERAL RPAREN SEMICOLON 
ID SEMICOLON 
ID ASSIGNOP INTLITERAL ASSIGNOP INTLITERAL SEMICOLON 
SCANEOF
```

Observations :

| Fragment d'Entrée | Tokenisation | Commentaire |
|-------------------|--------------|-------------|
| `BEGIN` | `BEGIN` | mot-clé reconnu |
| `13.7` | `FLOATLITERAL` | flottant reconnu |
| `5e-2` | `FLOATLITERAL` | flottant avec exposant reconnu |
| `#` | *(sauté, erreur lexicale sur console)* | caractère illégal |
| `'` | *(sauté, erreur lexicale sur console)* | caractère illégal |
| `=` | *(sauté, erreur lexicale sur console)* | `=` seul invalide |
| commentaires `--` | *(sautés)* | non présents dans cette entrée, mais supportés |

> **Important :** Les erreurs lexicales sont imprimées sur **stdout**, pas dans `sortie_analyseur.txt`. Le fichier de tokens ne contient que des tokens valides.

### Sortie de la Passe 2 : erreurs de syntaxe sur stdout

Parce que l'entrée est malformée, le parseur signale des erreurs. Exemples :

```
Excpected RPAREN ligne 2
Expression manquante ou erreur de syntaxe ligne 2
Found unexpected SCANEOF
Excpected SEMICOLON ligne 4
...
```

Les messages exacts dépendent du chemin de récupération du parseur. Le point clé est que **le parseur se rétablit et continue**, produisant plusieurs diagnostics plutôt que de s'arrêter à la première erreur.

### Pourquoi les Sorties des Deux Passes Diffèrent

- **Passe 1** ne voit que le flux de caractères. Elle supprime silencieusement les caractères invalides (`#`, `'`, `=`).
- **Passe 2** voit le flux de tokens. Elle ne peut pas savoir que des caractères ont été supprimés, donc elle signale des erreurs *syntaxiques* basées sur les tokens présents.

C'est une leçon classique : **les erreurs lexicales se propagent en erreurs syntaxiques**. Les compilateurs réels essaient de supprimer les diagnostics en cascade en marquant les tokens après une erreur lexicale.

---

## 🛠️ Gestion et Récupération des Erreurs

### Erreurs Lexicales

Signalées par `lexical_error()` :

```c
void lexical_error(int sym) {
    printf("Erreur a la ligne %d caractere inconnu %c\n", ligneid, sym);
}
```

- Le caractère fautif est **sauté**.
- Le scan continue.
- L'erreur n'est **pas** écrite dans le fichier de tokens.

### Erreurs Syntaxiques

Signalées par `syntax_error()` :

```c
void syntax_error(token t) {
    int temp = (c == '\n') ? nb - 1 : nb;
    switch (t) {
        case -1: printf("Expression manquante ou erreur de syntaxe ligne %d\n", temp); break;
        default: printf("Excpected %s ligne %d\n", tokens[t], temp);
    }
}
```

Deux stratégies de récupération sont utilisées :

1. **Pas d'avance en cas de non-correspondance** (`match()`) : le token fautif reste ; le parseur peut réessayer avec une règle différente.
2. **Saut en mode panique** (`prim()`, `id_list()`) : saute des tokens jusqu'à un point de synchronisation.

### Comparaison avec les Compilateurs Réels

| Stratégie | Ce Projet | GCC / Clang | javac |
|-----------|-----------|-------------|-------|
| Erreur lexicale | Affiche + saute caractère | Affiche + saute caractère | Affiche + saute caractère |
| Erreur syntaxique | Mode panique + pas d'avance | Mode panique + ensembles de récupération | Mode panique + récupération |
| Limite d'erreurs | Aucune | Configurable | Configurable |
| Messages d'erreur | Numéro de ligne | Ligne + colonne + caret | Ligne + colonne + caret |
| Erreurs multiples | ✅ | ✅ | ✅ |
| Sévérité d'erreur | Niveau unique | Warning/Error/Fatal | Warning/Error/Fatal |
| Suppression en cascade | ❌ | ✅ | ✅ |

---

## ⚖️ Comparaison avec les Compilateurs Réels

| Aspect | Ce Projet | GCC / Clang / MSVC | javac |
|--------|-----------|-------------------|-------|
| **Passes** | Deux (basé fichier) | Une (entrelacé) | Une (entrelacé) |
| **Lexer** | Écrit à la main | Écrit à la main (GCC), piloté par table (Clang) | Écrit à la main (Unicode-aware) |
| **Parseur** | Descente récursive | LALR généré par Bison (GCC) ; écrit à la main (Clang) | Descente récursive |
| **Lookahead** | 1 token | 1–2 tokens, parfois arbitraire | Arbitraire via `Reader` |
| **Recherche de mots-clés** | `strcmp` linéaire | Hash parfait / trie | Table de hachage |
| **Gestion de tampon** | Fixe (`MAX=50`, `char[13]`) | Extensible, mappé en mémoire | Extensible |
| **Support des flottants** | ✅ `13.7`, `.5`, `5e-2` | ✅ Complet (hex, suffixes, NaN, Inf) | ✅ Complet (hex, suffixes, underscores) |
| **Unicode** | ❌ ASCII uniquement | ✅ UTF-8/UTF-16/UTF-32 | ✅ Unicode complet |
| **Récupération d'erreur** | Mode panique | Mode panique + ensembles de récupération | Mode panique + récupération |
| **Diagnostics** | Ligne uniquement | Ligne + colonne + fichier + caret | Ligne + colonne + fichier + caret |
| **AST** | ❌ (aucun arbre construit) | ✅ AST complet | ✅ AST complet |
| **Table des symboles** | ❌ | ✅ | ✅ |
| **Vérification de type** | ❌ | ✅ | ✅ |
| **IR / codegen** | ❌ | ✅ | ✅ (bytecode) |
| **Optimisation** | ❌ | ✅ (centaines de passes) | ✅ (JIT + AOT) |
| **Performance** | ~µs par token | ~ns par token (SIMD) | ~ns par token |
| **Tests** | Manuels | Suites massives, fuzzing | Suites massives, JCK |
| **Standard** | Ad-hoc | ISO C / ISO C++ | JLS |

### Comparaisons Approfondies

#### 1. Deux Passes vs. Une Passe

- **Ce projet** : deux passes avec un fichier de tokens entre les deux.
- **GCC** : une seule passe à travers le préprocesseur/lexer/parseur, tout en mémoire.
- **Clang** : une seule passe ; le parseur appelle `Lexer::Lex()` à la demande.
- **javac** : une seule passe ; le parseur tire les tokens du `Scanner`.

Les conceptions en deux passes sont pédagogiquement propres mais **plus lentes** (I/O supplémentaire) et **moins flexibles** (pas de retour du parseur vers le lexer). Les compilateurs réels entrelacent lexing et parsing pour ces raisons.

#### 2. Descente Récursive vs. LALR

- **Ce projet** utilise la descente récursive — chaque règle de grammaire devient une fonction.
- **GCC** utilise un **parseur LALR(1) généré par Bison** pour C/C++ (historiquement ; le parseur C++ de GCC est écrit à la main).
- **Clang** utilise la descente récursive écrite à la main.
- **javac** utilise la descente récursive.

Les compilateurs modernes préfèrent la descente récursive pour **de meilleurs messages d'erreur** et **une gestion plus facile des constructions sensibles au contexte**.

#### 3. Flux de Tokens Basé Fichier vs. Tokens en Mémoire

- **Ce projet** écrit les tokens dans `sortie_analyseur.txt` et les relit.
- **Les compilateurs réels** gardent les tokens en mémoire (souvent dans un allocateur d'arène).

L'approche basée fichier est plus lente mais rend le flux de tokens **inspectable** — utile pour l'enseignement et le débogage.

#### 4. Récupération d'Erreur

| Fonctionnalité | Ce Projet | GCC | javac |
|----------------|-----------|-----|-------|
| Mode panique | ✅ | ✅ | ✅ |
| Ensembles de synchronisation | Partiel | ✅ | ✅ |
| Suppression en cascade | ❌ | ✅ | ✅ |
| Limite d'erreurs | ❌ | ✅ | ✅ |
| Suivi de colonne | ❌ | ✅ | ✅ |
| Diagnostics avec caret | ❌ | ✅ | ✅ |
| Diagnostics machine-readable | ❌ | ✅ (`-fdiagnostics-format=json`) | ✅ (JSON) |

#### 5. Reconnaissance des Littéraux Flottants

| Forme | Ce Projet | GCC / Clang | javac |
|-------|:---------:|:-----------:|:-----:|
| `13.7` | ✅ | ✅ | ✅ |
| `.5` | ✅ | ✅ | ✅ |
| `5e-2` | ✅ | ✅ | ✅ |
| `0x1.8p3` | ❌ | ✅ (flottant hex) | ✅ (flottant hex) |
| `1_000_000` | ❌ | ❌ (C23 l'ajoute) | ✅ (underscores) |
| `1.0f` | ❌ | ✅ (suffixe) | ✅ (suffixe `f`/`d`) |
| `NaN`, `Inf` | ❌ | ✅ | ✅ |

---

## 📚 Contexte Académique & Théorie

### Phases du Compilateur

Un compilateur est traditionnellement divisé en :

1. **Analyse lexicale** — caractères → tokens.
2. **Analyse syntaxique** — tokens → arbre de parsing / AST.
3. **Analyse sémantique** — vérification de type, résolution de portée.
4. **Génération de code intermédiaire** — AST → IR.
5. **Optimisation** — IR → IR.
6. **Génération de code** — IR → code machine.

> Ce projet couvre les **phases 1 et 2**, réparties en deux passes.

### Langages Formels

- **Structure lexicale** = **langage régulier** (reconnaissable par un DFA).
- **Structure syntaxique** = **langage hors-contexte** (reconnaissable par un PDA / descente récursive).

Chaque classe de token est décrite par une **expression régulière** :

| Token | Expression Régulière |
|-------|---------------------|
| `BEGIN` | `[bB][eE][gG][iI][nN]` |
| `ID` | `[a-zA-Z][a-zA-Z0-9_]*` |
| `INTLITERAL` | `[0-9]+` |
| `FLOATLITERAL` | `[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?` \| `\.[0-9]+([eE][+-]?[0-9]+)?` |
| `ASSIGNOP` | `:=` |
| commentaire | `--[^\n]*` |

La grammaire est **LL(1)** — analysable avec un token de lookahead en descente récursive.

### La Règle du Plus Long Match

Le scanner suit la règle du **plus long match** (maximal munch) : il consomme autant de caractères que possible tout en correspondant à un token.

| Entrée | Tokenisation | Raison |
|--------|--------------|--------|
| `:=` | `ASSIGNOP` | Préféré à `:` puis `=` |
| `13.7` | `FLOATLITERAL` | Pas `INT(13)` `.` `INT(7)` |
| `5e-2` | `FLOATLITERAL` | Pas `INT(5)` `ID(e)` `MINUSOP` `INT(2)` |

### Récupération en Mode Panique

Du Dragon Book (Section 4.1.3) :

> **Récupération en mode panique** : en découvrant une erreur, le parseur rejette les tokens d'entrée jusqu'à ce que l'un d'un ensemble désigné de **tokens de synchronisation** soit trouvé.

> 🎯 `prim()` et `id_list()` dans `parser.c` implémentent ceci : ils sautent des tokens jusqu'à `)`, `,`, `read`, `end`, EOF, ou `;`.

### Références

1. Aho, A. V., Lam, M. S., Sethi, R., & Ullman, J. D. (2006). *Compilers: Principles, Techniques, and Tools* (2nd ed.). Pearson. — **Le Dragon Book**.
2. Louden, K. C. (1997). *Compiler Construction: Principles and Practice*. PWS Publishing. — Origine du langage Micro.
3. Levine, J. R. (2009). *Flex & Bison*. O'Reilly Media.
4. Parr, T. (2013). *The Definitive ANTLR 4 Reference*. Pragmatic Bookshelf.
5. Source GCC : `libcpp/lex.c`, `gcc/cp/parser.c`.
6. Source Clang : `clang/lib/Lex/Lexer.cpp`, `clang/lib/Parse/ParseExpr.cpp`.
7. Source OpenJDK : `src/jdk.compiler/share/classes/com/sun/tools/javac/parser/`.

---

## ⚠️ Limitations & Problèmes Connus

### 1. Lecture Hors Bornes `tokens[-1]`

Dans `analyseurlex()` :

```c
while (strcmp(tokens[i = scanner(myFile)], "SCANEOF")) {
    if (i >= 0) { ... } else { ... }
}
```

`scanner()` peut retourner `-1` (signal de saut de ligne), et `tokens[-1]` est un **comportement indéfini**. Sur la plupart des plateformes, cela lit accidentellement le dernier élément du tableau (à cause de la disposition mémoire), ce qui explique pourquoi le bug est silencieux. **Correction :** vérifier `i` avant l'indexation :

```c
while (1) {
    i = scanner(myFile);
    if (i == SCANEOF) break;
    if (i >= 0) fprintf(output, "%s ", tokens[i]);
    else        fprintf(output, "%c", '\n');
}
```

### 2. Fuite Mémoire dans `next_token()`

```c
char *token_name = malloc(20 * sizeof(char));
fscanf(output, "%s%c%c", token_name, &a, &c);
...
```

`token_name` n'est jamais libéré. **Correction :** utiliser un tampon sur la pile :

```c
char token_name[20];
fscanf(output, "%s%c%c", token_name, &a, &c);
```

### 3. `char tokens[15][13]` dans un En-tête

Déclarer (pas `extern`) le tableau dans `parser.h` crée une **définition tentative** dans chaque unité de traduction. Ce n'est pas strictement correct en C et peut causer des erreurs de linker. **Correction :**

```c
/* parser.h */
extern char tokens[15][13];

/* parser.c */
char tokens[15][13] = { ... };
```

### 4. Tampons de Taille Fixe

`buffer.token_tab[MAX]` avec `MAX = 50` peut déborder sur de longs identificateurs. `char tokens[15][13]` est correct pour les noms de tokens actuels mais fragile. **Correction :** tampons dynamiques, ou au moins des vérifications de bornes.

### 5. Pas de Suivi de Colonne

Seulement les numéros de ligne. Les compilateurs réels signalent `ligne:colonne`. **Correction :** ajouter le suivi de `column`.

### 6. `match()` n'Avance pas en Cas de Non-Correspondance

Cela évite de sauter des tokens valides, mais peut causer des **erreurs en cascade** car le même token déclenche plusieurs non-correspondances. **Correction :** utiliser des tokens de synchronisation et/ou marquer les tokens après une erreur.

### 7. Pas de Construction d'AST

Le parseur valide la syntaxe mais ne construit pas d'arbre. **Correction :** ajouter des types de nœuds AST et les construire dans `prim`, `expression`, `inst`.

### 8. Pas de Table des Symboles

Les identificateurs ne sont pas enregistrés. **Correction :** ajouter une table de hachage pour les identificateurs.

### 9. Pas de Vérification de Type

`var := (1+2)-7` est accepté syntaxiquement même si les types sont incorrects. **Correction :** ajouter un analyseur sémantique.

### 10. Chemins de Fichiers Codés en Dur

`input.txt` et `sortie_analyseur.txt` sont codés en dur. **Correction :** accepter des arguments CLI.

### 11. État Global

`main.c` et `parser.c` utilisent des globales. **Correction :** encapsuler dans `struct Scanner` / `struct Parser`.

### 12. `int add_op()` ne Retourne Rien d'Utile

`add_op()` est déclaré `int` mais ne retourne pas de valeur. **Correction :** le rendre `void`, ou retourner un code de statut.

### 13. Avertissements `-Wall`

Compiler avec `-Wall -Wextra` révélera plusieurs avertissements (variables inutilisées, déclarations implicites, retours manquants). À corriger pour un build propre.

---

## 🔬 Extensions Suggérées pour la Recherche

### 1. Construire un AST

Définir des types de nœuds :

```c
typedef struct Node {
    enum { NODE_ID, NODE_INT, NODE_FLOAT, NODE_BINOP, NODE_ASSIGN, NODE_READ, NODE_WRITE } kind;
    union {
        char *name;
        int ival;
        double fval;
        struct { struct Node *left, *right; char op; } binop;
        ...
    } u;
} Node;
```

Le construire dans `prim`, `expression`, `inst`. Puis écrire un pretty-printer.

### 2. Ajouter une Table des Symboles

Table de hachage associant les noms d'identificateurs aux types et portées. Détecter :
- Variables non déclarées.
- Variables redéclarées.
- Variables inutilisées.

### 3. Ajouter la Vérification de Type

Étendre Micro avec les types `int` et `float`. Implémenter un vérificateur de type qui :
- Promeut `int` en `float` dans les expressions mixtes.
- Rejette `float := int` si le rétrécissement est interdit.
- Signale les erreurs de type avec les numéros de ligne.

### 4. Générer du Code Intermédiaire

Émettre du code à trois adresses (TAC) :

```
t1 = 5 + 13.7
t2 = t1
var1 = t2
```

Puis écrire un interpréteur pour le TAC.

### 5. Fusionner les Deux Passes en Une

Refactoriser en une conception **en une passe** où le parseur appelle directement le scanner (comme dans `TP2.c` de l'autre dépôt). Comparer les performances.

### 6. Comparer avec les Compilateurs Réels

Profiler avec `perf` ou `valgrind --tool=callgrind`. Comparer tokens/sec avec GCC sur un grand fichier C.

### 7. Tests par Fuzzing

Utiliser **AFL** ou **libFuzzer** pour fuzzer le scanner/parseur. Trouver des crashes et des débordements de tampon.

### 8. Vérification Formelle

Modéliser le parseur en **Coq** ou **TLA+**. Prouver :
- Chaque entrée acceptée appartient au langage.
- Le parseur termine toujours.

### 9. Meilleure Récupération d'Erreur

Implémenter la **récupération au niveau phrase** et les **productions d'erreur**. Comparer avec les stratégies du Dragon Book.

### 10. Émettre des Diagnostics en JSON

Les IDE modernes (VS Code, Eclipse) consomment des diagnostics structurés. Émettre du JSON au lieu de texte brut.

### 11. Ajouter le Support Unicode

Remplacer `isalpha` / `isdigit` par des fonctions Unicode-aware. Tester avec des identificateurs comme `café`, `变量`.

### 12. Parsing Incrémental

Re-parser uniquement la région modifiée d'un fichier — utile pour les IDE.

---

## 🧪 Stratégie de Test

### Tests Unitaires

Créer des fichiers `.micro`, chacun testant une fonctionnalité :

| Fichier | Tests |
|---------|-------|
| `test_keywords.micro` | `begin`, `end`, `read`, `write` |
| `test_ids.micro` | Identificateurs avec `_` et chiffres |
| `test_ints.micro` | Entiers, zéros en tête |
| `test_floats.micro` | `13.7`, `.5`, `5e-2`, `1.5E+3` |
| `test_ops.micro` | `:=`, `+`, `-` |
| `test_punct.micro` | `(`, `)`, `;`, `,` |
| `test_comments.micro` | Commentaires `--` |
| `test_errors.micro` | Caractères illégaux |
| `test_empty.micro` | Fichier vide |
| `test_eof.micro` | Fichier sans nouvelle ligne finale |

### Tests de Référence (Golden)

Stocker les sorties attendues et comparer :

```bash
cp test_keywords.micro input.txt
./TP_TP_TP > actual.txt
diff actual.txt expected/test_keywords.out
```

### Tests avec Sanitizers

```bash
make debug
for f in tests/*.micro; do cp "$f" input.txt; ./TP_TP_TP_dbg > /dev/null; done
```

Le sanitizer signalera immédiatement le problème `tokens[-1]`.

### Fuzzing

```bash
afl-gcc -o TP_afl main.c parser.c
afl-fuzz -i seeds/ -o findings/ ./TP_afl
```

### Intégration Continue

Ajouter `.github/workflows/ci.yml` pour compiler et tester sur Linux, macOS et Windows.

---

## 📜 Licence & Remerciements

Ce projet est une implémentation éducative d'un front-end de compilateur en deux passes pour le langage **Micro**, inspiré par :

- Kenneth C. Louden, *Compiler Construction: Principles and Practice*.
- Aho, Lam, Sethi, Ullman, *Compilers: Principles, Techniques, and Tools*.

Il est destiné à un **usage académique**. N'hésitez pas à forker, étendre et utiliser dans des cours de construction de compilateurs.

### Citation Suggérée

```bibtex
@misc{micro_2pass_frontend,
  title  = {Front-End Compilateur Micro en Deux Passes},
  author = {Anonyme},
  year   = {2024},
  note   = {Lexer et parseur à descente récursive éducatifs en C},
  url    = {https://github.com/example/micro-2pass-frontend}
}
```

---

## 🗂️ Carte de Référence Rapide

### Commandes de Compilation

```bash
# Compiler avec Code::Blocks
#   Ouvrir TP.cbp → Build → Run

# Compiler avec GCC
gcc -Wall -Wextra -O2 -o TP_TP_TP main.c parser.c

# Build de débogage
gcc -Wall -Wextra -g -fsanitize=address,undefined -o TP_TP_TP_dbg main.c parser.c

# Exécuter (lit input.txt, écrit sortie_analyseur.txt)
./TP_TP_TP

# Inspecter le flux de tokens
cat sortie_analyseur.txt

# Windows
bin\Debug\TP_TP_TP.exe
type sortie_analyseur.txt
```

### Référence des Tokens

| Token | Nom d'Affichage | Exemple |
|-------|-----------------|---------|
| Mot-clé | `BEGIN`, `END`, `READ`, `WRITE` | `begin`, `END` |
| Identificateur | `ID` | `x`, `var1` |
| Entier | `INTLITERAL` | `52`, `0` |
| Flottant | `FLOATLITERAL` | `13.7`, `5e-2` |
| Affectation | `ASSIGNOP` | `:=` |
| Plus / Moins | `PLUSOP` / `MINUSOP` | `+`, `-` |
| Parenthèses | `LPAREN` / `RPAREN` | `(`, `)` |
| Point-virgule | `SEMICOLON` | `;` |
| Virgule | `COMMA` | `,` |
| EOF | `SCANEOF` | — |

### Pipeline en Un Coup d'Œil

```
input.txt ──► [PASSE 1 : LEXER] ──► sortie_analyseur.txt ──► [PASSE 2 : PARSER] ──► diagnostics
```

---

<div align="center">

**⭐ Star ce dépôt si tu le trouves utile ! ⭐**

*Fait avec ❤️ pour l'enseignement de la construction de compilateurs*

[Retour en Haut](#front-end-compilateur-micro-en-deux-passes)

</div>
