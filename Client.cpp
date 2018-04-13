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

Client::obtenirCodeClient() {
    return codeClient_;
}

Client::modifierCodeClient(unsigned int codeClient) {
    codeClient_ = codeClient;
}
