#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStyle>
#include <QKeySequence>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QSlider>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <typeinfo>
#include <string>

#include "mainwindow.h"
#include "Gestionnaire.h"
#include "Usager.h"
#include "Fournisseur.h"
#include "ClientPremium.h"
#include "ExceptionArgumentInvalide.h"

using namespace std;

Q_DECLARE_METATYPE(Usager*)

// Constructeur de la fenetre principale de l'application
MainWindow::MainWindow(Gestionnaire* gestionnaire, QWidget *parent) :
    QMainWindow(parent)
{
    gestionnaire_ = gestionnaire;
    setup();
}

MainWindow::~MainWindow() {
    while (!ajoute_.empty()) {
        delete ajoute_.back();
        ajoute_.pop_back();
    }
}

void MainWindow::setup() {
    indexCourantDuFiltre_ = 0;
    setMenu();
    setUI();
    setConnections();
    chargerUsagers();
}

/**
* Méthode permettant d'initialiser les connections entre les signaux d'ajout et de
* suppression d'un usager au gestionnaire
* @param {void}
* @return {void}
*/
void MainWindow::setConnections()
{
    //connecter le signal signal_usagerAjoute (Usager* u) du gestionnaire au slot usagerAEteAjoute(Usager* u)
    connect(gestionnaire_, SIGNAL(signal_usagerAjoute(Usager*)),
            this, SLOT(usagerAEteAjoute(Usager*)));
    //connecter le signal signal_usagerSupprime(Usager* u) du gestionnaire au slot usagerAEteSupprime(Usager* u)
    connect(gestionnaire_, SIGNAL(signal_usagerSupprime(Usager*)),
            this, SLOT(usagerAEteSupprime(Usager*)));
}

/**
* Méthode permettant d'initialiser le menu de l'application
* @param {void}
* @return {void}
*/
void MainWindow::setMenu() {

    // On crée un bouton 'Exit'
    QAction* exit = new QAction(tr("&Quitter"), this);

    // On ajoute un raccourci clavier qui simulera l'appui sur ce bouton (Ctrl+Q)
    exit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    // On connecte le clic sur ce bouton avec l'action de clore le programme
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    // On crée un nouveau menu 'File'
    QMenu* fileMenu = menuBar()->addMenu(tr("&Fichier"));

    // Dans lequel on ajoute notre bouton 'Exit'
    fileMenu->addAction(exit);
}

