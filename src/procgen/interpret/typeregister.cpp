#include "typeregister.h"

namespace ProcGen {
bool TypeRegister::addAlias(const std::string& alias, const std::string& aliased)
{
    // if aliased type exists and alias doesn't
    if (names.find(aliased) != names.end()
        && names.find(alias) == names.end()) {
        names[alias] = names[aliased];
        return true;
    }
    return false;
}    

bool TypeRegister::addComposite(const std::string& typeName, std::vector<TypeId> types)
{
    // Fail on invalid type
    for (auto type : types) {
        if (hasTypeWithID(type) == false)
            return false;
    }

    TypeId id = highest++;
    unsigned int size = 0;
    for (auto& x : types)
        size += this->getType(x)->getAlignedSize();

    std::vector<std::string> componentNames;
    this->types[id] = std::make_shared<CompositeType>(shared_from_this(), size, types, componentNames);
    names[typeName] = id;

    return true;
}



std::shared_ptr<Resource> TypeRegister::createResourceFromJson(json symbol)
{
    if (symbol.is_object()) {
        // if structure doesn't have type,
        // then we can't determine what kind of structure it is
        if (symbol.find("_type") == symbol.end())
            return nullptr;
        auto structureTypeName = symbol["_type"].get<std::string>();

        // create structure
        auto structureInstance = this->sharedResource(structureTypeName);
        if (structureInstance == nullptr)
            return nullptr;
        if (structureInstance->getResourceType() != ResourceType::COMPOSITE)
            return nullptr;
        auto compositeStructureInstance = std::static_pointer_cast<CompositeResource>(structureInstance);

        // create submembers
        for (json::iterator it = symbol.begin(); it != symbol.end(); ++it) {
            // ignore _type member
            if (it.key() == "_type")
                continue;
            auto submember = createResourceFromJson(it.value());
            if (submember == nullptr)
                return nullptr;

            LOG_INFO("SUBMEMBER %s\n", it.key().c_str());

            auto memberPosition = compositeStructureInstance->getComponentPosition(it.key());
            if (memberPosition == COMPOSITE_COMPONENT_NOT_FOUND)
                return nullptr;
            auto memberInstance = compositeStructureInstance->getComponent(memberPosition);
            if (memberInstance == nullptr)
                return nullptr;
            memberInstance->copy(submember);
        }
        return structureInstance;
    } else if (symbol.is_array()) {
        // build collection
        LOG_INFO("ARRAY \n");
        auto collection = std::dynamic_pointer_cast<CollectionResource>(this->sharedResource("collection"));
        for (auto& element : symbol) {
            auto elementInstance = this->createResourceFromJson(element);
            collection->append(elementInstance);
        }
        return collection;
    } else if (symbol.is_number()) {
        LOG_INFO("NUMBER \n");
        if (symbol.is_number_float()) {
            auto floatInstance = this->sharedResource("float");
            if (floatInstance == nullptr)
                return nullptr;
            *(float*)floatInstance->getData() = symbol.get<float>();
            return floatInstance;
        } else {
            auto intInstance = this->sharedResource("int");
            if (intInstance == nullptr)
                return nullptr;
            *(int*)intInstance->getData() = symbol.get<int>();
            return intInstance;
        }
    }
    return nullptr;
}

}
