#include <procgen/interpret/function.h>

namespace ProcGen {

bool Function::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;
    return false;
}

bool Function::bindInput(size_t id, std::shared_ptr<Function> func)
{
    this->_inputs[id] = func;
    return true;
}
bool Function::bindOutput(std::shared_ptr<Resource> res)
{
    this->_output = res;
    return true;
}

bool Function::_doInputs(RunStatus& stat)
{
    for (auto& x : this->_inputs) {
        if ((*(x.second))(stat) == true) {
            LOG_DEBUG("Return triggered, coming home.\n");
            return true;
        }
    }
    return false;
}

bool HandleFunction::operator()(RunStatus& stat)
{
    return false;
}

bool PrintJson::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;
    LOG_INFO("PrintJson %s\n", _getInput(0)->getOutput()->to_json().dump().c_str());
    return false;
}

bool CollectionAppend::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;
    std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput())->append(_getInput(1)->getOutput());
    return false;
}

bool CollectionIndex::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    // TODO check if 0 is collection and 1 is size_t
    auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
    this->bindOutput(res->at(*(int*)_getInput(1)->getOutput()->getData()));
    return false;
}
bool CollectionLength::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    // TODO check if 0 is collection and 1 is size_t
    auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
    *(int*)this->getOutput()->getData() = res->length();
    return false;
}

bool CollectionRemove::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    // TODO check if 0 is collection and 1 is size_t
    auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
    res->remove(*(int*)_getInput(1)->getOutput()->getData());
    return false;
}
bool CollectionClear::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
    res->clear();
    return false;
}

bool CompositeGet::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    // TODO check if 0 is collection and 1 is size_t
    auto res = std::dynamic_pointer_cast<CompositeResource>(_getInput(0)->getOutput());
    size_t index = *(int*)_getInput(1)->getOutput()->getData();
    this->bindOutput(res->getComponent(index));
    return false;
}
bool CompositeSet::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    // TODO check if 0 is collection and 1 is size_t
    auto res = std::dynamic_pointer_cast<CompositeResource>(_getInput(0)->getOutput());
    size_t index = *(int*)_getInput(1)->getOutput()->getData();
    res->getComponent(index)->copy(_getInput(2)->getOutput());
    return false;
}
bool FloatToBool::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    bool out = (bool)*(float*)_getInput(0)->getOutput()->getData();
    *(bool*)(getOutput()->getData()) = out;
    return false;
}

bool GenericCopy::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;
    _getInput(0)->getOutput()->copy(_getInput(1)->getOutput());
    this->bindOutput(_getInput(0)->getOutput());
    return false;
}
bool Convert::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;
    if (_getInput(0)->getOutput()->getBaseId() != type) {
        // error = types doesn't match
        throw std::runtime_error("Convert<>: types don't match");
    }
    this->bindOutput(_getInput(0)->getOutput());
    return false;
}
bool GetTypeId::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;
    // Write TypeId to output resource
    *(int*)this->getOutput()->getData() = (_getInput(0)->getOutput()->getBaseId());
    return false;
}
bool GenerateUniform::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    // a = 0
    // b = 1

    float a = *(float*)_getInput(0)->getOutput()->getData();
    float b = *(float*)_getInput(1)->getOutput()->getData();

    float x = rand() / (float(RAND_MAX) + 1.0);
    x = x * (b - a) + a;
    *(float*)this->getOutput()->getData() = x;
    return false;
}

bool GenerateRandom::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    *(float*)this->getOutput()->getData() = rand() / (float(RAND_MAX) + 1.0);
    return false;
}

bool SetRandomSeed::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    srand(*(int*)_getInput(0)->getOutput()->getData());
    return false;
}

bool ConstructCollection::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    LOG_DEBUG("Construct \n");
    assert(this->getOutput() != nullptr);

    auto resourceType = this->getOutput()->getResourceType();
    if (resourceType == ResourceType::COLLECTION) {
        LOG_DEBUG("Constructing collection\n");
        auto output = std::dynamic_pointer_cast<CollectionResource>(this->getOutput());
        size_t countOfFunctionInputs = this->getCountOfInputs();
        output->clear();
        for (size_t i = 0; i < countOfFunctionInputs; i++) {
            // append at the end of collection
            output->append(this->_getInput(i)->getOutput());
        }
    }

    return false;
}

size_t Construct::getCountOfComponents()
{
    auto output = std::dynamic_pointer_cast<CompositeResource>(this->getOutput());
    return output->getComponentCount();
}

bool Construct::operator()(RunStatus& stat)
{
    if (_doInputs(stat))
        return true;

    LOG_DEBUG("Construct \n");
    assert(this->getOutput() != nullptr);

    auto resourceType = this->getOutput()->getResourceType();
    if (resourceType == ResourceType::COMPOSITE) {
        auto output = std::dynamic_pointer_cast<CompositeResource>(this->getOutput());
        size_t count = output->getComponentCount();
        size_t countOfFunctionInputs = this->getCountOfInputs();

        // presume correct match of arguments and parameters
        assert(count == countOfFunctionInputs);
        static_cast<void>(countOfFunctionInputs);

        for (size_t i = 0; i < count; i++) {
            LOG_DEBUG("Construct atrib %d\n", i);
            output->getComponent(i)->copy(this->_getInput(i)->getOutput());
        }
    }
    return false;
}

bool Construct::bindInput(size_t id, std::shared_ptr<Function> func)
{
    auto output = std::dynamic_pointer_cast<CompositeResource>(this->getOutput());
    assert(output != nullptr);

    if (id >= output->getComponentCount())
        return false;
    // detect type consistency
    if (!func->getOutput()->hasSameType(output->getComponent(id))) {
        return false;
    }
    Function::bindInput(id, func);
    return true;
}
}
