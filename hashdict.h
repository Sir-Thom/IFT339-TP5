#ifndef HASHDICT_H
#define HASHDICT_H

/*
Implémentation d'un dictionnaire avec une table de hachage.
On peut supposer que TYPECLE est hachable.  Ceci veut dire que vous pouvez
transformer une clé en une adresse entre 0 et cap avec le code
(std::hash<TYPECLE>()(cle) % cap)
*/
#include <algorithm>
#include <cstddef>
#include <functional>
template <typename TYPECLE, typename TYPEVAL>
class hashdict {
   private:
    struct alveole {
        TYPECLE cle;
        TYPEVAL val;

        alveole() {}

        alveole(const TYPECLE &cle, const TYPEVAL &val) {
            this->cle = cle;
            this->val = val;
        }
    };

    /*
     * table est un tableau de pointeurs d'alvéoles.  Il y a 3 cas pour un élément
     * table[i]: Si table[i] == nullptr, alors l'alvéole i est inutilisée. Si
     * table[i] == bidon, alors l'alvéole ne contient rien et est libre pour
     * insertion, mais il y a déjà eu un élément qui a été supprimé. Sinon,
     * table[i] contient un élément utilisateur.
     */
    alveole **table;

    alveole *bidon;  // alvéole bidon, réutilisable à plusieurs endroits

    size_t cap;     // capacité de table
    size_t nbelem;  // nombre d'éléments utilisateur dans table

    /*
    Vous pouvez ajouter des fonctions privées, au besoin, mais pas de nouvelles
    variable.
    */

   public:
    hashdict();

    hashdict(const hashdict &src);

    hashdict &operator=(const hashdict &src);

    ~hashdict();

    /*
     * Supprime tous les éléments et met la capacité à une valeur par défaut
     */
    void clear();

    /*
     * Insère la clé et l'associe à val.  Retourne true s'il y a eu insertion, ou
     * false si la clé était déjà présente
     */
    bool inserer(const TYPECLE &cle, const TYPEVAL &val);

    /*
     * Retourne true si la clé est présente, et false sinon
     */
    bool contient_cle(const TYPECLE &cle) const;

    /*
     * Supprime la clé.
     */
    bool supprimer(const TYPECLE &cle);
    void resize();
    /*
     * Retourne une référence vers la valeur associée à cle.  Si la clé est
     * absente, elle sera d'abord insérée et associée à une valeur créée par le
     * constructeur par défaut.
     */
    TYPEVAL &operator[](const TYPECLE &cle);

    /*
     * Retourne le nombre d'éléments
     */
    size_t size() { return nbelem; }
};

template <typename TYPECLE, typename TYPEVAL>
hashdict<TYPECLE, TYPEVAL>::hashdict() {
    cap = 16;
    nbelem = 0;
    table = new alveole *[cap];
    for (size_t i = 0; i < cap; ++i) {
        table[i] = nullptr;
    }
    bidon = new alveole();
}
template <typename TYPECLE, typename TYPEVAL>
hashdict<TYPECLE, TYPEVAL>::~hashdict() {
    for (size_t i = 0; i < cap; ++i) {
        delete table[i];
        table[i] = nullptr;
    }
    delete[] table;
    table = nullptr;
    delete bidon;
    bidon = nullptr;
}

// à vous d'ajouter le code d'implémentation de vos fonctions de classe

template <typename TYPECLE, typename TYPEVAL>
hashdict<TYPECLE, TYPEVAL>::hashdict(const hashdict &src) {
    // par clémence, je l'ai implémenté pour vous - vous pouvez modifier au besoin
    table = nullptr;
    bidon = new alveole();  // une alvéole arbitraire qui servira de bidon

    *this = src;
}

template <typename TYPECLE, typename TYPEVAL>
hashdict<TYPECLE, TYPEVAL> &hashdict<TYPECLE, TYPEVAL>::operator=(const hashdict &src) {
    // par clémence, je l'ai implémenté pour vous - vous pouvez modifier au besoin
    if (this == &src) return *this;

    if (table) {
        // exceptionnellement, si vous avez à répéter ce bout de code de nettoyage,
        // ce sera toléré
        for (int i = 0; i < cap; ++i) delete table[i];
        delete[] table;
    }

    cap = src.cap;
    nbelem = src.nbelem;

    table = new alveole *[src.cap];  // créé un tableau de pointeurs
    for (size_t i = 0; i < src.cap; i++) {
        if (src.table[i] == src.bidon)
            table[i] = bidon;
        else if (src.table[i]) {
            alveole *alv = new alveole(src.table[i]->cle, src.table[i]->val);
            table[i] = alv;
        } else
            table[i] = nullptr;
    }
    return *this;
}
template <typename TYPECLE, typename TYPEVAL>
void hashdict<TYPECLE, TYPEVAL>::resize() {
    size_t old_cap = cap;
    cap *= 2;
    auto new_table = new alveole *[cap];
    for (size_t i = 0; i < cap; ++i) {
        new_table[i] = nullptr;
    }
    for (size_t i = 0; i < old_cap; ++i) {
        if (table[i] && table[i] != bidon) {
            size_t index = std::hash<TYPECLE>()(table[i]->cle) % cap;
            while (new_table[index] != nullptr) {
                index = (index + 1) % cap;
            }
            new_table[index] = table[i];
        }
    }
    delete[] table;
    table = new_table;
}
template <typename TYPECLE, typename TYPEVAL>
bool hashdict<TYPECLE, TYPEVAL>::contient_cle(const TYPECLE &cle) const {
    size_t index = std::hash<TYPECLE>()(cle) % cap;
    return table[index] != nullptr && table[index] != bidon && table[index]->cle == cle;
}

template <typename TYPECLE, typename TYPEVAL>
bool hashdict<TYPECLE, TYPEVAL>::inserer(const TYPECLE &cle, const TYPEVAL &val) {
    size_t index = std::hash<TYPECLE>()(cle);
    if (nbelem >= cap / 2) {
        resize();
    }
    while (table[index] != nullptr && table[index] != bidon) {
        if (table[index]->cle == cle) {
            return false;
        }
        index = (index + 1) % cap;
    }
    if (table[index] == nullptr || table[index] == bidon) {
        table[index] = new alveole(cle, val);
        nbelem++;
        return true;
    }

    return false;
}

template <typename TYPECLE, typename TYPEVAL>
bool hashdict<TYPECLE, TYPEVAL>::supprimer(const TYPECLE &cle) {
    size_t index = std::hash<TYPECLE>()(cle);
    while (table[index] != nullptr && table[index] != bidon) {
        if (table[index]->cle == cle) {
            table[index] = bidon;
            nbelem--;

            return true;
        }
        index = (index + 1) % cap;
    }

    return false;
}
template <typename TYPECLE, typename TYPEVAL>
TYPEVAL &hashdict<TYPECLE, TYPEVAL>::operator[](const TYPECLE &cle) {
    size_t index = std::hash<TYPECLE>()(cle) % cap;
    while (table[index] != nullptr && table[index] != bidon) {
        if (table[index]->cle == cle) {
            return table[index]->val;
        }
        index = (index + 1) % cap;
    }

    if (nbelem >= cap / 2) {
        resize();
        index = std::hash<TYPECLE>()(cle) % cap;
    }

    while (!index || table[index] == bidon) {
        index = (index + 1) % cap;
    }
    table[index] = new alveole(cle, TYPEVAL());
    nbelem++;
    return table[index]->val;
}

template <typename TYPECLE, typename TYPEVAL>
void hashdict<TYPECLE, TYPEVAL>::clear() {}

#endif  // HASHDICT_H
