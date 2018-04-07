#include <procgen/interpret/resource.h>
#include <procgen/interpret/typereg.h>
#include <sstream>

bool Resource::hasSameType(std::shared_ptr<Resource> res)
{
    return (this->getBaseId() == res->getBaseId());
}

std::shared_ptr<Resource> Resource::allocateClone()
{
    return this->tr.lock()->sharedResource(this->getBaseId());
}
bool AtomicResource::copy(const std::shared_ptr<Resource> src)
{
    if (!value)
        return false;
    auto tmp = src;

    // if src is ANY, then use it content
    if (src->getResourceType() == ResourceType::ANY)
        tmp = std::dynamic_pointer_cast<AnyResource>(src)->getContent();

    if (tmp == nullptr)
        return false;

    if (tmp->getBaseId() != baseType)
        return false;

    size_t size = tr.lock()->getType(baseType)->getSize();
    memcpy(this->value, tmp->getData(), size);

    return true;
}

json AtomicResource::to_json() const
{
    std::string nameOfType = tr.lock()->getTypeName(baseType);
    if (nameOfType == "float")
        return json(*(float*)this->getData());

    if (nameOfType == "int")
        return json(*(int*)this->getData());

    if (nameOfType == "bool")
        return json(*(bool*)this->getData());

    return json("unkAtomicValue");
}

bool AtomicResource::isInteger()
{
    std::string nameOfType = tr.lock()->getTypeName(baseType);
    return (nameOfType == "int");
}
bool AtomicResource::isFloat()
{
    std::string nameOfType = tr.lock()->getTypeName(baseType);
    return (nameOfType == "float");
}

std::string Resource::getTypeName() const
{
    return tr.lock()->getTypeName(this->baseType);
}

int AtomicResource::getInteger()
{
    return *(int*)this->getData();
}
float AtomicResource::getFloat()
{
    return *(float*)this->getData();
}

bool CompositeResource::copy(const std::shared_ptr<Resource> src)
{
    auto tmp = src;
    // if src is ANY, then use it content
    if (src->getResourceType() == ResourceType::ANY)
        tmp = std::dynamic_pointer_cast<AnyResource>(src)->getContent();

    if (tmp == nullptr)
        return false;

    auto srcComposite = std::dynamic_pointer_cast<CompositeResource>(tmp);
    for (size_t i = 0; i < this->components.size(); i++) {
        this->getComponent(i)->copy(srcComposite->getComponent(i));
    }
    return true;
}

size_t CompositeResource::getComponentCount() const
{
    return this->components.size();
}

void CollectionResource::append(std::shared_ptr<Resource> item)
{
    auto newElement = this->tr.lock()->sharedResource(item->getBaseId());
    newElement->copy(item);
    this->collection.push_back(newElement);
}

void CollectionResource::remove(size_t index)
{
    this->collection.erase(this->collection.begin() + index);
}

std::shared_ptr<Resource> CollectionResource::at(size_t index)
{
    return this->collection[index];
}

bool CollectionResource::copy(const std::shared_ptr<Resource> src)
{

    auto tmp = src;
    // if src is ANY, then use it content
    if (src->getResourceType() == ResourceType::ANY)
        tmp = std::dynamic_pointer_cast<AnyResource>(src)->getContent();

    if (tmp == nullptr)
        return false;

    auto srcCollection = std::dynamic_pointer_cast<CollectionResource>(tmp);
    this->collection.clear();
    for (size_t i = 0; i < srcCollection->length(); i++) {
        auto res = this->tr.lock()->sharedResource(srcCollection->at(i)->getBaseId());
        res->copy(srcCollection->at(i));
        this->collection.push_back(res);
    }
    return true;
}

json CollectionResource::to_json() const
{
    json result;
    for (size_t i = 0; i < collection.size(); i++) {
        result.push_back(collection[i]->to_json());
    }
    return result;
}

json CompositeResource::to_json() const
{
    json object;
    object["_type"] = this->getTypeName();
    for (size_t i = 0; i < this->components.size(); i++) {
        object[getComponentName(i)] = (this->components.at(i)->to_json());
    }
    return json(object);
}

std::shared_ptr<CompositeType> CompositeResource::getCompositeTypeDescription() const
{
    auto typeDesc = tr.lock()->getType(baseType);
    return std::dynamic_pointer_cast<CompositeType>(typeDesc);
}

const std::string CompositeResource::getComponentName(size_t index) const
{
    auto compositeDescription = this->getCompositeTypeDescription();
    return compositeDescription->getComponentName(index);
}

size_t CompositeResource::getComponentPosition(const std::string name) const
{
    auto compositeDescription = this->getCompositeTypeDescription();
    return compositeDescription->getComponentPositionByName(name);
}

TypeId CompositeResource::getComponentType(size_t index) const
{
    auto compositeDescription = this->getCompositeTypeDescription();
    return compositeDescription->getComponentTypeId(index);
}

bool AnyResource::copy(const std::shared_ptr<Resource> src)
{
    auto realsource = src;
    if (src->getResourceType() == ResourceType::ANY) {
        realsource = std::dynamic_pointer_cast<AnyResource>(src)->getContent();
    }
    if (realsource == nullptr)
        return false;

    auto newContent = tr.lock()->sharedResource(realsource->getBaseId());
    if (newContent == nullptr)
        return false;
    newContent->copy(realsource);
    content = newContent;
    return true;
}

json AnyResource::to_json() const
{
    if (content)
        return content->to_json();
    return json("any (empty)");
}

size_t AnyResource::getBaseId()
{
    if (content == nullptr)
        return 0;
    return content->getBaseId();
}
