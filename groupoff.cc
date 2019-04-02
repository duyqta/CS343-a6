#include "soda.h"

void Groupoff::main() {
	printer.print(Printer::Kind::Groupoff, (char) States::Start);
	WATCard::FWATCard cards[numStudents];
	emptyCards = cards;
	for (unsigned int i = 0; i < numStudents; i += 1) _Accept(giftCard); //Note this does not accept destructor

	for (unsigned int i = 0; i < numStudents; i += 1) {
		yield(groupoffDelay); // should i yield before destructor accept?
		_Accept(~Groupoff) {
			printer.print(Printer::Kind::Groupoff, (char) States::Finished);
			break;
		} _Else {
			unsigned int chosenCard = mprng(cardCount - 1);
			WATCard * giftCard = new WATCard();
			giftCard->deposit(sodaCost);
			emptyCards[chosenCard].delivery(giftCard);
			printer.print(Printer::Kind::Groupoff, (char) States::Deposit, (int) sodaCost);

			for (unsigned int i = chosenCard + 1; i < numStudents; i += 1) {
				emptyCards[i - 1] = emptyCards[i];
			}
			emptyCards -= 1;
		}
	}
}

Groupoff::Groupoff( Printer & prt, unsigned int numStudents, 
	unsigned int sodaCost, unsigned int groupoffDelay ) 
	: printer(prt), numStudents(numStudents),  sodaCost(sodaCost), 
	groupoffDelay(groupoffDelay), cardCount(0) {

}


WATCard::FWATCard Groupoff::giftCard() {
	emptyCards[cardCount] = WATCard::FWATCard();
	cardCount += 1;
	return emptyCards[cardCount - 1];
}