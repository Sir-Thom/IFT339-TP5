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
        if (table[i] && table[i] != bidon) {
            delete table[i];
        }
    }
    delete[] table;

    if (bidon) {
        delete bidon;
        bidon = nullptr;
    }
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
        for (size_t i = 0; i < cap; ++i) delete table[i];
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
    alveole **old_table = table;

    cap *= 2;
    table = new alveole *[cap];
    for (size_t i = 0; i < cap; ++i) {
        table[i] = nullptr;
    }

    nbelem = 0;
    for (size_t i = 0; i < old_cap; ++i) {
        if (old_table[i] && old_table[i] != bidon) {
            inserer(old_table[i]->cle, old_table[i]->val);
            delete old_table[i];
        }
    }
    delete[] old_table;
}

template <typename TYPECLE, typename TYPEVAL>
bool hashdict<TYPECLE, TYPEVAL>::contient_cle(const TYPECLE &cle) const {
    size_t index = std::hash<TYPECLE>()(cle) % cap;
    size_t start_index = index;

    while (table[index] != nullptr) {
        if (table[index] != bidon && table[index]->cle == cle) {
            return true;
        }
        index = (index + 1) % cap;

        if (index == start_index) {
            break;
        }
    }
    return false;
}

template <typename TYPECLE, typename TYPEVAL>
bool hashdict<TYPECLE, TYPEVAL>::inserer(const TYPECLE &cle, const TYPEVAL &val) {
    size_t index = std::hash<TYPECLE>()(cle) % cap;

    if (nbelem >= cap / 2) {
        resize();
        index = std::hash<TYPECLE>()(cle) % cap;
    }
    while (table[index] != nullptr && table[index] != bidon) {
        if (table[index] != nullptr && table[index]->cle == cle) {
            return false;
        }
        index = (index + 1) % cap;
    }

    if (table[index] != nullptr && table[index] != bidon) {
        delete table[index];
    }
    table[index] = new alveole(cle, val);
    ++nbelem;
    return true;
}

template <typename TYPECLE, typename TYPEVAL>
bool hashdict<TYPECLE, TYPEVAL>::supprimer(const TYPECLE &cle) {
    if (nbelem == 0) {
        // La table est vide, il n'y a rien à supprimer
        return false;
    }
    size_t index = std::hash<TYPECLE>()(cle);
    while (table[index] != nullptr) {
        if (table[index] != bidon && table[index]->cle == cle) {
            delete table[index];
            table[index] = bidon;
            --nbelem;
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
    if (table[index] == nullptr || table[index] == bidon) {
        table[index] = new alveole(cle, TYPEVAL());
        ++nbelem;
    }
    return table[index]->val;
}

template <typename TYPECLE, typename TYPEVAL>
void hashdict<TYPECLE, TYPEVAL>::clear() {
    for (size_t i = 0; i < cap; i++) {
        delete table[i];
    }
    delete[] table;
    nbelem = 0;
    cap = 16;
    table = new alveole *[cap];
    for (size_t i = 0; i < cap; i++) {
        table[i] = nullptr;
    }
}

#endif  // HASHDICT_H
