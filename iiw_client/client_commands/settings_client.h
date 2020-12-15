void settingsClient() {
    printf("\n \'windowSize <integer>\' \t\t imposta il valore della finestra di spedizione.\n \'lossRate <integer>\' \t\t\t imposta la probabilità di perdita pacchetto.\n \'timeout <integer>\' \t\t\t imposta la durata del timeout.\n \'adaptiveTimeout <integer>\' \t attiva il timeout adattivo (1) o disattivalo (0)\n");

    char buffer[MAXLINE] = {0};
    fgets(buffer, MAXLINE, stdin);

    // comando da eseguire
    char *variableName = strtok(buffer, " ");
    char *valueInCharacters = strtok(NULL, " ");

    int valueToSet = strtol(valueInCharacters, NULL, 10);

    if (errno = ERANGE && (valueToSet == LONG_MIN | valueToSet == LONG_MAX)) {
        perror("Si è verificato un errore nella gestione del valore indicato:");
        return;
    }

    if (strcmp(variableName, "windowSize") == 0) {
        if (valueToSet <= 0) {
            printf("Il valore del parametro windowSize deve essere maggiore di 0.\n");
        } else {
            windowSize = valueToSet;
        }

    } else if (strcmp(variableName, "lossRate") == 0) {
        if (valueToSet < 0 | valueToSet > 100) {
            printf("Il valore del parametro lossRate deve essere compreso tra 0 e 100.\n");
        } else {
            lossRate = valueToSet;
        }

    } else if (strcmp(variableName, "timeout") == 0) {
        if (valueToSet <= 0) {
            printf("Il valore del parametro timeout deve essere maggiore di 0.\n");
        } else {
            timeout = valueToSet;
        }


    } else if (strcmp(variableName, "adaptiveTimeout") == 0) {
        if (valueToSet != 0 && valueToSet != 1) {
            printf("Il valore del parametro adaptiveTimeout può essere solo 0 oppure 1.\n");
        } else {
            adaptiveTimeout = valueToSet;
        }


    } else {
        printf("Comando non valido.\n");
    }
}
