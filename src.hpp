// src.hpp - Pokedex implementation for ACMOJ 1277
// Single-header submission as required by the OJ.

#include <bits/stdc++.h>
using namespace std;

// Exception hierarchy
class BasicException {
protected:
    string message_;
public:
    explicit BasicException(const char *_message): message_(_message ? _message : "") {}
    virtual const char *what() const { return message_.c_str(); }
    virtual ~BasicException() = default;
};

class ArgumentException: public BasicException {
public:
    explicit ArgumentException(const char *m): BasicException(m) {}
};

class IteratorException: public BasicException {
public:
    explicit IteratorException(const char *m): BasicException(m) {}
};

struct Pokemon {
    char name[12];
    int id;
    vector<string> types;
    Pokemon() : name{0}, id(0) {}
    Pokemon(const string &n, int i, const vector<string> &t): id(i), types(t) {
        memset(name, 0, sizeof(name));
        size_t len = min<size_t>(sizeof(name)-1, n.size());
        memcpy(name, n.c_str(), len);
    }
};

static inline string tolower_str(string s){ for (auto &c:s) c=tolower((unsigned char)c); return s; }

class Pokedex {
private:
    string fileName;
    map<int, Pokemon> byId; // ascending by id

    vector<string> allowedTypes = {"normal","fire","water","grass","electric","ground","flying"};
    unordered_map<string,int> typeIndex;
    float eff[7][7]; // atk x def multiplier

    void init_type_index(){
        typeIndex.clear();
        for (int i=0;i<(int)allowedTypes.size();++i) typeIndex[allowedTypes[i]]=i;
    }

    void init_type_chart(){
        for (int i=0;i<7;++i) for (int j=0;j<7;++j) eff[i][j]=1.0f;
        // Immunities
        eff[typeIndex["ground"]][typeIndex["flying"]] = 0.0f; // Ground vs Flying
        eff[typeIndex["electric"]][typeIndex["ground"]] = 0.0f; // Electric vs Ground
        // Fire
        eff[typeIndex["fire"]][typeIndex["grass"]] = 2.0f;
        eff[typeIndex["fire"]][typeIndex["water"]] = 0.5f;
        eff[typeIndex["fire"]][typeIndex["fire"]] = 0.5f;
        // Water
        eff[typeIndex["water"]][typeIndex["fire"]] = 2.0f;
        eff[typeIndex["water"]][typeIndex["grass"]] = 0.5f;
        eff[typeIndex["water"]][typeIndex["water"]] = 0.5f;
        eff[typeIndex["water"]][typeIndex["ground"]] = 2.0f;
        // Grass
        eff[typeIndex["grass"]][typeIndex["water"]] = 2.0f;
        eff[typeIndex["grass"]][typeIndex["ground"]] = 2.0f;
        eff[typeIndex["grass"]][typeIndex["fire"]] = 0.5f;
        eff[typeIndex["grass"]][typeIndex["grass"]] = 0.5f;
        eff[typeIndex["grass"]][typeIndex["flying"]] = 0.5f;
        // Electric
        eff[typeIndex["electric"]][typeIndex["water"]] = 2.0f;
        eff[typeIndex["electric"]][typeIndex["flying"]] = 2.0f;
        eff[typeIndex["electric"]][typeIndex["electric"]] = 0.5f;
        eff[typeIndex["electric"]][typeIndex["grass"]] = 0.5f;
        // Ground
        eff[typeIndex["ground"]][typeIndex["electric"]] = 2.0f;
        eff[typeIndex["ground"]][typeIndex["fire"]] = 2.0f;
        eff[typeIndex["ground"]][typeIndex["grass"]] = 0.5f;
        // Flying
        eff[typeIndex["flying"]][typeIndex["grass"]] = 2.0f;
        eff[typeIndex["flying"]][typeIndex["electric"]] = 0.5f;
        // Normal stays 1 across this restricted set
    }

    static bool valid_name(const string &s){ if(s.empty()||s.size()>10) return false; for(unsigned char c: s){ if(!isalpha(c)) return false; } return true; }
    static bool valid_id(long long id){ return id>0 && id<=1000000000LL; }

    vector<string> parse_types_checked(const char *types) const {
        string s(types?types:"");
        vector<string> out;
        string cur;
        for(char ch: s){
            if(ch=='#'){
                if(!cur.empty()){ out.push_back(tolower_str(cur)); cur.clear(); }
                else { out.push_back(""); }
            } else cur.push_back(ch);
        }
        if(!cur.empty() || (!s.empty() && s.back()=='#')) out.push_back(tolower_str(cur));
        vector<string> clean;
        unordered_set<string> seen;
        for(auto &t: out){ if(t.empty()) continue; clean.push_back(t); }
        if(clean.empty() || clean.size()>7) {
            throw ArgumentException("Argument Error: PM Type Invalid ()");
        }
        for(auto &t: clean){
            if(!typeIndex.count(t)){
                string msg = string("Argument Error: PM Type Invalid (") + t + ")";
                throw ArgumentException(msg.c_str());
            }
            if(!seen.insert(t).second){
                string msg = string("Argument Error: PM Type Duplicated (") + t + ")";
                throw ArgumentException(msg.c_str());
            }
        }
        return clean;
    }

