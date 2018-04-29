#ifndef _TYPEDESC_H
#define _TYPEDESC_H
#include <memory>
#include <stddef.h>
#include <vector>

namespace ProcGen {
using TypeId = size_t;
enum descType { ATOMIC,
    COMPOSITE,
    COLLECTION,
    ABSTRACT,
    ANY };

class TypeRegister;

/**
* @class AbstractType
* @brief Interface for type description
*
* For each type of Resource (e.g. Atomic, Composite...), there is a special
* class describing the resource (e.g. AtomicType, CompositeType).
* Description includes:
*   - types of members
*   - names of members
* Each type has a global description instance which is shared by type instances.
*
* Composite types can build hierarchies of types according to composite pattern.
*/
class AbstractType {
protected:
    unsigned int size;

public:
    // Virtual destructor
    AbstractType()
        : size(0)
    {
    }
    virtual ~AbstractType() {}
    /**
* @brief Get typedescription type of class
*
* @return one of enum descType (ATOMIC, COLLECTION, ...)
*/
    virtual descType getType() { return ABSTRACT; }
    /**
* @brief Deprecated
*
* @return 
*/
    unsigned int getSize() { return size; }
    /**
* @brief Deprecated
*
* @return 
*/
    unsigned int getAlignedSize();
};

/**
* @class AtomicType
* @brief Metaclass for exact types (int, float, ...)
*/
class AtomicType : public AbstractType {
public:
    AtomicType(unsigned int size)
    {
        this->size = size;
    }
    virtual descType getType() { return ATOMIC; };
};

const size_t COMPOSITE_COMPONENT_NOT_FOUND = -1;

/**
* @class CompositeType
* @brief Metaclass for structured types
*/
class CompositeType : public AbstractType {
    std::weak_ptr<TypeRegister> tr;

public:
    CompositeType(std::weak_ptr<TypeRegister>,
        unsigned int size, std::vector<TypeId> compos, std::vector<std::string> items);
    virtual descType getType() { return COMPOSITE; };
    /**
* @brief Has member with name
*/
    bool hasComponentWithName(const std::string name) const;
    /**
* @brief Get order of member subtype
*
* @param name
*
* @return -1 if name isn't member 
*/
    size_t getComponentPositionByName(const std::string name) const;
    /**
* @brief Get member name from order (position)
*
* @param componentID
*
* @return 
*/
    const std::string getComponentName(size_t componentID) const;
    /**
* @brief Get member TypeId from order
*
* @param componentID
*
* @return 
*/
    TypeId getComponentTypeId(size_t componentID) const;

    size_t getComponentCount() { return this->components.size(); }

protected:
    std::vector<TypeId> components;
    std::vector<std::string> componentsNames;
};
/**
* @class CollectionType
* @brief Holds global collection type
*/
class CollectionType : public AbstractType {
public:
    CollectionType();
    virtual descType getType() { return COLLECTION; }
};

/**
* @class AnyType
* @brief Holds special any type
*/
class AnyType : public AbstractType {
public:
    AnyType(){};
    virtual descType getType() { return ANY; }
};
}
#endif
