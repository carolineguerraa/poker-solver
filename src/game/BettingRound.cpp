#include "BettingRound.hpp"
#include <iostream>
#include <limits>
#include <string>

void executeBettingRound(PokerGame &game)
{
    std::cout << "=== Betting Round ===" << std::endl;

    Agent *currentPlayer =
      game.playerIsDealer ? static_cast<Agent *>(game.player.get()) : static_cast<Agent *>(game.bot.get());
    Agent *opponent =
      game.playerIsDealer ? static_cast<Agent *>(game.bot.get()) : static_cast<Agent *>(game.player.get());

    bool bettingComplete = false;
    bool firstRound = true;

    while (!bettingComplete) {
        if (currentPlayer->isActive()) {
            if (currentPlayer == game.player.get()) {
                std::cout << currentPlayer->getName() << "'s turn. Current bet: " << game.currentBet
                          << ", your chips: " << currentPlayer->getChips() << std::endl;

                std::string action;
                std::cout << "Enter your action (fold, call, raise): ";
                std::cin >> action;

                if (action == "fold" || action == "f") {
                    currentPlayer->fold();
                    std::cout << currentPlayer->getName() << " folds." << std::endl;
                    return;
                } else if (action == "call" || action == "c") {
                    double amountToCall = game.currentBet - currentPlayer->getCurrentBet();
                    std::cout << amountToCall << std::endl;

                    if (amountToCall > currentPlayer->getChips()) { amountToCall = currentPlayer->getChips(); }
                    currentPlayer->deductChips(amountToCall);
                    game.pot += amountToCall;
                    currentPlayer->setCurrentBet(game.currentBet);
                    std::cout << currentPlayer->getName() << " calls with " << amountToCall << " chips." << std::endl;
                } else if (action == "raise" || action == "r") {
                    double raiseAmount = 0;
                    std::cout << "Enter raise amount: ";
                    std::cin >> raiseAmount;

                    while (std::cin.fail() || raiseAmount <= 0) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid raise amount. Please try again." << std::endl;
                        std::cout << "Enter raise amount: ";
                        std::cin >> raiseAmount;
                    }

                    double totalBet = game.currentBet + raiseAmount;
                    if (totalBet > currentPlayer->getChips()) { totalBet = currentPlayer->getChips(); }

                    double addedAmount = totalBet - currentPlayer->getCurrentBet();
                    currentPlayer->deductChips(addedAmount);
                    game.pot += addedAmount;
                    currentPlayer->setCurrentBet(totalBet);
                    game.currentBet = totalBet;

                    std::cout << currentPlayer->getName() << " raises to " << game.currentBet << " chips." << std::endl;
                } else {
                    std::cout << "Invalid action. Please try again." << std::endl;
                    continue;
                }
            } else {
                game.bot->makeMove(game.pot, game.currentBet);
            }
        }

        std::swap(currentPlayer, opponent);

        // Betting round ends when both players have matched bets, or a player folds
        if ((!opponent->isActive() || !currentPlayer->isActive())
            || (currentPlayer->getCurrentBet() == game.currentBet && opponent->getCurrentBet() == game.currentBet
                && !firstRound)) {
            bettingComplete = true;
        }
        firstRound = false;
    }

    // Reset current bets for the next round
    game.setCurrentBet(0);
    game.player->setCurrentBet(0);
    game.bot->setCurrentBet(0);
}
