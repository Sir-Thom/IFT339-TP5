#include <iostream>
#include <string>

#include "hashdict.h"

// inclusions peut-être utiles pour la partie 2
#include <algorithm>
#include <cassert>
#include <set>
#include <unordered_map>

using namespace std;

/***********************************
Code pour la partie 2
************************************/

/*
Fonction à implémenter pour la partie 2.  Vous devez retourner le nombre minimum
d'alvéoles que k insertions peuvent rendre non-vide dans une table de hachage de
capacité m, avec l'univers de clés 0 à n-1.  Il n'est pas nécessaire de
retourner les clés à insérer.
*/
int get_min_alveoles(int n, int m, int k) {
    // à implémenter
    return 0;
}

/***********************************
Code pour la partie 1
************************************/
void test_table_vide() {
    hashdict<int, std::string> dict;
    assert(!dict.contient_cle(42));
    assert(!dict.supprimer(42));
    std::cout << "Test table vide: OK\n";
}

void test_collisions() {
    hashdict<int, std::string> dict;
    int collisions[] = {1, 17, 33}; // Exemple: même hash pour cap = 16
    for (int key : collisions) {
        dict.inserer(key, "valeur" + std::to_string(key));
    }

    for (int key : collisions) {
        assert(dict.contient_cle(key));
        assert(dict[key] == "valeur" + std::to_string(key));
    }

    std::cout << "Test collisions: OK\n";
}
void test_redimensionnement() {
    hashdict<int, int> dict;
    int n = 1000; // Cap initiale de 16, donc resize attendu
    for (int i = 0; i < n; i++) {
        dict.inserer(i, i * 10);
    }

    bool ok = true;
    for (int i = 0; i < n; i++) {
        ok &= dict.contient_cle(i) && dict[i] == i * 10;
    }

    std::cout << "Test redimensionnement: " << (ok ? "OK" : "ÉCHEC") << "\n";
}
void test_suppression_et_reinsertion() {
    hashdict<int, int> dict;
    dict.inserer(10, 100);
    dict.inserer(26, 260); // Collision possible
    dict.supprimer(10);

    assert(!dict.contient_cle(10));
    dict.inserer(10, 101);

    assert(dict.contient_cle(10) && dict[10] == 101);
    assert(dict.contient_cle(26) && dict[26] == 260);

    std::cout << "Test suppression et réinsertion: OK\n";
}
void test_acces_via_operator() {
    hashdict<std::string, int> dict;
    assert(dict["inexistant"] == 0); // Clé créée avec valeur par défaut
    dict["inexistant"] = 42;
    assert(dict["inexistant"] == 42);

    std::cout << "Test opérateur []: OK\n";
}
void test_reutilisation_alveoles() {
    hashdict<int, std::string> dict;
    dict.inserer(5, "cinq");
    dict.supprimer(5);

    dict.inserer(5, "five");
    assert(dict.contient_cle(5) && dict[5] == "five");

    std::cout << "Test réutilisation alvéoles: OK\n";
}


// Une structure qui servira à tester des clés arbitraires.  On a besoin de
// spécifier == et != pour savoir si deux objets ont la même clé.  La fct de
// hachage est plus bas.
struct STest {
    int x;
    string str;

    bool operator==(const STest &other) const { return (x == other.x && str == other.str); }

    bool operator!=(const STest &other) const { return (x != other.x || str != other.str); }
};

// Ce qui suit sert à définir une fonction de hachage "custom" pour la classe
// STest Cette définition nous permettra d'utiliser un objet STest comme clé de
// notre hash table, et ce sans avoir à modifier la classe hashtable.
namespace std {
template <>
struct hash<STest> {
    size_t operator()(const STest &k) const {
        // mauvaise fonction de hachage, j'en conviens
        return ((size_t)k.x + k.str.length());
    }
};
}  // namespace std

