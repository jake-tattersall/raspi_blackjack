#include <stdio.h>
#include <locale.h>

#include "card.h"
#include "deck.h"

int main() {
    setlocale(LC_ALL, "");

    printf("Hello world!\n");
    printf("%d %d\n", MAXCARDS, VALUES_LEN);

    printf("1");
    struct Deck *d = createDeck();
    printf("2\n");

    printf("%d\n", d->len);
    for (int i = 0; i < d->len; i++) {
        struct Card* c = d->deck[i];
        printf("%s%s\n", c->suit, c->value);
    }

    freeDeck(d);

    return 0;
}