/**
 * Méthode permettant d'initialiser l'interface de l'application
 * @param {void}
 * @return {void}
*/
void MainWindow::setUI() {

    // Le sélecteur pour filtrer ce que l'on souhaite dans la liste (QComboBox*)
    // liste de filtres
    QStringList listeFiltres = (QStringList()
                                << "Tout Afficher"
                                << "Afficher Clients Reguliers"
                                << "Afficher Clients Premiums"
                                << "Afficher Fournisseurs");
    QComboBox* showCombobox = new QComboBox(this);
    showCombobox->addItems(listeFiltres);

    // connection de la combobox
    connect(showCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtrerListe(int)));

    // La liste des usagers
    listUsager = new QListWidget(this);
    listUsager->setSortingEnabled(true);
    connect(listUsager, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(selectionnerUsager(QListWidgetItem*)));

    // Le bouton pour supprimer tout le monde
    QPushButton* boutonSupprimerTousLesUsagers = new QPushButton(this);
    boutonSupprimerTousLesUsagers->setText("Supprimer Tous Les Usagers");
    connect(boutonSupprimerTousLesUsagers, SIGNAL(clicked()), this, SLOT(supprimerTousLesUsagers()));

    // Le bouton pour remettre à zéro la vue et ajouter un nouvel employé
    QPushButton* boutonAjouterUsager = new QPushButton(this);
    boutonAjouterUsager->setText("Ajouter Usager");
    connect(boutonAjouterUsager, SIGNAL(clicked(bool)), this, SLOT(nettoyerVue()));

    // Le premier layout, pour la colonne de gauche, dans lequel on insère les
    // éléments que l'on veut dans l'ordre dans lequel on veut qu'ils apparaissent
    QVBoxLayout* listLayout = new QVBoxLayout;
    listLayout->addWidget(showCombobox);
    listLayout->addWidget(listUsager);
    listLayout->addWidget(boutonSupprimerTousLesUsagers);
    listLayout->addWidget(boutonAjouterUsager);

    // Champ pour le nom
    QLabel* nomLabel = new QLabel;
    nomLabel->setText("Nom:");
    editeurNom= new QLineEdit;

    QHBoxLayout* nomLayout = new QHBoxLayout;
    nomLayout->addWidget(nomLabel);
    nomLayout->addWidget(editeurNom);

    // Champ pour le prenom
    QLabel* prenomLabel = new QLabel;
    prenomLabel->setText("Prenom:");
    editeurPrenom = new QLineEdit;

    QHBoxLayout* prenomLayout = new QHBoxLayout;
    prenomLayout->addWidget(prenomLabel);
    prenomLayout->addWidget(editeurPrenom);

    //Champ pour l'identifiant avec validateur int entre 0 et 100 000
    QLabel* identifiantLabel = new QLabel;
    identifiantLabel->setText("Identifiant:");
    editeurIdentifiant = new QLineEdit;
    editeurIdentifiant->setValidator(new QIntValidator(0, 100000, Q_NULLPTR));

    QHBoxLayout* identifiantLayout = new QHBoxLayout;
    identifiantLayout->addWidget(identifiantLabel);
    identifiantLayout->addWidget(editeurIdentifiant);

    // Champ pour le code postal
    QLabel* codePostalLabel = new QLabel;
    codePostalLabel->setText("Code postal:");
    editeurCodePostal = new QLineEdit;

    QHBoxLayout* codePostalLayout = new QHBoxLayout;
    codePostalLayout->addWidget(codePostalLabel);
    codePostalLayout->addWidget(editeurCodePostal);

    //Champ pour JoursRestant de ClientPremium avec validateur int entre 0 et 1000
    QLabel* joursRestantsLabel = new QLabel;
    joursRestantsLabel->setText("Jours restants:");
    editeurJoursRestants = new QLineEdit;
    editeurJoursRestants->setValidator(new QIntValidator(0, 1000, Q_NULLPTR));

    QHBoxLayout* joursRestantsLayout = new QHBoxLayout;
    joursRestantsLayout->addWidget(joursRestantsLabel);
    joursRestantsLayout->addWidget(editeurJoursRestants);

    // Boutons radio
    QRadioButton* clientPremiumBoutonRadio = new QRadioButton("&ClientPremium", this);
    clientPremiumBoutonRadio->setChecked(true);
    boutonRadioTypeUsager.push_back(clientPremiumBoutonRadio);

    QRadioButton* clientRegulierBoutonRadio = new QRadioButton("&Client", this);
    boutonRadioTypeUsager.push_back(clientRegulierBoutonRadio);

    QRadioButton* fournisseurBoutonRadio = new QRadioButton("&Fournisseur", this);
    boutonRadioTypeUsager.push_back(fournisseurBoutonRadio);

    QButtonGroup* typeUsagerGroupeBoutons = new QButtonGroup;
    typeUsagerGroupeBoutons->addButton(clientPremiumBoutonRadio);
    typeUsagerGroupeBoutons->addButton(clientRegulierBoutonRadio);
    typeUsagerGroupeBoutons->addButton(fournisseurBoutonRadio);
    connect(typeUsagerGroupeBoutons, SIGNAL(buttonClicked(int)),
            this, SLOT(changerTypeUsager(int)));

    QHBoxLayout* typeUsagerLayout = new QHBoxLayout;
    typeUsagerLayout->addWidget(clientPremiumBoutonRadio);
    typeUsagerLayout->addWidget(clientRegulierBoutonRadio);
    typeUsagerLayout->addWidget(fournisseurBoutonRadio);

    // Trait horizontal de séparation
    QFrame* horizontalFrameLine = new QFrame;
    horizontalFrameLine->setFrameShape(QFrame::HLine);

    // Bouton pour supprimer l'usager sélectionné dans la liste
    // TODO verifier
    boutonSupprimer = new QPushButton(this);
    boutonSupprimer->setEnabled(false);
    boutonSupprimer->setText("Supprimer");
    connect(boutonSupprimer, SIGNAL(clicked()),
            this, SLOT(supprimerUsagerSelectionne()));

    // Bouton pour ajouter l'usager dont on
    // vient d'entrer les informations
    boutonAjouter = new QPushButton(this);
    boutonAjouter->setText("Ajouter");
    connect(boutonAjouter, SIGNAL(clicked()),
            this, SLOT(ajouterUsager()));

    // Organisation horizontale des boutons
    QHBoxLayout* ajouterSupprimerLayout = new QHBoxLayout;
    ajouterSupprimerLayout->addWidget(boutonAjouter);
    ajouterSupprimerLayout->addWidget(boutonSupprimer);

    // Organisation pour la colonne de droite
    QVBoxLayout* displayLayout = new QVBoxLayout;
    displayLayout->addLayout(nomLayout);
    displayLayout->addLayout(prenomLayout);
    displayLayout->addLayout(identifiantLayout);
    displayLayout->addLayout(codePostalLayout);
    displayLayout->addLayout(joursRestantsLayout);
    displayLayout->addLayout(typeUsagerLayout);
    displayLayout->addWidget(horizontalFrameLine);
    displayLayout->addLayout(ajouterSupprimerLayout);

    // Trait vertical de séparation
    QFrame* verticalFrameLine = new QFrame;
    verticalFrameLine->setFrameShape(QFrame::VLine);

    // Organisation horizontale
    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addLayout(listLayout);
    mainLayout->addWidget(verticalFrameLine);
    mainLayout->addLayout(displayLayout);

    // On crée un nouveau Widget, et on définit son
    // layout pour celui que l'on vient de créer
    QWidget* widget = new QWidget;
    widget->setLayout(mainLayout);

    // Puis on définit ce widget comme le widget
    // centrale de notre classe
    setCentralWidget(widget);

    // Enfin, on met à jour le titre de la fenêtre
    string title = "Gestionnaire de polebay!";
    setWindowTitle(title.c_str());
}

