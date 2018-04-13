#include "ClientPremium.h"
#include <iostream>

ClientPremium::ClientPremium(unsigned int joursRestants)
    : Client(),
      joursRestants_(joursRestants)
{
}

ClientPremium::ClientPremium(const string &nom, const string &prenom, int identifiant,
                             const string &codePostal,
                             unsigned int joursRestants)
    : Client(nom, prenom, identifiant, codePostal),
      joursRestants_(joursRestants)
{
}

unsigned int ClientPremium::obtenirJoursRestants() const
{
    return joursRestants_;
}

void ClientPremium::modifierJoursRestants(unsigned int joursRestants) {
    joursRestants_ = joursRestants;
}
