//!
/// @brief: regdef.h  define registry define.
///

#if !defined(regshot_reg_def_h_)
#define regshot_reg_def_h_

#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cclib/types.h>
#include <cclib/smartptr_def.h>

using namespace cclib;

#if 0

typedef struct _VNode {
    /// value data type.
    reg::VType type;
    std::string name;
    std::vector<byte_t> data;

    _VNode *brother;
    _KNode *father;

    /// compare result: equal/new/del/modify?

    
} VNode;

typedef struct _KNode {
    /// key's name.
    std::string name;

    /// constructe a tree,   
    ///     knode  ----------> first vnode
    ///      / 
    ///     /
    ///  1st subk 
    _VNode *firstv;
    _KNode *firstsubk;
    _KNode *brother;
    _KNode *father;
    ///
} KNode;

#endif

enum reg_dtype {
    regSZ,
};

struct VNode;
struct KNode;

typedef std::vector<byte_t> buffer_t;
typedef std::list<shared_ptr<VNode>> ValueList;
typedef std::list<shared_ptr<KNode>> KeyList;

const reg_dtype kdftregdtype = regSZ;

struct VNode {
    std::string name_;
    reg_dtype type_;
    buffer_t data_;
    /// father key.
    weak_ptr<KNode> fkey_;

    
    VNode(
        std::string const &name = "", reg_dtype type = kdftregdtype, 
        buffer_t const &data = buffer_t(), weak_ptr<KNode> const &fkey = weak_ptr<KNode>()
        )
        : name_(name), type_(type), data_(data), fkey_(fkey)
    {
    }
};

struct KNode {
    std::string name_;
    /// value list;
    ValueList vlist_;
    /// brother list;
    KeyList blist_;
    /// father key.
    weak_ptr<KNode> fkey_;

    KNode(
        std::string const &name = "", ValueList const &vlist = ValueList(),
        KeyList const &blist = KeyList(), weak_ptr<KNode> const &fkey = weak_ptr<KNode>()
        )
        : name_(name), vlist_(vlist), blist_(blist), fkey_(fkey)
    {
    
    }
};

struct regshot {
    KeyList HKLM;
    KeyList HKUS;

    regshot();
    regshot(std::string const &file);

private:
    /// do shot timestamp.
    FILETIME shot_ts_;

    void doshot();
};

enum ModifyStatus {
    StatusNoMatch,
    StatusMatch,
    StatusAdded,
    StatusDeled,
    StatusModed,
};

typedef std::map<ModifyStatus, KeyList> ModifyList;

bool make_diff(regshot const &shot1, regshot const &shot2, std::vector<ModifyList> *diff);

#endif // regshot_reg_def_h_