#include "soda.h"

void Groupoff::main() {
	printer.print(Printer::Kind::Groupoff, (char) States::Start);
	WATCard::FWATCard cards[numStudents];
	emptyCards = cards;

	// first give all students a empty future
	for (unsigned int i = 0; i < numStudents; i += 1) _Accept(giftCard);

	for (unsigned int i = 0; i < numStudents; i += 1) {
		_Accept(~Groupoff) {
			break;
		} _Else {
			yield(groupoffDelay);
			unsigned int chosenCard = mprng(cardCount - 1);	// chose a random gift card to be filled
			WATCard * giftCard = new WATCard();
			giftCard->deposit(sodaCost);
			emptyCards[chosenCard].delivery(giftCard);		// put the filled card in the future
			printer.print(Printer::Kind::Groupoff, (char) States::Deposit, (int) sodaCost);

			// shift all cards in the array that are after the chosen card
			for (unsigned int i = chosenCard + 1; i < numStudents; i += 1) {
				emptyCards[i - 1] = emptyCards[i];
			}
			emptyCards -= 1;
		}
	}
	printer.print(Printer::Kind::Groupoff, (char) States::Finished);
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