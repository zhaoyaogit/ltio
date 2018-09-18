//
// Created by gh on 18-9-17.
//

#ifndef LIGHTINGIO_READER_FACTORY_H
#define LIGHTINGIO_READER_FACTORY_H

#include "loader.h"
#include <string>
#include <base/memory/lazy_instance.h>

namespace component {
namespace sl {

class LoaderFactory{
public:
  static LoaderFactory& Instance();
  Loader* CreateLoader(LoaderDelegate*, Json&);
};

}}
#endif //LIGHTINGIO_READER_FACTORY_H