/**
 * Cette fonction crée une boite de message de type popup
 * @param {QString} msg message à afficher
 * @return {void}
*/
void MainWindow::afficherMessage(QString msg) {
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}

/**
 * cette fonction permet de charger tous les usagers connus
 * dans la liste des usagers
 * @param {void}
 * @return {void}
 */
void MainWindow::chargerUsagers() {
    // On s'assure que la liste est vide
    listUsager->clear();
    // Puis, pour tous les usagers
    int max = gestionnaire_->obtenirNombreUsager();
    for (int i = 0; i < max; i++) {
        // On récupère le pointeur vers l'employé
        Usager* usager = gestionnaire_->obtenirUsager(i);
        if (usager == nullptr) {
            continue;
        }
        // Et on l'ajoute en tant qu'item de la liste:
        // le nom et prenom sera affiché, et le pointeur sera contenu
        QListWidgetItem* item = new QListWidgetItem(
            QString::fromStdString(usager->obtenirNom()) + ", " + QString::fromStdString(usager->obtenirPrenom()), listUsager);
        item->setData(Qt::UserRole, QVariant::fromValue<Usager*>(usager));
        item->setHidden(filtrerMasque(usager));
    }
}

/**
 * Fonction qui retourne un boolean sur le type de filtre choisi
 * @param {Usager*} usager usager dont il faut filtrer le masque
 * @return {bool} si le masque de l'usager correspond à un type dérivé
*/
bool MainWindow::filtrerMasque(Usager* usager) {
    switch (indexCourantDuFiltre_) {
    case 1:
        return (typeid(*usager) != typeid(Client));
    case 2:
        return (typeid(*usager) != typeid(ClientPremium));
    case 3:
        return (typeid(*usager) != typeid(Fournisseur));
    case 0:
    default:
        return false;
    }
}

