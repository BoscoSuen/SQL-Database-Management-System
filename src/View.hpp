//
//  View.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef View_h
#define View_h

#include <iostream>
#include <unordered_map>
#include "StorageBlock.hpp"
#include "Storage.hpp"
#include "Schema.hpp"
#include "Database.hpp"
#include "Value.hpp"
#include "Attribute.hpp"

using namespace std;

namespace ECE141 {
    class Schema;

    //completely generic view, which you will subclass to show information
    class View {
    public:
      virtual         ~View() {}
      virtual bool    show(std::ostream &aStream) = 0;
      virtual void    printFormatedData(std::string str , int leng);
      virtual void    printRowSeparator();
      virtual void    printRow(std::vector<std::string> array);
    protected:
      std::vector<int>      length;
      int                   total;
    };

    class DescDBView : public View {
    public:
      DescDBView(Storage& storage);

      ~DescDBView() {};
      bool            show(std::ostream &aStream);

    protected:
      ostream               stream;
      Storage&              storage;
    };

    class ShowTableView : public View {
    public:
      ShowTableView(Storage& storage);

      ~ShowTableView() {};
      bool            show(std::ostream & aStream);

    protected:
      ostream               stream;
      Storage&              storage;
    };

    class DescTableView : public View {
    public:
      DescTableView(Storage& storage);
      DescTableView(Schema& aSchema);
      ~DescTableView() {};
      bool            show(std::ostream & aStream);

    protected:
      ostream               stream;
      Schema&               schema;
    };

}

#endif /* View_h */
