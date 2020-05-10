//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
//#include <variant>
#include <unordered_map>
#include "Storage.hpp"
#include "Database.hpp"

namespace ECE141 {
//    using varType = std::variant<bool , float , int , /*time ,*/ std::string>;

    enum class DataType {
        no_type='N', bool_type='B', datetime_type='D', float_type='F', int_type='I',  varchar_type='V',
    };

    class Attribute : public Storable {
    protected:
        std::string   name;
        DataType      type;

        bool          autoIncrease;
        bool          primary;
        bool          nullable;
        bool          hasDefault;
        std::string   defaultValue;
      
        std::string   varCharOther;
        //STUDENT: What other data should you save in each attribute?

    public:

        Attribute(DataType aType=DataType::no_type);
        Attribute(std::string aName, DataType aType);
        Attribute(const Attribute &aCopy);
        ~Attribute();

        Attribute&    setName(std::string &aName);
        Attribute&    setType(DataType aType);

        bool          isValid(); //is this schema valid? Are all the attributes value?

        const std::string&  getName() const {return name;}
        DataType            getType() const {return type;}
        string              getTypeString();

        bool                getAutoIncreasing() const {return autoIncrease;};
        void                setAutoIncreasing(bool a) {autoIncrease = a;};

        bool                getPrimary() const {return primary;};
        void                setPrimary(bool a) {primary = a;};

        bool                getNullable() const {return nullable;};
        void                setNullable(bool a) {nullable = a;};

        bool                getHasDefault() const {return hasDefault;};
        void                setHasDefault(bool a) {hasDefault = a;};

        std::string         getDefaultValue() const {return defaultValue;};
        bool                setDefaultValue(std::string a) ;
      
        void                setVarCharOther(std::string a) {varCharOther = a;};
        std::string         getVarCharOther() const {return varCharOther;};

        StatusResult  encode(std::ostringstream &aWriter) const;
      
        static StatusResult decode(Attribute& att, std::string str);
        //STUDENT: are there other getter/setters to manage other attribute properties?

    };


}


#endif /* Attribute_hpp */
