
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <odCore/db/Class.h>
#include <odCore/db/Database.h>
#include <odCore/db/DbManager.h>
#include <odCore/db/DependencyTable.h>
#include <odCore/db/Model.h>

#include <odCore/rfl/Field.h>

static std::string typeIdToString(uint32_t type)
{
    std::string name;

    switch(type & 0xff)
    {
    case static_cast<uint32_t>(odRfl::Field::Type::INTEGER):
        name = "Int";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::FLOAT):
        name = "Float";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::CLASS):
        name = "ClassRef";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::MODEL):
        name = "ModelRef";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::SOUND):
        name = "SoundRef";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::ENUM):
        name = "Enum";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::CHAR_CHANNEL):
        name = "CharChannel";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::ANIMATION):
        name = "AnimationRef";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::STRING):
        name = "String";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::SEQUENCE):
        name = "SequenceRef";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::TEXTURE):
        name = "TextureRef";
        break;

    case static_cast<uint32_t>(odRfl::Field::Type::COLOR):
        name = "Color";
        break;
    };

    if(type & 0x1000)
    {
        name += "[]";
    }

    return name;
}

static void printClassStats(std::shared_ptr<odDb::Database> db, odDb::Class &c)
{
    std::cout << "[" << c.getName() << "]" << std::endl
              << "ID: " << c.getAssetId() << std::endl
              << "RFL class ID: 0x" << std::hex << c.getRflClassId() << std::dec << std::endl;

    if(!c.hasModel())
    {
        std::cout << "Model: none" << std::endl;

    }else
    {
        auto modelDb = (c.getModelRef().dbIndex == 0) ? db : db->getDependencyTable()->getDependency(c.getModelRef().dbIndex);
        auto model = db->getDependencyTable()->loadAsset<odDb::Model>(c.getModelRef());

        if(modelDb != nullptr && model != nullptr)
        {
            std::cout << "Model: " << c.getModelRef() << " ('" << model->getName() << "' from DB '" << modelDb->getShortName() << "')" << std::endl;

        }else
        {
            std::cout << "Model: " << c.getModelRef() << " (invalid reference or failed to load)" << std::endl;
        }
    }

    std::cout << "Fields: " << std::endl;
    for(auto &field : c.getFieldLoader().getFieldEntries())
    {
        std::cout << "  " << std::left << std::setw(16) << typeIdToString(field.fieldType) << " '" << field.fieldName << "'" << std::endl;
    }

    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout << "Usage: classStat <path to db file>" << std::endl;
        return -1;
    }

    std::string dbPath = argv[1];

    odDb::DbManager dbManager;

    auto db = dbManager.loadDatabase(dbPath);

    std::vector<od::RecordId> classes;
    auto classFactory = db->getClassFactory();
    if(classFactory == nullptr || classFactory->listAssets(classes) == 0)
    {
        std::cout << "Database has no classes" << std::endl;
        return 0;
    }

    for(auto classId : classes)
    {
        auto loadedClass = db->loadClass(classId);
        printClassStats(db, *loadedClass);
    }

    return 0;
}
