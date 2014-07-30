//!
/// @brief: regdef.h  define registry define.
///

#if !defined(regshot_reg_def_h_)
#define regshot_reg_def_h_

#include <windows.h>
#include <string>
#include <cclib/types.h>
#include <cclib/win32/types.h>


using namespace cclib;
using namespace cclib::win32;

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



#endif // regshot_reg_def_h_