#include "soda.h"
#include <vector>

void Groupoff::main() {
	printer.print(Printer::Kind::Groupoff, (char) States::Start);
	//WATCard::FWATCard emptyCards[numStudents];

	// first give all students a empty future
	for (unsigned int i = 0; i < numStudents; i += 1) {

		//emptyCards[cardCount] = WATCard::FWATCard();
		if (cardCond.empty()) _Accept(giftCard);
		cardCount += 1;
		//newCard = emptyCards[cardCount];
		emptyCards.push_back(WATCard::FWATCard());
		cardCond.signalBlock();
	}

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
			cardCount -= 1;
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
	cardCond.wait();
	
	return emptyCards[cardCount - 1];
}