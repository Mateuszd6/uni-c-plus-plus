#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

class PublicationAlreadyCreated : public std::exception {
  public:
    const char *what() const noexcept {
        return "PublicationAlreadyCreated";
    }
};

class PublicationNotFound : public std::exception {
  public:
    const char *what() const noexcept {
        return "PublicationNotFound";
    }
};

class TriedToRemoveRoot : public std::exception {
  public:
    const char *what() const noexcept {
        return "TriedToRemoveRoot";
    }
};

template <typename Publication>
class CitationGraph {
  private:
    // Forward delcare klasy Node jest potrzebny Ĺźeby zaliasowac map_type
    // uzywany zarowno w Node jak i w CitationGraph
    class Node;

    using id_type = typename Publication::id_type;
    using map_type = std::map<id_type, std::weak_ptr<Node>>;
    using map_iterator = typename map_type::iterator;

    class Node {
      public:
        Publication p;

        map_type *map_ptr;
        map_iterator map_iter;

        std::set<std::shared_ptr<Node>> children;
        std::set<Node *> parents;

        Node(id_type const &p_id, map_type *map_ptr)
            : p(p_id), map_ptr(map_ptr) {
        }

        ~Node() noexcept {
            // ZakĹadamy, Ĺźe Node jest usuwany wtedy i tylko wtedy gdy wszyscy
            // jego rodzice zostali usuniÄci bÄdĹş wywoĹany zostaĹ remove, ktĂłry
            // usuwa z wektorĂłw rodzicĂłw, wiÄc nie musimy przechodziÄ po
            // wektorze parents i siÄ usuwaÄ.

            for (auto child_ptr : children) {
                child_ptr->parents.erase(this);
            }

            if (map_ptr != nullptr)
                map_ptr->erase(map_iter);
        }

        // Usuwa wszystkie krawedzie na ktore wskazuje dany shared_ptr.
        void remove_child(std::shared_ptr<Node> &child_ptr) noexcept {
            children.erase(children.find(child_ptr));
        }
    };

    std::unique_ptr<map_type> map_ptr;
    std::shared_ptr<Node> root_ptr;

    // Dodaje pojedynczego node'a bez krawedzi do mapy. Jedyny shared_ptr do
    // tego node'a to zwracana wartosc. Uzytkownik musi dodac krawedz do tego
    // wierzcholka, albo zostanie on usuniety kiedy shared_ptr opusci
    // scope. Destruktor Node'a poprawnie usuwa go z mapy.
    std::shared_ptr<Node> insert_node_aux(id_type const &id) {
        auto sp = std::make_shared<Node>(id, map_ptr.get());

        try {
            auto [iter, _] = map_ptr->emplace(id, std::weak_ptr<Node>(sp));
            sp->map_iter = iter;
        }
        catch (...) {
            // Nie udaĹo siÄ wstawiÄ do mapy, wiÄc w destruktorze node'a nie
            // bÄdziemy z niej usuwaÄ.
            sp->map_ptr = nullptr;
            throw;
        }

        return sp;
    }

  public:
    // Tworzy nowy graf. Tworzy takĹźe wÄzeĹ publikacji o identyfikatorze
    // stem_id.
    CitationGraph(id_type const &stem_id) {
        map_ptr = std::make_unique<map_type>();
        root_ptr = insert_node_aux(stem_id);
    }

    CitationGraph(CitationGraph<Publication> const &other) = delete;

    CitationGraph<Publication> &
    operator=(CitationGraph<Publication> const &other) = delete;

    // Konstruktor przenoszÄcy i przenoszÄcy operator przypisania.
    CitationGraph(CitationGraph<Publication> &&other) noexcept
        : map_ptr(std::move(other.map_ptr)),
          root_ptr(std::move(other.root_ptr)) {
    }

    CitationGraph<Publication> &
    operator=(CitationGraph<Publication> &&other) noexcept {
        if (this != &other) {
            this->map_ptr = std::move(other.map_ptr);
            this->root_ptr = std::move(other.root_ptr);
        }

        return *this;
    }

    // Zwraca identyfikator ĹşrĂłdĹa. Metoda ta jest noexcept wtedy i tylko
    // wtedy, gdy metoda Publication::get_id jest noexcept.
    id_type get_root_id() const
        noexcept(noexcept(std::declval<Publication>().get_id())) {
        return root_ptr->p.get_id();
    }

    // Zwraca listÄ identyfikatorĂłw publikacji cytujÄcych publikacjÄ o podanym
    // identyfikatorze. ZgĹasza wyjÄtek PublicationNotFound, jeĹli dana
    // publikacja nie istnieje.
    std::vector<id_type> get_children(id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();
        else {
            auto const &children_set = map_ptr->at(id).lock()->children;
            std::vector<id_type> retval{};

            retval.reserve(children_set.size());
            for (auto const &i : children_set)
                retval.emplace_back(i->p.get_id());
            return retval;
        }
    }

