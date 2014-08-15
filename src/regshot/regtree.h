//
/// brief: define reg tree structure.
/// NOTICE: use composite design partern to implement tree structure.
///

#if !defined(reg_tree_h_)
#define reg_tree_h_

#include <windows.h>
#include <string>
#include <list>
#include <cclib/smartptr_def.h>

enum NodeType {
    NTValue,
    NTKey,
};

struct Node {
    Node(NodeType type, std::string const &name) 
        : type_(type), name_(name) 
    {}

    virtual ~Node() {};  
    virtual void Add(shared_ptr<Node> node) = 0;
    virtual bool Remove(shared_ptr<Node> node) = 0;
    virtual std::list<shared_ptr<Node>> const& child() const = 0;

    NodeType    type() const { return type_; }
    std::string name() const { return name_; }
private:
    NodeType type_;
    std::string name_;
};

struct Value : public Node {

    Value(std::string const& name, RegType type, ) : Node(NTValue, name) {}

    /// not implement the following interface.
    virtual void Add(shared_ptr<Node> node);
    virtual bool Remove(shared_ptr<Node> node);
    virtual std::list<shared_ptr<Node>> const& child() const;

private:
    std::vector<byte_t> data_;
};

struct Key : public Node {
    Key(std::string const& name) : Node(NTKey, name) {}

    virtual void Add(shared_ptr<Node> node)
    {
        child_.push_back(node);
    }

    virtual bool Remove(shared_ptr<Node> node)
    {
        std::list<shared_ptr<Node>>::iterator it;
        for (it = child_.begin(); it != child_.end(); ++it) {
            if (node == (*it)) {
                child_.erase(it);
                return true;
            }
        }
        return false;
    }

    virtual std::list<shared_ptr<Node>> const& child() const
    {
        return child_;
    }
private:
    std::list<shared_ptr<Node>> child_;
};

#endif // reg_tree_h_
