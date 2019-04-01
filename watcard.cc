#include "soda.h"

WATCard::WATCard(): balance( 0 ) {}

void WATCard::deposit( unsigned int amount ){
    balance += amount;
}

void withdraw( unsigned int amount ) {
    balance -= amount;
}

unsigned int getBalance() {
    return balance;
}