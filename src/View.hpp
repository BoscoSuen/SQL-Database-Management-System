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
#include "StorageBlock.hpp"
#include "Storage.hpp"
#include "Database.hpp"

using namespace std;

namespace ECE141 {
  //completely generic view, which you will subclass to show information
  class View {
  public:
    virtual         ~View() {}
    virtual bool    show(std::ostream &aStream);
  };

  /*
  class DescView : public View {
  public:
    DescView(Storage& storage);

    ~DescView() {};
    bool            show(std::ostream &aStream);

  protected:
    ostream               stream;
    Storage&              storage;
  };
   */

}

#endif /* View_h */
