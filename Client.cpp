#include "Client.h"
#include <iostream>

Client::Client()
    : Usager()
{
}

Client::Client(const string &nom, const string &prenom, int identifiant,
               const string &codePostal)
    : Usager(nom, prenom, identifiant, codePostal)
{
}

unsigned int Client::obtenirCodeClient() const {
    return codeClient_;
}

void Client::modifierCodeClient(unsigned int codeClient) {
    codeClient_ = codeClient;
}
