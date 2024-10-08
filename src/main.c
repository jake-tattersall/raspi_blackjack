#include <stdio.h>

#include "cards.h"
#include "deck.h"

int main() {

    printf("Hello world!\n");
    printf("%d %d\n", MAXCARDS, VALUES_LEN);

    struct Deck *d = createDeck();

    freeDeck(d);

    return 0;
}