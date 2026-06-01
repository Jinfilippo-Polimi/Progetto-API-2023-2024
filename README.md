# Progetto-API-2023-2024 - Politecnico di Milano
Il progetto di Algoritmi e Strutture Dati è un progetto di programmazione assegnato agli studenti del corso di Algoritmi e Principi dell'Informatica (API) del Politecnico di Milano.  
Il corso di API si segue di norma durante il secondo semestre del secondo anno di Laurea Triennale di Ingengneria Informatica.  
Il progetto viene assegnato al termine di tale corso, a Maggio/Giugno, da svolgere durante l'estate e consegnare entro i primi giorni di Settembre.  
Ha un peso di 1 CFU.

# Descrizione del progetto A.A. 2023/2024
Il progetto richiede l'utilizzo esclusivo del linguaggio C11 (ISO/IEC 9899:2011). Inoltre è permesso solo l'utilizzo della "C Standard Library", che contiene le seguenti librerie:
* <assert.h>
* <ctype.h>
* <errno.h>
* <float.h>
* <iso646.h>
* <limits.h>
* <math.h>
* <stdef.h>
* <stdio.h>
* <stdlib.h>
* <string.h>
* <time.h>

Il progetto viene valutato considerando tre aspetti:
1. Il file di output prodotto dal programma deve essere corretto.
2. Il programma deve completare il calcolo entro un determinato tempo.
3. Il programma deve avere un uso di memoria di picco inferiore ad una certa soglia.
   
La prova di quest'anno consiste nel progettare un programma che simuli il funzionamento di una pasticceria: dopo aver fornito al programma un file (.txt) in ingresso tramite stdin, il programma esegue le istruzioni presenti nel file e salva il suo output in un altro file di testo.  
Al termine dell'elaborazione il file di output prodotto viene confrontato con un file di riferimento per controllare che i due siano uguali.  
La valutazione viene eseguita attraverso un sito web del Politecnico di Milano: dopo aver caricato il proprio codice sorgente sul sito, esso viene compilato ed eseguito su un server remoto.  

Di seguito sono elencati i rispettivi requisiti di tempo e memoria per ogni fascia di voto:

| VOTO     | TEMPO  | MEMORIA   | ESITO              |
| ---------|--------|-----------|--------------------|
| OPEN (*) | 45.0 s | 150.0 MiB | :white_check_mark: |
| 18       | 14.0 s | 35.0 MiB  | :white_check_mark: |
| 21       | 11.5 s | 30.0 MiB  | :white_check_mark: |
| 24       | 9.0 s  | 25.0 MiB  | :white_check_mark: |
| 27       | 6.5 s  | 20.0 MiB  | :white_check_mark: |
| 30       | 4.0 s  | 15.0 MiB  | :white_check_mark: |
| 30L      | 1.5 s  | 14.0 MiB  | :white_check_mark: |

(*) OPEN è un test con ampi margini di tempo e memoria, in modo da permettere agli studenti di avere un riferimento sul tempo impiegato e sulla memoria utilizzata dal loro programma, qualora non riuscissero a superare il test 18.

# Overview del mio progetto
Timeline del mio progetto, con allegato i vari programmi con cui ho superato per la prima volta ogni fascia di voto:
| DATA       | EVENTO             | TEMPO TARGET | TEMPO UTILIZZATO | MEMORIA TARGET | MEMORIA UTILIZZATA | CODICE SORGENTE        |
|------------|--------------------|--------------|------------------|----------------|--------------------|------------------------|
| 15/07/2024 | Inizio progetto    | N/A          | N/A              | N/A            | N/A                | N/A                    |
| 29/07/2024 | Superato test OPEN | 45.0 s       | 21.776 s         | 150.0 MiB      | 68.5 MiB           | non ancora disponibile |
| 30/07/2024 | Superato test 18   | 14.0 s       | 12.548 s         | 35.0 MiB       | 14.3 MiB           | non ancora disponibile |
| 30/072024  | Superato test 21   | 11.5 s       | 11.355 s         | 30.0 MiB       | 14.5 MiB           | non ancora disponibile |
| 01/08/2024 | Superato test 24   | 9.0 s        | 7.784 s          | 25.0 MiB       | 12.2 MiB           | non ancora disponibile |
| 04/08/2024 | Superato test 27   | 6.5 s        | 1.513 s          | 20.0 MiB       | 9.22 MiB           | non ancora disponibile |
| 04/08/2024 | Superato test 30   | 4.0 s        | 1.495 s          | 15.0 MiB       | 9.22 MiB           | non ancora disponibile |
| 04/08/2024 | Superato test 30L  | 1.5 s        | 1.497 s          | 14.0 MiB       | 9.22 MiB           | non ancora disponibile |


# Strumenti utilizzati
* MacOS 14 Sonoma
* Ubuntu 24
* Visual Studio Code
* Valgrind suite
* GDB (GNU debugger) su Ubuntu
* LLDB debugger su MacOS