    void load(){
        byId.clear();
        ifstream fin(fileName);
        if(!fin.good()) return;
        string line;
        while(getline(fin,line)){
            if(line.empty()) continue;
            string idstr,namestr,typestr;
            stringstream ss(line);
            if(!(ss>>idstr>>namestr)) continue;
            getline(ss, typestr);
            if(!typestr.empty() && typestr[0]==' ') typestr.erase(0,1);
            long long idv=0; try{ idv=stoll(idstr);}catch(...){continue;}
            if(!valid_id(idv)) continue;
            if(!valid_name(namestr)) continue;
            vector<string> tks; try{ tks = parse_types_checked(typestr.c_str()); }catch(...){ continue; }
            byId[(int)idv] = Pokemon(namestr,(int)idv,tks);
        }
    }

    void save() const {
        ofstream fout(fileName, ios::trunc);
        if(!fout.good()) return;
        for(auto &kv: byId){
            const Pokemon &p = kv.second;
            fout<<p.id<<' '<<p.name<<' ';
            for(size_t i=0;i<p.types.size();++i){ if(i) fout<<'#'; fout<<p.types[i]; }
            fout<<"\n";
        }
    }

public:
    explicit Pokedex(const char *_fileName){
        if(!_fileName) _fileName = "pokedex.db";
        fileName = _fileName;
        init_type_index();
        init_type_chart();
        load();
    }
    ~Pokedex(){
        try{ save(); }catch(...){}
    }

    bool pokeAdd(const char *cname, int id, const char *types){
        string name = cname ? string(cname) : string();
        if(!valid_name(name)) { string msg = string("Argument Error: PM Name Invalid (") + name + ")"; throw ArgumentException(msg.c_str()); }
        if(!valid_id(id)) { string msg = string("Argument Error: PM ID Invalid (") + to_string(id) + ")"; throw ArgumentException(msg.c_str()); }
        vector<string> tks = parse_types_checked(types);
        if(byId.count(id)) return false;
        byId[id] = Pokemon(name,id,tks);
        return true;
    }

    bool pokeDel(int id){
        auto it = byId.find(id);
        if(it==byId.end()) return false;
        byId.erase(it);
        return true;
    }

    string pokeFind(int id) const {
        auto it = byId.find(id);
        if(it==byId.end()) return string("None");
        return string(it->second.name);
    }

    string typeFind(const char *types) const {
        vector<string> tks = const_cast<Pokedex*>(this)->parse_types_checked(types);
        vector<const Pokemon*> res;
        for(auto &kv: byId){
            const Pokemon &p = kv.second;
            bool ok=true;
            for(auto &t: tks){ if(find(p.types.begin(), p.types.end(), t)==p.types.end()){ ok=false; break; } }
            if(ok) res.push_back(&p);
        }
        if(res.empty()) return string("None");
        string out = to_string(res.size());
        for(auto *pp: res){ out += "\n"; out += string(pp->name); }
        return out;
    }

    float attack(const char *type, int id) const {
        string t = tolower_str(string(type?type:""));
        if(!typeIndex.count(t)){
            string msg = string("Argument Error: PM Type Invalid (") + t + ")";
            throw ArgumentException(msg.c_str());
        }
        auto it = byId.find(id);
        if(it==byId.end()) return -1.0f;
        int atk = typeIndex.at(t);
        float mul = 1.0f;
        for(const string &df: it->second.types){
            int di = typeIndex.at(df);
            mul *= eff[atk][di];
        }
        return mul;
    }

    int catchTry() const {
        if(byId.empty()) return 0;
        auto it0 = byId.begin();
        unordered_set<int> owned;
        owned.insert(it0->first);
        bool changed=true;
        while(changed){
            changed=false;
            unordered_set<int> atkset;
            for(int id: owned){ const auto &p = byId.at(id); for(const string &t: p.types) atkset.insert(typeIndex.at(t)); }
            for(auto &kv: byId){ int pid=kv.first; if(owned.count(pid)) continue; const Pokemon &def = kv.second;
                bool capt=false;
                for(int atk: atkset){ float mul=1.0f; for(const string &td: def.types){ mul *= eff[atk][typeIndex.at(td)]; } if(mul >= 2.0f - 1e-6f){ capt=true; break; } }
                if(capt){ owned.insert(pid); changed=true; }
            }
        }
        return (int)owned.size();
    }

    struct iterator {
        using MapIter = map<int,Pokemon>::iterator;
        Pokedex *dex=nullptr;
        MapIter it;
        iterator()=default;
        iterator(Pokedex *d, MapIter i): dex(d), it(i) {}
        iterator &operator++(){ if(!dex) throw IteratorException("Iterator Error: Invalid Iterator"); if(it==dex->byId.end()) throw IteratorException("Iterator Error: Out Of Range"); ++it; return *this; }
        iterator &operator--(){ if(!dex) throw IteratorException("Iterator Error: Invalid Iterator"); if(it==dex->byId.begin()) throw IteratorException("Iterator Error: Out Of Range"); --it; return *this; }
        iterator operator++(int){ iterator tmp=*this; ++(*this); return tmp; }
        iterator operator--(int){ iterator tmp=*this; --(*this); return tmp; }
        iterator & operator = (const iterator &rhs){ dex=rhs.dex; it=rhs.it; return *this; }
        bool operator == (const iterator &rhs) const { return dex==rhs.dex && it==rhs.it; }
        bool operator != (const iterator &rhs) const { return !(*this==rhs); }
        Pokemon & operator*() const { if(!dex||it==dex->byId.end()) throw IteratorException("Iterator Error: Dereference"); return it->second; }
        Pokemon *operator->() const { if(!dex||it==dex->byId.end()) throw IteratorException("Iterator Error: Dereference"); return &it->second; }
    };

    iterator begin(){ return iterator(this, byId.begin()); }
    iterator end(){ return iterator(this, byId.end()); }
};