/**
 * Fonction qui affiche les usagers d'un certain type selon l'index donné en paramètre
 * Il s'agit des champs du dropdown menu ( Tous les usagers = 0 , Tous les clients reguliers = 1, tous les fournisseurs = 2...)
 * @param {int} index index de la liste
 * @return {void}
*/
void MainWindow::filtrerListe(int index) {
    // on s'assure que la liste soit vide
    listUsager->clear();
    switch (index) {
    case 0:
        for (int i = 0; i < gestionnaire_->obtenirNombreUsager(); ++i) {
            // On récupère le pointeur vers l'employé
            Usager* usager = gestionnaire_->obtenirUsager(i);
            if (usager == nullptr) {
                continue;
            }
            // Et on l'ajoute en tant qu'item de la liste:
            // le nom et prenom sera affiché, et le pointeur sera contenu
            QListWidgetItem* item = new QListWidgetItem(
                QString::fromStdString(usager->obtenirNom()) + ", " + QString::fromStdString(usager->obtenirPrenom()), listUsager);
            item->setData(Qt::UserRole, QVariant::fromValue<Usager*>(usager));
            item->setHidden(filtrerMasque(usager));
        }
        break;
    case 1: // clients reg
        for (int i = 0; i < gestionnaire_->obtenirNombreUsager(); ++i) {
            // on recupere le pointeur vers le client
            Usager* usager = gestionnaire_->obtenirUsager(i);
            if (dynamic_cast<Client*>(usager) == nullptr) {
                continue;
            }
            QListWidgetItem* item = new QListWidgetItem(
                        QString::fromStdString(usager->obtenirNom()) + ", " +
                        QString::fromStdString(usager->obtenirPrenom()),
                        listUsager);
            item->setData(Qt::UserRole, QVariant::fromValue<Usager*>(usager));
            item->setHidden(filtrerMasque(usager));
        }
        break;
    case 2: // clients premiums
        for (int i = 0; i < gestionnaire_->obtenirNombreUsager(); ++i) {
            // on recupere le pointeur vers le client
            Usager* usager = gestionnaire_->obtenirUsager(i);
            if (dynamic_cast<ClientPremium*>(usager) == nullptr) {
                continue;
            }
            QListWidgetItem* item = new QListWidgetItem(
                        QString::fromStdString(usager->obtenirNom()) + ", " +
                        QString::fromStdString(usager->obtenirPrenom()),
                        listUsager);
            item->setData(Qt::UserRole, QVariant::fromValue<Usager*>(usager));
            item->setHidden(filtrerMasque(usager));
        }
        break;
    case 3: // fournisseurs
        for (int i = 0; i < gestionnaire_->obtenirNombreUsager(); ++i) {
            // on recupere le pointeur vers le client
            Usager* usager = gestionnaire_->obtenirUsager(i);
            if (dynamic_cast<Fournisseur*>(usager) == nullptr) {
                continue;
            }
            QListWidgetItem* item = new QListWidgetItem(
                        QString::fromStdString(usager->obtenirNom()) + ", " +
                        QString::fromStdString(usager->obtenirPrenom()),
                        listUsager);
            item->setData(Qt::UserRole, QVariant::fromValue<Usager*>(usager));
            item->setHidden(filtrerMasque(usager));
        }
        break;
    default:
        return;
    }
}

/**
 * Fonction qui affiche les informations de l'usager sélectionné à partir de la liste.
 * Ses informations sont affichées dans les boîtes de texte du panneau de droite.
 * Les champs sont disabled à l'utilisateur pour éviter qu'il ne modifie l'objet
 * @param {QListWidgetItem*} item item de la liste
 * @return {void}
*/
void MainWindow::selectionnerUsager(QListWidgetItem* item) {
    Usager* usager = item->data(Qt::UserRole).value<Usager*>();
    boutonSupprimer->setEnabled(true);
    //Tous les champs sont mis à disabled et affiche l'information de l'usager sélectionné
    /*À Faire*/
    editeurNom->setDisabled(true);
    editeurNom->setText(QString::fromStdString(usager->obtenirNom()));
    editeurPrenom->setDisabled(true);
    editeurPrenom->setText(QString::fromStdString(usager->obtenirPrenom()));
    editeurIdentifiant->setDisabled(true);
    editeurIdentifiant->setText(QString::fromStdString(to_string(usager->obtenirIdentifiant())));
    editeurCodePostal->setDisabled(true);
    editeurCodePostal->setText(QString::fromStdString(usager->obtenirCodePostal()));
    editeurJoursRestants->setDisabled(true);

    //Affiche les jours restants s'il s'agit d'un ClientPremium, sinon on affiche "-"
    if (typeid(*usager) == typeid(ClientPremium)) {
        ClientPremium* clientPrem = dynamic_cast<ClientPremium*>(usager);
        editeurJoursRestants->setText(QString::fromStdString(to_string(clientPrem->obtenirJoursRestants())));
    }
    else {
        editeurJoursRestants->setText("-");
    }

    //On met a checked le type d'usager qui est sélectionné
    list<QRadioButton*>::iterator end = boutonRadioTypeUsager.end();
    for (list<QRadioButton*>::iterator it = boutonRadioTypeUsager.begin(); it != end; it++) {
        (*it)->setDisabled(true);

        bool checked = false;

        if ((typeid(*usager) == typeid(Client) && (*it)->text().endsWith("Client"))
             || (typeid(*usager) == typeid(ClientPremium) && (*it)->text().endsWith("ClientPremium"))
             || (typeid(*usager) == typeid(Fournisseur) && (*it)->text().endsWith("Fournisseur"))) {
                checked = true;
        }
        (*it)->setChecked(checked);
    }

    boutonAjouter->setDisabled(true);
    boutonSupprimer->setDisabled(false);
}