    // Zwraca listÄ identyfikatorĂłw publikacji cytowanych przez publikacjÄ o
    // podanym identyfikatorze. ZgĹasza wyjÄtek PublicationNotFound, jeĹli dana
    // publikacja nie istnieje.
    std::vector<id_type> get_parents(id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();
        else {
            auto const &parents_set = map_ptr->at(id).lock()->parents;
            std::vector<id_type> retval{};

            retval.reserve(parents_set.size());
            for (auto const &i : parents_set)
                retval.emplace_back(i->p.get_id());

            return retval;
        }
    }

    // Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(id_type const &id) const {
        return map_ptr->find(id) != map_ptr->end();
    }

    // Zwraca referencjÄ do obiektu reprezentujÄcego publikacjÄ o podanym
    // identyfikatorze. ZgĹasza wyjÄtek PublicationNotFound, jeĹli ĹźÄdana
    // publikacja nie istnieje.
    Publication &operator[](id_type const &id) const {
        auto el = map_ptr->find(id);
        if (el == map_ptr->end()) {
            throw PublicationNotFound();
        }
        return el->second.lock()->p;
    }

    // Tworzy wÄzeĹ reprezentujÄcy nowÄ publikacjÄ o identyfikatorze id cytujÄcÄ
    // publikacje o podanym identyfikatorze parent_id lub podanych
    // identyfikatorach parent_ids. ZgĹasza wyjÄtek PublicationAlreadyCreated,
    // jeĹli publikacja o identyfikatorze id juĹź istnieje. ZgĹasza wyjÄtek
    // PublicationNotFound, jeĹli ktĂłryĹ z wyspecyfikowanych poprzednikĂłw nie
    // istnieje albo lista poprzednikĂłw jest pusta.
    void create(id_type const &id, id_type const &parent_id) {
        if (exists(id))
            throw PublicationAlreadyCreated();

        if (!exists(parent_id))
            throw PublicationNotFound();

        auto sp = insert_node_aux(id);

        // Jesli add_citation rzuci wyjÄtek to krawÄdĹş nie zostaĹa dodana,
        // shared ptr zwolni node'a (bo jedyna referencja do niego to sp na
        // stosie) a on usunie sie z mapy w destruktorze.
        add_citation(id, parent_id);
    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        // JeĹli ten wektor jest pusty, to rzucamy PublicationNotFound
        if (parent_ids.empty())
            throw PublicationNotFound();

        if (exists(id))
            throw PublicationAlreadyCreated();

        for (auto const &parent_id : parent_ids)
            if (!exists(parent_id))
                throw PublicationNotFound();

        auto sp = insert_node_aux(id);
        std::vector<Node *> parents_childs{};

        size_t i = 0;
        try {
            for (; i < parent_ids.size(); ++i) {
                parents_childs.push_back(
                    (*map_ptr)[parent_ids[i]].lock().get());
                add_citation(id, parent_ids[i]);
            }
        }
        catch (...) {
            while (i--)
                parents_childs[i]->children.erase(sp);

            throw;
        }
    }

    // Dodaje nowÄ krawÄdĹş w grafie cytowaĹ. ZgĹasza wyjÄtek
    // PublicationNotFound, jeĹli ktĂłraĹ z podanych publikacji nie istnieje.
    void add_citation(id_type const &child_id, id_type const &parent_id) {
        auto parent_node = map_ptr->find(parent_id);
        auto child_node = map_ptr->find(child_id);

        if (parent_node == map_ptr->end() || child_node == map_ptr->end()) {
            throw PublicationNotFound();
        }

        auto child_node_ptr = child_node->second.lock();
        auto parent_node_ptr = parent_node->second.lock();

        parent_node_ptr->children.emplace(child_node_ptr);

        try {
            child_node_ptr->parents.emplace(parent_node_ptr.get());
        }
        catch (...) {
            parent_node_ptr->children.erase(child_node_ptr);
            throw;
        }

    }

    // Usuwa publikacjÄ o podanym identyfikatorze. ZgĹasza wyjÄtek
    // PublicationNotFound, jeĹli ĹźÄdana publikacja nie istnieje. ZgĹasza
    // wyjÄtek TriedToRemoveRoot przy prĂłbie usuniÄcia pierwotnej publikacji. W
    // wypadku rozspĂłjnienia grafu, zachowujemy tylko spĂłjnÄ skĹadowÄ
    // zawierajÄcÄ ĹşrĂłdĹo.
    void remove(id_type const &id) {
        if (!exists(id)) {
            throw PublicationNotFound();
        }
        else if (id == get_root_id()) {
            throw TriedToRemoveRoot();
        }

        // Wszyskim rodzicom usuwamy shared ptr wiec gdy node_ptr opusci scope
        // node zostanie zniszczony, a jego destruktor usunie go z mapy.
        {
            auto node_ptr = map_ptr->at(id).lock();
            for (auto const &i : node_ptr->parents) {
                i->remove_child(node_ptr);
            }
        }
    }
};

#endif // CITATION_GRAPH_H