int main() {
    hashdict<int, int> dict;

    int num_ins = 10000;

    int nb_ok = 0;

    // On fait num_ins insertions et on vérifie que contientCle valide que la clé
    // est présente ou non
    for (int i = 0; i < num_ins; i++) {
        dict.inserer(i, i + 10);

        int val = dict[i];
        if (val == i + 10) nb_ok++;
        bool contient = dict.contient_cle(i);
        if (contient) nb_ok++;
        bool contient2 = dict.contient_cle(i + 10);
        if (!contient2) nb_ok++;
    }

    cout << "Test du 100000 insertions, verification de [ ] et contient vs ne "
            "contient pas"
         << endl;
    cout << "nb_ok = " << nb_ok << "/" << 3 * num_ins << endl;

    // quelques tests avec le [ ]
    dict[100] = 0;  // réaffecte la valeur associée à 100
    dict[200] = 1;
    dict[num_ins + 10] = 2;  // va **insérer** la valeur 2 associée à la clé num_ins + 20
    dict[num_ins + 20];      // va créer une valeur par défaut associée à la clé num_ins + 20

    cout << "Test table[100] : " << (dict[100] == 0 ? "OK" : "PAS OK!") << endl;
    cout << "Test table[200] : " << (dict[200] == 1 ? "OK" : "PAS OK!") << endl;
    cout << "Test table[300] : " << (dict[300] == 1 ? "PAS OK!" : "OK") << endl;
    cout << "Test table[" << num_ins + 10 << "] : " << (dict[num_ins + 10] == 2 ? "OK" : "PAS OK!")
         << endl;
    cout << "Test table[" << num_ins + 15
         << "] : " << (dict.contient_cle(num_ins + 15) ? "PAS OK!" : "OK") << endl;
    cout << "Test table[" << num_ins + 20
         << "] : " << (dict.contient_cle(num_ins + 20) ? "OK" : "PAS OK") << endl;

    // afficher quelques valeurs pour voir
    cout << "Test d'affichage de quelques valeurs" << endl;
    for (int i = 90; i < 220; i++) {
        cout << dict[i] << " ";
        if ((i - 90 + 1) % 20 == 0) cout << endl;
    }
    cout << endl;

    // test de constructeur par copie et operator=, que j'ai ajoutés juste pour
    // vérifier que le tout compile
    hashdict<int, int> dict_copy1(dict);
    hashdict<int, int> dict_copy2;
    dict_copy2 = dict;

    cout << "Test de constructeur par copie et operator=: OK" << endl;

    // tests de suppression
    cout << "Test de suppressions" << endl;
    dict.supprimer(1000);
    cout << "Test supprimer 1000 : "
         << ((dict.contient_cle(1000) || !dict.contient_cle(1010)) ? "PAS OK!" : "OK") << endl;
    for (int i = 0; i < num_ins; i += 100) {
        dict.supprimer(i);
    }
    nb_ok = 0;
    for (int i = 0; i < num_ins; i++) {
        bool isIn = dict.contient_cle(i);
        if (i % 100 == 0 && !isIn)
            nb_ok++;
        else if (i % 100 != 0 && isIn)
            nb_ok++;
    }
    cout << "Test des suppressions de i % 100 == 0" << endl;
    cout << "nb_ok = " << nb_ok << "/" << num_ins << endl;

    cout << endl;

    hashdict<STest, string> dict2;
    STest s;
    s.x = 1;
    s.str = "1234";
    dict2[s] = "allo";
    cout << "Test table2[" << s.str << "] : " << (dict2[s] == "allo" ? "OK" : "PAS OK") << endl;

    STest s2;
    s2.x = 2;
    s2.str = "123456";
    dict2[s2];
    cout << "Test table2[" << s2.str << "] : " << (dict2.contient_cle(s2) ? "OK" : "PAS OK")
         << endl;

    STest s3;
    s3.x = 2;
    s3.str = "234567";
    dict2[s3] = "salut";
    cout << "Test table2[" << s3.str << "] : " << (dict2[s3] == "salut" ? "OK" : "PAS OK") << endl;

    cout << "Test table2[" << s2.str << "] : " << (dict2[s2] == "salut" ? "PAS OK" : "OK") << endl;

    STest s4;
    s4.x = 3;
    s4.str = "345678";
    cout << "Test table2[" << s4.str << "] : " << (dict2.contient_cle(s4) ? "PAS OK!" : "OK")
         << endl;

    cout << "Test de clear" << endl;

    hashdict<int, int> dico;

    // Ajout de quelques éléments
    for (int i = 0; i < 10; ++i) {
        dico.inserer(i, i * 10);
    }

    std::cout << "Avant clear:" << std::endl;
    std::cout << "Taille: " << dico.size() << std::endl;

    // Appel de la méthode clear
    dico.clear();

    std::cout << "Après clear:" << std::endl;
    std::cout << "Taille: " << dico.size() << std::endl;
    test_table_vide();
    test_collisions();
    test_redimensionnement();
    test_suppression_et_reinsertion();
    test_acces_via_operator();
    test_reutilisation_alveoles();

    /***********************************
                    Tests partie 2
    ************************************/
    cout << endl << endl << "Tests de la partie 2" << endl;
    cout << "NOTE: il se peut que vos valeurs ne soient pas les memes, car la "
            "fonction de hachage depend de votre version de C++"
         << endl
         << endl;
    int a1 = get_min_alveoles(1000, 100, 100);
    cout << "get_min_alveoles(n = 1000, m = 100, k = 100) = " << a1 << endl;

    int a2 = get_min_alveoles(10000, 100, 100);
    cout << "get_min_alveoles(10000, 100, 100) = " << a2
         << "         (ceci devrait toujours etre 1)" << endl;

    int a3 = get_min_alveoles(1000, 500, 500);
    cout << "get_min_alveoles(1000, 500, 500) = " << a3 << endl;

    int a4 = get_min_alveoles(1000, 1000, 1000);
    cout << "get_min_alveoles(1000, 1000, 1000) = " << a4 << endl;

    int a5 = get_min_alveoles(1000, 5000, 1000);
    cout << "get_min_alveoles(1000, 10000, 1000) = " << a5
         << "         (ceci devrait 1000, ou tres proche de 1000)" << endl;

    int a6 = get_min_alveoles(1000, 5000, 1);
    cout << "get_min_alveoles(1000, 5000, 1) = " << a6 << "         (ceci devrait toujours etre 1)"
         << endl;

    return 0;
}