/**
 * Fonction permettant de remettre à neuf la vue tel qu'on puisse y ajouter
 * un nouvel usager
 * @param {void}
 * @return {void}
*/
void MainWindow::nettoyerVue() {
    editeurNom->setDisabled(false);
    editeurNom->setText("");

    editeurPrenom->setDisabled(false);
    editeurPrenom->setText("");

    editeurIdentifiant->setDisabled(false);
    editeurIdentifiant->setText("");

    editeurCodePostal->setDisabled(false);
    editeurCodePostal->setText("");

    editeurJoursRestants->setDisabled(false);
    editeurJoursRestants->setText("");

    //Par défaut le boutons radios est à ClientPremium
    list<QRadioButton*>::iterator end = boutonRadioTypeUsager.end();
    for (list<QRadioButton*>::iterator it = boutonRadioTypeUsager.begin(); it != end; it++) {
        (*it)->setDisabled(false);
        if ((*it)->text().endsWith("ClientPremium")) {
            (*it)->setChecked(true);
        }
    }

    listUsager->clearSelection();
    boutonAjouter->setDisabled(false);
    boutonSupprimer->setDisabled(true);
    editeurNom->setFocus();
}

/**
 * M/thode permettant de déterminer si le champ jours restants
 * doit être actif ou non
 * Le champ JoursRestants est activé seulement s'il s'agit d'un ClientPremium
 * @param {int} index index du type de l'usager
 * @return {void}
*/
void MainWindow::changerTypeUsager(int index) {
    if (index == -2) {
        editeurJoursRestants->setDisabled(false);
    } else {
        editeurJoursRestants->setDisabled(true);
    }
}

/**
 * Supprimer tous les usagers de la liste
 * @param {void}
 * @return {void}
*/
void MainWindow::supprimerTousLesUsagers() {
    vector<Usager*> toDelete;
    for (Usager* u : gestionnaire_->obtenirUsagers()) {
        gestionnaire_->supprimerUsager(u);
    }
    listUsager->clear();
    nettoyerVue();
}

/**
 * Supprime l'usager sélectionné dans la liste
 * @param {void}
 * @return {void}
*/
void MainWindow::supprimerUsagerSelectionne() {
    if(!(editeurNom->text().length() == 0 &&
         editeurPrenom->text().length() == 0 &&
         editeurIdentifiant->text().length() == 0 &&
         editeurCodePostal->text().length() == 0 &&
         editeurJoursRestants->text().length() == 0))
    {
        // Trouver l'information de l'usager selectionne
        int identifiantRecherche = editeurIdentifiant->text().toInt();

        // Rechercher l'usager
        for (int i = 0; i < gestionnaire_->obtenirNombreUsager(); ++i) {
            if (gestionnaire_->obtenirUsager(i)->obtenirIdentifiant() == identifiantRecherche) {
                // supprimer l'usager
                usagerAEteSupprime(gestionnaire_->obtenirUsager(i));
                gestionnaire_->supprimerUsager(gestionnaire_->obtenirUsager(i));
                break;
            }
        }
    }
}

