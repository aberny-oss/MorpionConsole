#include <iostream>
#include <array>
#include <conio.h>
#include <Windows.h>

using Board = std::array<char, 9>;
unsigned int stock;
int parties = 0;

char GetQuiJoue()
{
    if (stock & 0x1)
    {
        return 'X';
    }
    return 'O';
} 

void SetQuiJoue() // changer de joueur a chaque tour
{
    // Crée un masque où tous les bits valent 1 sauf le bit de poids faible (bit 0) : 11111110 (sur 8 bits).
    unsigned int mask = ~1; // interchange les 0 en 1 et les 1 en 0 du bit choisie (~1 <=> 0xFFFFFFFE) sauf le dernier
    // (stock & mask) garde les ancienne valeur car si c'était 1 1et 1 = 1 et si c'était 0 0et 1 = 0
    // (~stock & 0x01) iverse tout et garde que le plus a droite car 0x01 et ajoute a stock
    stock = (stock & mask) | (~stock & 0x1);
}

BYTE GetEtatPartie()
{
    // Lit uniquement les bits 1 et 2, puis les décale à droite de 1 pour obtenir la valeur d'état (0 à 3)
    return (stock & 0x6) >> 1;
}

void SetEtatPartie(BYTE etatPartie)
{
    // On veut uniquement modifier les bits 2 et 3 :
    // 1. Efface les bits 2 et 3
    // 3avec un & ~0x6 (car 0x6 = 0b00000110)
    // 2. Ajoute la nouvelle valeur, décalée de 1 cran à gauche
    stock = (stock & ~0x6) | ((etatPartie & 0x3) << 1);
}


void afficherPlateau(const Board& b)
{
    system("cls");

    std::cout << "Morpion (Tic-Tac-Toe) en C++\n";
    std::cout << "Disposition des cases :\n";
    std::cout << " 7 | 8 | 9\n";
    std::cout << "---+---+---\n";
    std::cout << " 4 | 5 | 6\n";
    std::cout << "---+---+---\n";
    std::cout << " 1 | 2 | 3\n\n";

    std::cout << "\n";
    std::cout << "Partie : " << (parties + 1) << "\n";
    std::cout << "\n";
    std::cout << " " << b[6] << " | " << b[7] << " | " << b[8] << "\n";
    std::cout << "---+---+---\n";
    std::cout << " " << b[3] << " | " << b[4] << " | " << b[5] << "\n";
    std::cout << "---+---+---\n";
    std::cout << " " << b[0] << " | " << b[1] << " | " << b[2] << "\n";
    std::cout << "\n";
}

bool caseLibre(const Board& b, int index)
{
    return b[index] == ' ';
}

bool coupGagnant(const Board& b, char joueur) // verifie si la partie est gagnée par le joueur actuel et regarder dans stock le joueur qui ne joue pas (c'est celui qui a perdu)
{
    const int lignesGagnantes[8][3] = {
        {0, 1, 2}, // lignes
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6}, // colonnes
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8}, // diagonales
        {2, 4, 6}
    };

    for (const auto& ligne : lignesGagnantes)
    {
        if (b[ligne[0]] == joueur && b[ligne[1]] == joueur && b[ligne[2]] == joueur)
        {
            return true;
        }
    }
    return false;
}

bool plateauPlein(const Board& b) // état partie en cours ou pas (null)
{
    for (char c : b)
    {
        if (c == ' ')
            return false;
    }
    return true;
}

int demanderCoup(const Board& b, char joueur)
{
    int choix = -1;
    while (true)
    {
        std::cout << "Joueur " << joueur << ", choisissez une case (1-9) : ";
        if (!(std::cin >> choix))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Saisie invalide, recommencez.\n";
            continue;
        }

        if (choix < 1 || choix > 9)
        {
            std::cout << "Veuillez entrer un nombre entre 1 et 9.\n";
            continue;
        }

        int index = choix - 1;
        if (caseLibre(b, index) == false)
        {
            std::cout << "Case deja prise, choisissez-en une autre.\n";
            continue;
        }

        return index;
    }
}

int main()
{
    Board plateau;
    plateau.fill(' ');

    char joueurCourant = GetQuiJoue();

    while (true)
    {
        // Etat de la partie : (0)00=en cours, (1)01=victoire, (2)10=nul
        SetEtatPartie(0); // jeu en cours
        while (GetEtatPartie() == 0)
        {
            afficherPlateau(plateau);
            int index = demanderCoup(plateau, joueurCourant);
            plateau[index] = joueurCourant;

            if (coupGagnant(plateau, joueurCourant))
            {
                afficherPlateau(plateau);
                std::cout << "Le joueur " << joueurCourant << " a gagne !\n";
                // dire qui a perdu si on a le temps
                SetEtatPartie(1); // victoire
            }
            else if (plateauPlein(plateau))
            {
                afficherPlateau(plateau);
                std::cout << "Match nul !\n";
                SetEtatPartie(2); // nul
            }
            else
            {
                SetQuiJoue();
                joueurCourant = GetQuiJoue();
                SetEtatPartie(0); // partie toujours en cours
            }
        }
        parties++;
        _getch();
        plateau.fill(' ');
    }
    return 0;
}