/**
 * Ajoute un nouvel usager dans la liste
 * @param {void}
 * @return {void}
*/
void MainWindow::ajouterUsager() {

    Usager* nouvelUsager;

    try {
        // Trouver le bouton selectionne
        QAbstractButton* boutonUsagerSelectionne;
        foreach (QAbstractButton *button, boutonRadioTypeUsager) {
            if (button->isChecked()) {
                boutonUsagerSelectionne = button;
                break;
            }
        }
        if (editeurNom->text().length() == 0)
        {
            throw ExceptionArgumentInvalide("Erreur: Le champs Nom est invalide. ");
        }
        if (editeurPrenom->text().length() == 0)
        {
            throw ExceptionArgumentInvalide("Erreur: Le champs Prenom est invalide. ");
        }
        if (editeurIdentifiant->text().length() == 0)
        {
            throw ExceptionArgumentInvalide("Erreur: Le champs Identifiant est invalide. ");
        }
        if (editeurCodePostal->text().length() == 0)
        {
            throw ExceptionArgumentInvalide("Erreur: Le champs Code Postal est invalide. ");
        }
        // On trouve le bon type d'usager selon le bouton radio sélectionné
        if (boutonUsagerSelectionne->text() == "&ClientPremium")
        {
            // On créé le bon type d'usager selon le cas
            nouvelUsager = new ClientPremium(editeurNom->text().toLocal8Bit().constData(),
                                             editeurPrenom->text().toLocal8Bit().constData(),
                                             editeurIdentifiant->text().toInt(),
                                             editeurCodePostal->text().toLocal8Bit().constData(),
                                             editeurJoursRestants->text().toInt());
            // Vérification que tous les champs ont été complétés
            // On ajoute le nouvel usager créé au gestionnaire
            gestionnaire_->ajouterUsager(nouvelUsager);
            // Mais on le stocke aussi localement dans l'attribut ajoute_ pour pouvoir
            //     le supprimer plus tard
            ajoute_.push_back(nouvelUsager);
        }
        else if (boutonUsagerSelectionne->text() == "&Client") {
            // On créé le bon type d'usager selon le cas
            nouvelUsager = new Client(editeurNom->text().toLocal8Bit().constData(),
                                      editeurPrenom->text().toLocal8Bit().constData(),
                                      editeurIdentifiant->text().toInt(),
                                      editeurCodePostal->text().toLocal8Bit().constData());
            // On ajoute le nouvel usager créé au gestionnaire
            gestionnaire_->ajouterUsager(nouvelUsager);
            // Mais on le stocke aussi localement dans l'attribut ajoute_ pour pouvoir
            //     le supprimer plus tard
            ajoute_.push_back(nouvelUsager);
        }
        else { // Fournisseur
            // On créé le bon type d'usager selon le cas
            nouvelUsager = new Fournisseur(editeurNom->text().toLocal8Bit().constData(),
                                           editeurPrenom->text().toLocal8Bit().constData(),
                                           editeurIdentifiant->text().toInt(),
                                           editeurCodePostal->text().toLocal8Bit().constData());
            // On ajoute le nouvel usager créé au gestionnaire
            gestionnaire_->ajouterUsager(nouvelUsager);
            // Mais on le stocke aussi localement dans l'attribut ajoute_ pour pouvoir
            //     le supprimer plus tard
            ajoute_.push_back(nouvelUsager);
        }
    }
    catch (ExceptionArgumentInvalide& eai) {
        afficherMessage(eai.what());
    }
    chargerUsagers();
}

/**
 * Mise à jour de la vue après l'ajout d'un usager
 * @param {Usager*} u usager qui a été ajouté
 * @return {void}
*/
void MainWindow::usagerAEteAjoute(Usager* u) {
    /*À Faire*/
    QListWidgetItem* item = new QListWidgetItem(
        QString::fromStdString(u->obtenirNom()) + ", " + QString::fromStdString(u->obtenirPrenom()), listUsager);
    item->setData(Qt::UserRole, QVariant::fromValue<Usager*>(u));
    item->setHidden(filtrerMasque(u));

    nettoyerVue();
}

/**
 * Mise à jour de la vue après la suppression d'un usager
 * @param {Usager*} u usager qui a été supprimé
 * @return {void}
*/
void MainWindow::usagerAEteSupprime(Usager* u) {
    // On cherche dans notre QlistWidget l'usager pour lequel le
    // signal a été envoyé, afin de l'en retirer
    for (int i = 0; i < listUsager->count(); ++i) {
        QListWidgetItem *item = listUsager->item(i);
        Usager* usager = item->data(Qt::UserRole).value<Usager*>();
        if (usager == u) {
            // delete sur un QlistWidget item va automatiquement le retirer de la liste

            delete item;
            // Si l'usager faisait partie de ceux créés localement, on veut le supprimer.
            auto it = std::find(ajoute_.begin(), ajoute_.end(), u);
            if (it != ajoute_.end()) {
                delete *it;
                ajoute_.erase(it);
            }
            break;
        }
    }
    // On remet à zéro l'affichage du panneau de gauche étant
    // donné que l'usager sélectionné a été supprimé
    nettoyerVue();
}
